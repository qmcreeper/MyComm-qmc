
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
	bool openport(char *);//�򿪴���
	bool setupdcb(int);//����DCB
	bool setuptimeout(DWORD ,DWORD ,DWORD ,DWORD ,DWORD);//���ó�ʱ����
	bool WriteChar(BYTE * ,DWORD);//д������
	bool ReceiveChar();//��������
	UCHAR AutoReadport();//�Զ���ȡ���ں�
	bool AutoInit();
	//oid MyCommTsetExample();
	/*
	GetCommState               ��⴮������
	SetCommState               ���ô���
	BuilderCommDCB             ���ַ����е�ֵ������豸���ƿ�
	GetCommTimeouts            ���ͨ�ų�ʱ����
	SetCommTimeouts            ����ͨ�ų�ʱ����
	SetCommMask                �趨������¼�
	WaitCommEvent              �ȴ�������¼�����
	WaitForMultipleObjects     �ȴ������������Ľ��
	WriteFile                  ��������
	ReadFile                   ��������
	GetOverlappedResult        ��������ص����첽���������
	PurgeComm                  ��մ��ڻ�����,�˳�������ز���
	ClearCommError             ���´���״̬�ṹ��,��������д���Ӳ������
	CloseHandle                �رմ��п�
	*/
};

#endif