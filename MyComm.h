
#ifndef _MYCOMM_H_
#define _MYCOMM_H_

#include "windows.h"
#include <afx.h>

//#include <iostream>
//using namespace std;



class MyComm
{
private:
	unsigned int BytesSentNum;
	DWORD MessageLength;
public:
	HANDLE hComm;
	OVERLAPPED m_ov;
	COMSTAT comstat;
	DWORD   m_dwCommEvents;
	CString data;
	bool openport(char *);//打开串口
	bool setupdcb(int);//设置DCB
	bool setuptimeout(DWORD ,DWORD ,DWORD ,DWORD ,DWORD);//设置超时限制
	bool WriteChar(BYTE * ,DWORD);//写入数据
	bool ReceiveChar();//接收数据
	UCHAR AutoReadport();//自动读取串口号
	bool AutoInit();
	//oid MyCommTsetExample();
	/*
	GetCommState               检测串口设置
	SetCommState               设置串口
	BuilderCommDCB             用字符串中的值来填充设备控制块
	GetCommTimeouts            检测通信超时设置
	SetCommTimeouts            设置通信超时参数
	SetCommMask                设定被监控事件
	WaitCommEvent              等待被监控事件发生
	WaitForMultipleObjects     等待多个被监测对象的结果
	WriteFile                  发送数据
	ReadFile                   接收数据
	GetOverlappedResult        返回最后重叠（异步）操作结果
	PurgeComm                  清空串口缓冲区,退出所有相关操作
	ClearCommError             更新串口状态结构体,并清除所有串口硬件错误
	CloseHandle                关闭串行口
	*/
};

#endif