#include "stdafx.h"
#include "MyComm.h"



///////////////////////////////////////
//打开串口
bool MyComm::openport(char *port)
{
	HANDLE hComm;
	hComm = CreateFile(port, //串口号
                       GENERIC_READ | GENERIC_WRITE, //允许读写
                       0, //通讯设备必须以独占方式打开
                       0, //无安全属性
                       OPEN_EXISTING, //通讯设备已存在
                       FILE_FLAG_OVERLAPPED,//异步I/O，同步改为0
                       0); //通讯设备不能用模板打开
	if (hComm == INVALID_HANDLE_VALUE)//INVALID_HANDLE_VALUE表示无效的句柄值 
	{
		CloseHandle(hComm);//句柄无效时关闭
		return false;
	}
	else
	{
		return true;
	}
}

//设置DCB,先获取DCB配置，再设置，最后看是否设置好
//输入：波特率rate_arg
bool MyComm::setupdcb(int rate_arg, int parity)
{
	DCB  dcb;//声明设备控制块结构
	int rate= rate_arg;
	memset(&dcb,0,sizeof(dcb));//在一段内存块中填充某个给定的值，是对较大的结构体或数组进行清零操作的一种最快方法
	if(!GetCommState(hComm,&dcb))//获取当前DCB配置
	{
		return FALSE;
	}
	// set DCB to configure the serial port
	dcb.DCBlength=sizeof(dcb);                //DCB结构体大小
	/* ---------- Serial Port Config ------- */
    dcb.BaudRate        = rate;               //波特率
	dcb.fParity         = 0;                  //是否进行奇偶校验
    dcb.Parity          = parity;           //奇偶校验 值0~4分别对应无校验、奇校验、偶校验、校验置位、校验清零
    dcb.StopBits        = ONESTOPBIT;         //停止位数
    dcb.ByteSize        = 8;                  //数据宽度，一般为8，有时候为7
    dcb.fOutxCtsFlow    = 0;                  //CTS线上的硬件握手
    dcb.fOutxDsrFlow    = 0;                  //DSR线上的硬件握手
    dcb.fDtrControl     = DTR_CONTROL_DISABLE;//DTR控制
    dcb.fDsrSensitivity =0;                   //DSR sensitivity
    dcb.fRtsControl     = RTS_CONTROL_DISABLE;
    dcb.fOutX           = 0;                  //是否使用XON/XOFF协议
    dcb.fInX            = 0;                  //是否使用XON/XOFF协议
	/* --------------- misc parameters ----- */
    dcb.fErrorChar      = 0;
    dcb.fBinary         = 1;                  //是否是二进制，一般设置为TRUE
    dcb.fNull           = 0; 
    dcb.fAbortOnError   = 0;
    dcb.wReserved       = 0;
    dcb.XonLim          = 2;                  //设置在XON字符发送之前inbuf中允许的最少字节数
    dcb.XoffLim         = 4;                  //在发送XOFF字符之前outbuf中允许的最多字节数
    dcb.XonChar         = 0x13;               //设置表示XON字符的字符,一般是采用0x11这个数值            
    dcb.XoffChar        = 0x19;               //设置表示XOFF字符的字符,一般是采用0x13这个数值
    dcb.EvtChar         = 0;
    // set DCB
    if(!SetCommState(hComm,&dcb))
	{
        return false;
	}
    else
	{
        return true;
	}
}

 
//设置超时限制，避免阻塞现象
bool MyComm::setuptimeout(DWORD ReadInterval,DWORD ReadTotalMultiplier,
						  DWORD ReadTotalconstant,DWORD WriteTotalMultiplier,
						  DWORD WriteTotalconstant)
{
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout=ReadInterval;                //以毫秒为单位指定通信线上两个字符到达之间的最大时间
    timeouts.ReadTotalTimeoutConstant=ReadTotalconstant;      //以毫秒为单位指定一个乘数，该乘数用来计算读操作的总限时时间
    timeouts.ReadTotalTimeoutMultiplier=ReadTotalMultiplier;  //以毫秒为单位指定一个常数，用于计算读操作的总限时时间
    timeouts.WriteTotalTimeoutConstant=WriteTotalconstant;
    timeouts.WriteTotalTimeoutMultiplier=WriteTotalMultiplier;
    if(!SetCommTimeouts(hComm, &timeouts))
	{
        return false;
	}
    else
	{
        return true;
	}
}


//接收数据
bool MyComm::ReceiveChar()
{
	data.Empty();
	BOOL  bRead = TRUE;
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	char RXBuff;
	CString RXBuff2Cst;

	for (;;)
	{
		bResult= ClearCommError(hComm, &dwError, &comstat);// 在使用ReadFile函数进行读操作前，应先使用ClearCommError函数清除错误
        if (comstat.cbInQue ==0)// COMSTAT结构返回串口状态信息
			//只用到了cbInQue成员变量，该成员变量的值代表输入缓冲区的字节数
            continue;
        if (bRead)
        {
			bResult = ReadFile( hComm,      // Handle to COMM port串口的句柄
							    &RXBuff,    //RX Buffer Pointer读入的数据存储的地址，即读入的数据将存储在以该指针的值为首地址的一片内存区
								1,          //Read one byte要读入的数据的字节数,
								&BytesRead, // Stores number of bytes read,指向一个DWORD数值，该数值返回读操作实际读入的字节数
								&m_ov);     //pointer to the m_ov structure重叠操作时，该参数指向一个OVERLAPPED结构，同步操作时，该参数为NULL
			//cout<<RXBuff;
			RXBuff2Cst.Format(_T("%d"),data,RXBuff);
			data += RXBuff2Cst;
			if (!bResult)   //当ReadFile和WriteFile返回FALSE时，不一定就是操作失败，线程应该调用GetLastError函数分析返回的结果
			{
			switch (dwError =GetLastError())
				{
				case ERROR_IO_PENDING:
					{
						bRead = FALSE;
						break;
					}
					default:
					{
						break;
					}
				}
			}
			else
			{
				bRead = TRUE;
			}
		}//endif (bRead)
        if (!bRead)
        {
			bRead = TRUE;
			bResult =GetOverlappedResult(hComm,   // Handle toCOMM port
                                         &m_ov,   // Overlappedstructure
										 &BytesRead,      // Stores number of bytes read
                                         TRUE);           // Wait flag
        }
    }
}

//写入数据
bool MyComm::WriteChar(BYTE * m_szWriteBuffer,DWORD m_nToSend)
{
	
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;
	DWORD BytesSent = 0;
	HANDLE  m_hWriteEvent;
	ResetEvent(m_hWriteEvent); 
	if (bWrite)
	{
		m_ov.Offset = 0;
		m_ov.OffsetHigh = 0;// Clear buffer
		bResult=WriteFile(hComm, // Handle toCOMM Port, 串口的句柄
						m_szWriteBuffer,// Pointer to message buffer in calling finction即以该指针的值为首地址的nNumberOfBytesToWrite个字节的数据将要写入串口的发送数据缓冲区
						m_nToSend,      // Length of message to send, 要写入的数据的字节数
						&BytesSent,     //Where to store the number of bytes sent指向指向一个DWORD数值，该数值返回实际写入的字节数
						&m_ov );        //Overlapped structure重叠操作时，该参数指向一个OVERLAPPED结构，同步操作时，该参数为NULL
		if(!bResult) // 当ReadFile和WriteFile返回FALSE时，不一定就是操作失败，线程应该调用GetLastError函数分析返回的结果
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
				case ERROR_IO_PENDING: //GetLastError函数返回ERROR_IO_PENDING。这说明重叠操作还未完成
				{
					// continue to GetOverlappedResults()
					BytesSent = 0;
					bWrite = FALSE;
					break;
				}		
				default:
				{
					// all other error codes
					break;
				}
			}
		}
	}//endif(bWrite)
	if(!bWrite)
	{
		bWrite = TRUE;
		bResult = GetOverlappedResult(hComm,    //Handle to COMM port
									&m_ov,      // Overlapped structure
									&BytesSent,     // Stores number of bytessent
									TRUE);         //Wait flag deal with the error code
		/*if (!bResult)
		{
			cout<<"GetOverlappedResults() in WriteFile()"<<endl;
		}*/
	} //endif (!bWrite)
	// Verify that the data size send equals what we tried to send
    if(BytesSent != m_nToSend)
	{
		/*cout<<"WARNING: WriteFile() error.. Bytes Sent:"<<BytesSent
			<<";MessageLength:"<<strlen((char*)m_szWriteBuffer)<<endl;*/
		BytesSentNum = BytesSent;
		MessageLength = strlen((char*)m_szWriteBuffer);
	}
	return true;
}

char * MyComm::AutoReadport()
{
	int i = 0;
	CHAR Name[25]; 
	UCHAR szPortName[25]; 
	LONG Status; 
	DWORD dwIndex = 0; 
	DWORD dwName; 
	DWORD dwSizeofPortName; 
	DWORD Type;
	HKEY hKey; 
	CString strSerialList[256];  // 临时定义 256 个字符串组，因为系统最多也就 256 个
	LPCTSTR data_Set="HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
	dwName = sizeof(Name); 
	dwSizeofPortName = sizeof(szPortName);
	//long ret0 = (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey)); 
	long ret0 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey); //打开一个制定的注册表键,成功返回ERROR_SUCCESS即“0”值
	if(ret0 == ERROR_SUCCESS) 
	{
		do 
		{
			Status = RegEnumValue(hKey, dwIndex++, Name, &dwName, NULL, &Type, szPortName, &dwSizeofPortName);//读取键值 
			if((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA)) 
			{ 
				strSerialList[i] = CString(szPortName);       // 串口字符串保存 
				printf("serial:%s\n",strSerialList[i]);
				i++;// 串口计数 
				//--可能出现多个串口，后面会修改
			} 
			//每读取一次dwName和dwSizeofPortName都会被修改 			//注意一定要重置,否则会出现很离奇的错误,本人就试过因没有重置,出现先插入串口号大的（如COM4）,再插入串口号小的（如COM3），此时虽能发现两个串口，但都是同一串口号（COM4）的问题，同时也读不了COM大于10以上的串口 
			dwName = sizeof(Name); 
			dwSizeofPortName = sizeof(szPortName); 
		} while((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA)); 
 
		 //RegCloseKey(hKey);
		if(strSerialList[0])
		{
			return ((LPSTR)(LPCSTR)strSerialList[0]);
		}
	}
	return 0;
}

//自动初始化
bool MyComm::AutoInit(int m_portRate, int m_parity)
{
	if(openport(AutoReadport())
		||setupdcb(m_portRate, m_parity)
		||setuptimeout(0,0,0,0,0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//测试示例
/*
void MyComm::MyCommTsetExample()
{
	if(openport("com6"))
		cout<<"open comport success"<<endl;
	if(setupdcb(9600))
		cout<<"setupDCB success"<<endl;
	if(setuptimeout(0,0,0,0,0)) //如果所有写超时参数均为0，那么就不使用写超时
		cout<<"setuptimeout success"<<endl;
	PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT |PURGE_TXABORT); // 在读写串口之前，还要用PurgeComm()函数清空缓冲区
		//PURGE_TXABORT   中断所有写操作并立即返回，即使写操作还没有完成。
		//PURGE_RXABORT   中断所有读操作并立即返回，即使读操作还没有完成。
		//PURGE_TXCLEAR   清除输出缓冲区
		//PURGE_RXCLEAR   清除输入缓冲区
	WriteChar("please send data now",20);
	cout<<"received data:"<<endl;
	ReceiveChar();
}
*/