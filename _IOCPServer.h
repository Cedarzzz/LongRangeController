#pragma once
#include <windows.h>
#include <winsock2.h>
#include <mstcpip.h>
#include <iostream>
#include "_CriticalSection.h"
#include "_CArray.h"
#include "zconf.h"
#include "zlib.h"
#include "Common.h"

using namespace std;
#pragma comment(lib,"WS2_32.lib")

#define PACKET_FLAG_LENGTH 5
#define PACKET_LENGTH 0x2000      //Alessio
#define PACKET_HEADER_LENGTH 13   //Alessio[���ݰ��ܳ�(4)][ԭʼ�����ܳ�(4)]

#define MAX_SEND_BUFFER 0x2000
#define MAX_RECEIVE_BUFFER 0x2000

typedef struct _CONTEXT_OBJECT_
{
	SOCKET ClientSocket;          //Serverʹ�ø��׽�����Ŀ��Client����ͨ��
	WSABUF ReceiveWsaBuffer;      //�����û���������� �ýṹû���ڴ� ��m_BufferData����
	WSABUF SendWsaBuffer;         //��ͻ���������  
	_CArray m_ReceivedCompressedBufferData;     //���յ���ѹ��������m_BufferData�������ó�Ա��
	_CArray m_ReceivedDecompressedBufferData;   //���յ���ѹ�������ݽ��еĽ�ѹ������
	_CArray m_SendCompressedBufferData;
	char BufferData[PACKET_LENGTH];       //�û��������ݵ��ڴ�  ��OnAccept��������ReceiveWsaBuffer����
	int DialogID;
	HANDLE DialogHandle;
	VOID InitMember()
	{
		ClientSocket = INVALID_SOCKET;
		memset(BufferData, 0, sizeof(char)*PACKET_LENGTH);
		memset(&ReceiveWsaBuffer, 0, sizeof(WSABUF));
		memset(&SendWsaBuffer, 0, sizeof(WSABUF));
		DialogID = 0;
		DialogHandle = NULL;
	}
	
}CONTEXT_OBJECT,*PCONTEXT_OBJECT;

enum PACKET_TYPE
{
	IO_INITIALIZE,  //��ʼ��
	IO_RECEIVE,     //����	
	IO_SEND,        //����
	IO_IDLE         //����
};
class OVERLAPPEDEX
{
public:
	OVERLAPPED m_Overlapped;
	PACKET_TYPE m_PackType;

	OVERLAPPEDEX(PACKET_TYPE PackType)
	{
		ZeroMemory(this, sizeof(OVERLAPPEDEX));
		m_PackType = PackType;
	}
};

typedef void(CALLBACK *LPFN_WINDOWNOTIFYPROCEDURE)(PCONTEXT_OBJECT ContextObject);
//����һ����������
typedef CList<PCONTEXT_OBJECT> CONTEXT_OBJECT_LIST;

class _CIOCPServer
{
public:
	_CIOCPServer();
	~_CIOCPServer();
private:
	SOCKET m_ListenSocket;                //�����׽���
	HANDLE m_ListenThreadHandle;          //�����߳̾��
	HANDLE m_ListenEventHandle;           //�����¼�

	CRITICAL_SECTION m_CriticalSection;   //�ؼ������
	HANDLE m_CompletionPortHandle;        //��ɶ˿ھ��
	HANDLE m_KillEventHandle;             //�����߳��˳���ѭ��
	BOOL   m_IsWorking;                   //�����̵߳�ѭ����־
	CONTEXT_OBJECT_LIST m_FreeContextObjectList;         //�ڴ��ģ��
	CONTEXT_OBJECT_LIST m_ConnectionContextObjectList;   //�����û����±������б�
	long  m_KeepAliveTime;                               //�������

	ULONG m_ThreadPoolMin;
	ULONG m_ThreadPoolMax;
	ULONG m_WorkThreadCount;
	ULONG m_CurrentThreadCount;
	ULONG m_BusyThreadCount;
	char m_PacketHeaderFlag[PACKET_FLAG_LENGTH];     //���ݰ�ƥ��
	LPFN_WINDOWNOTIFYPROCEDURE m_WindowNotifyProcedure;  //�����ɴ������д��ݹ����ĺ���ָ��

public:
	BOOL ServerRun(USHORT ListenPort, LPFN_WINDOWNOTIFYPROCEDURE WindowNotifyProcedure);    //��������
	static DWORD WINAPI ListenThreadProcedure(LPVOID ParameterData);  //�����̴߳���
	void _CIOCPServer::OnAccept();        //��������ͨ���׽���ƥ��
	PCONTEXT_OBJECT _CIOCPServer::AllocateContextObject();
	VOID _CIOCPServer::RemoveContextObject(CONTEXT_OBJECT* ContextObject);   //�Ƴ����±��������ݽṹ
	VOID _CIOCPServer::MoveContextObjectToFreePool(CONTEXT_OBJECT* ContextObject);  //�����ڴ�ṹ�������ڴ��(��RemoveContextObject����)
	//Ͷ�����������ݵ��첽����
	VOID _CIOCPServer::PostReceive(CONTEXT_OBJECT* ConTextObject);
	BOOL _CIOCPServer::InitializeIOCP(VOID);  //��ʼ��
	static DWORD WINAPI WorkThreadProcedure(LPVOID ParameterData);
	BOOL _CIOCPServer::HandleIO(PACKET_TYPE PacketType, PCONTEXT_OBJECT ContextObject, DWORD BufferLength);
	BOOL _CIOCPServer::OnInitializing(PCONTEXT_OBJECT ContextObject, DWORD BufferLength);  //��Ϊ���Դ���
	//�����õ������ݶ˵�����
	BOOL _CIOCPServer::OnReceiving(PCONTEXT_OBJECT ContextObject, DWORD BufferLength);

	//�������ݰ�
	VOID _CIOCPServer::OnPrepareSending(CONTEXT_OBJECT* ContextObject, PBYTE BufferData, ULONG BufferLength);
	//�����ݷ��͵��ͻ���
	BOOL _CIOCPServer::OnSending(CONTEXT_OBJECT* ContextObject, ULONG BufferLength);
};

