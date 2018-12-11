#include "stdafx.h"
#include "MyComm.h"



///////////////////////////////////////
//�򿪴���
bool MyComm::openport(char *port)
{
	HANDLE hComm;
	hComm = CreateFile(port, //���ں�
                       GENERIC_READ | GENERIC_WRITE, //�����д
                       0, //ͨѶ�豸�����Զ�ռ��ʽ��
                       0, //�ް�ȫ����
                       OPEN_EXISTING, //ͨѶ�豸�Ѵ���
                       FILE_FLAG_OVERLAPPED,//�첽I/O��ͬ����Ϊ0
                       0); //ͨѶ�豸������ģ���
	if (hComm == INVALID_HANDLE_VALUE)//INVALID_HANDLE_VALUE��ʾ��Ч�ľ��ֵ 
	{
		CloseHandle(hComm);//�����Чʱ�ر�
		return false;
	}
	else
	{
		return true;
	}
}

//����DCB,�Ȼ�ȡDCB���ã������ã�����Ƿ����ú�
//���룺������rate_arg
bool MyComm::setupdcb(int rate_arg, int parity)
{
	DCB  dcb;//�����豸���ƿ�ṹ
	int rate= rate_arg;
	memset(&dcb,0,sizeof(dcb));//��һ���ڴ�������ĳ��������ֵ���ǶԽϴ�Ľṹ�������������������һ����췽��
	if(!GetCommState(hComm,&dcb))//��ȡ��ǰDCB����
	{
		return FALSE;
	}
	// set DCB to configure the serial port
	dcb.DCBlength=sizeof(dcb);                //DCB�ṹ���С
	/* ---------- Serial Port Config ------- */
    dcb.BaudRate        = rate;               //������
	dcb.fParity         = 0;                  //�Ƿ������żУ��
    dcb.Parity          = parity;           //��żУ�� ֵ0~4�ֱ��Ӧ��У�顢��У�顢żУ�顢У����λ��У������
    dcb.StopBits        = ONESTOPBIT;         //ֹͣλ��
    dcb.ByteSize        = 8;                  //���ݿ�ȣ�һ��Ϊ8����ʱ��Ϊ7
    dcb.fOutxCtsFlow    = 0;                  //CTS���ϵ�Ӳ������
    dcb.fOutxDsrFlow    = 0;                  //DSR���ϵ�Ӳ������
    dcb.fDtrControl     = DTR_CONTROL_DISABLE;//DTR����
    dcb.fDsrSensitivity =0;                   //DSR sensitivity
    dcb.fRtsControl     = RTS_CONTROL_DISABLE;
    dcb.fOutX           = 0;                  //�Ƿ�ʹ��XON/XOFFЭ��
    dcb.fInX            = 0;                  //�Ƿ�ʹ��XON/XOFFЭ��
	/* --------------- misc parameters ----- */
    dcb.fErrorChar      = 0;
    dcb.fBinary         = 1;                  //�Ƿ��Ƕ����ƣ�һ������ΪTRUE
    dcb.fNull           = 0; 
    dcb.fAbortOnError   = 0;
    dcb.wReserved       = 0;
    dcb.XonLim          = 2;                  //������XON�ַ�����֮ǰinbuf������������ֽ���
    dcb.XoffLim         = 4;                  //�ڷ���XOFF�ַ�֮ǰoutbuf�����������ֽ���
    dcb.XonChar         = 0x13;               //���ñ�ʾXON�ַ����ַ�,һ���ǲ���0x11�����ֵ            
    dcb.XoffChar        = 0x19;               //���ñ�ʾXOFF�ַ����ַ�,һ���ǲ���0x13�����ֵ
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

 
//���ó�ʱ���ƣ�������������
bool MyComm::setuptimeout(DWORD ReadInterval,DWORD ReadTotalMultiplier,
						  DWORD ReadTotalconstant,DWORD WriteTotalMultiplier,
						  DWORD WriteTotalconstant)
{
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout=ReadInterval;                //�Ժ���Ϊ��λָ��ͨ�����������ַ�����֮������ʱ��
    timeouts.ReadTotalTimeoutConstant=ReadTotalconstant;      //�Ժ���Ϊ��λָ��һ���������ó����������������������ʱʱ��
    timeouts.ReadTotalTimeoutMultiplier=ReadTotalMultiplier;  //�Ժ���Ϊ��λָ��һ�����������ڼ��������������ʱʱ��
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


//��������
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
		bResult= ClearCommError(hComm, &dwError, &comstat);// ��ʹ��ReadFile�������ж�����ǰ��Ӧ��ʹ��ClearCommError�����������
        if (comstat.cbInQue ==0)// COMSTAT�ṹ���ش���״̬��Ϣ
			//ֻ�õ���cbInQue��Ա�������ó�Ա������ֵ�������뻺�������ֽ���
            continue;
        if (bRead)
        {
			bResult = ReadFile( hComm,      // Handle to COMM port���ڵľ��
							    &RXBuff,    //RX Buffer Pointer��������ݴ洢�ĵ�ַ������������ݽ��洢���Ը�ָ���ֵΪ�׵�ַ��һƬ�ڴ���
								1,          //Read one byteҪ��������ݵ��ֽ���,
								&BytesRead, // Stores number of bytes read,ָ��һ��DWORD��ֵ������ֵ���ض�����ʵ�ʶ�����ֽ���
								&m_ov);     //pointer to the m_ov structure�ص�����ʱ���ò���ָ��һ��OVERLAPPED�ṹ��ͬ������ʱ���ò���ΪNULL
			//cout<<RXBuff;
			RXBuff2Cst.Format(_T("%d"),data,RXBuff);
			data += RXBuff2Cst;
			if (!bResult)   //��ReadFile��WriteFile����FALSEʱ����һ�����ǲ���ʧ�ܣ��߳�Ӧ�õ���GetLastError�����������صĽ��
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

//д������
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
		bResult=WriteFile(hComm, // Handle toCOMM Port, ���ڵľ��
						m_szWriteBuffer,// Pointer to message buffer in calling finction���Ը�ָ���ֵΪ�׵�ַ��nNumberOfBytesToWrite���ֽڵ����ݽ�Ҫд�봮�ڵķ������ݻ�����
						m_nToSend,      // Length of message to send, Ҫд������ݵ��ֽ���
						&BytesSent,     //Where to store the number of bytes sentָ��ָ��һ��DWORD��ֵ������ֵ����ʵ��д����ֽ���
						&m_ov );        //Overlapped structure�ص�����ʱ���ò���ָ��һ��OVERLAPPED�ṹ��ͬ������ʱ���ò���ΪNULL
		if(!bResult) // ��ReadFile��WriteFile����FALSEʱ����һ�����ǲ���ʧ�ܣ��߳�Ӧ�õ���GetLastError�����������صĽ��
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
				case ERROR_IO_PENDING: //GetLastError��������ERROR_IO_PENDING����˵���ص�������δ���
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
	CString strSerialList[256];  // ��ʱ���� 256 ���ַ����飬��Ϊϵͳ���Ҳ�� 256 ��
	LPCTSTR data_Set="HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
	dwName = sizeof(Name); 
	dwSizeofPortName = sizeof(szPortName);
	//long ret0 = (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey)); 
	long ret0 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey); //��һ���ƶ���ע����,�ɹ�����ERROR_SUCCESS����0��ֵ
	if(ret0 == ERROR_SUCCESS) 
	{
		do 
		{
			Status = RegEnumValue(hKey, dwIndex++, Name, &dwName, NULL, &Type, szPortName, &dwSizeofPortName);//��ȡ��ֵ 
			if((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA)) 
			{ 
				strSerialList[i] = CString(szPortName);       // �����ַ������� 
				printf("serial:%s\n",strSerialList[i]);
				i++;// ���ڼ��� 
				//--���ܳ��ֶ�����ڣ�������޸�
			} 
			//ÿ��ȡһ��dwName��dwSizeofPortName���ᱻ�޸� 			//ע��һ��Ҫ����,�������ֺ�����Ĵ���,���˾��Թ���û������,�����Ȳ��봮�ںŴ�ģ���COM4��,�ٲ��봮�ں�С�ģ���COM3������ʱ���ܷ����������ڣ�������ͬһ���ںţ�COM4�������⣬ͬʱҲ������COM����10���ϵĴ��� 
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

//�Զ���ʼ��
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

//����ʾ��
/*
void MyComm::MyCommTsetExample()
{
	if(openport("com6"))
		cout<<"open comport success"<<endl;
	if(setupdcb(9600))
		cout<<"setupDCB success"<<endl;
	if(setuptimeout(0,0,0,0,0)) //�������д��ʱ������Ϊ0����ô�Ͳ�ʹ��д��ʱ
		cout<<"setuptimeout success"<<endl;
	PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT |PURGE_TXABORT); // �ڶ�д����֮ǰ����Ҫ��PurgeComm()������ջ�����
		//PURGE_TXABORT   �ж�����д�������������أ���ʹд������û����ɡ�
		//PURGE_RXABORT   �ж����ж��������������أ���ʹ��������û����ɡ�
		//PURGE_TXCLEAR   ������������
		//PURGE_RXCLEAR   ������뻺����
	WriteChar("please send data now",20);
	cout<<"received data:"<<endl;
	ReceiveChar();
}
*/