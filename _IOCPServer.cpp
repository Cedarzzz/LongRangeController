#include "stdafx.h"
#include "_IOCPServer.h"


_CIOCPServer::_CIOCPServer()
{
    //��ʼ���׽������
	WSADATA WsaData;
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		return;
	}

	memcpy(m_PacketHeaderFlag, "zengz", PACKET_FLAG_LENGTH);   //Shine-->Alessio
	m_KillEventHandle = NULL;
	m_ListenSocket = INVALID_SOCKET;
	m_ListenThreadHandle = NULL;
	m_ListenEventHandle = WSA_INVALID_EVENT;  //�����¼�

	InitializeCriticalSection(&m_CriticalSection);
	m_CompletionPortHandle = INVALID_HANDLE_VALUE;

	m_ThreadPoolMin = 0;
	m_ThreadPoolMax = 0;
	m_WorkThreadCount = 0;
	m_CurrentThreadCount = 0;
	m_BusyThreadCount = 0;

	m_IsWorking = FALSE;


}


//�����������Ի������ر�
_CIOCPServer::~_CIOCPServer()
{
	Sleep(1);
	//�����¼�ʹ�������˳������̵߳�ѭ��
	SetEvent(m_KillEventHandle);

	//�ȴ������߳��˳�
	WaitForSingleObject(m_ListenThreadHandle, INFINITE);


	//���ո�����Դ
	if (m_ListenSocket != INVALID_SOCKET)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
	}
	if (m_ListenEventHandle != WSA_INVALID_EVENT)
	{
		WSACloseEvent(m_ListenEventHandle);
		m_ListenEventHandle = WSA_INVALID_EVENT;
	}
	if (m_CompletionPortHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_CompletionPortHandle);
		m_CompletionPortHandle = INVALID_HANDLE_VALUE;
	}

	m_ThreadPoolMax = 0;
	m_ThreadPoolMin = 0;
	m_WorkThreadCount = 0;
	m_CurrentThreadCount = 0;
	m_BusyThreadCount = 0;
	m_IsWorking = FALSE;

	//���ٹؼ������
	DeleteCriticalSection(&m_CriticalSection);
	WSACleanup();
}
BOOL _CIOCPServer::ServerRun(USHORT ListenPort, LPFN_WINDOWNOTIFYPROCEDURE WindowNotifyProcedure)
{

	m_WindowNotifyProcedure = WindowNotifyProcedure;
	//�����˳������̵߳��¼�
	m_KillEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_KillEventHandle == NULL)
	{
		return FALSE;
	}

	//����һ�������׽���
	m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_ListenSocket == INVALID_SOCKET)
	{
		return FALSE;
	}

	//����һ�������¼�
	//�¼�ѡ��ģ��
	m_ListenEventHandle = WSACreateEvent();
	if (m_ListenEventHandle == WSA_INVALID_EVENT)     //ע�� �Ұ�"!="�ĳ���"=="   �Ѳ�
	{
		goto Error;
	}
	//�������׽������¼����й���������FD_ACCEPT��FD_CLOSE������
	BOOL IsOk = WSAEventSelect(m_ListenSocket,  
		m_ListenEventHandle,
		FD_ACCEPT | FD_CLOSE);

	if (IsOk == SOCKET_ERROR)
	{
		goto Error;
	}

	//��ʼ��Server������
	SOCKADDR_IN ServerAddress;
	ServerAddress.sin_port = htons(ListenPort);
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = INADDR_ANY;

	//���׽���
	IsOk = bind(m_ListenSocket,
		(sockaddr*)&ServerAddress,
		sizeof(ServerAddress));
	if (IsOk == SOCKET_ERROR)
	{
		goto Error;
	}

	//����
	IsOk = listen(m_ListenSocket, SOMAXCONN);
	if (IsOk == SOCKET_ERROR)
	{
		goto Error;
	}

	//���������߳�
	m_ListenThreadHandle = 
		(HANDLE)CreateThread(NULL,
		0,
		(LPTHREAD_START_ROUTINE)ListenThreadProcedure,
		(void*)this,          //���̻߳ص�����This �������ǵ��̻߳ص��������еĳ�Ա
		0,
		NULL);
	if (m_ListenThreadHandle == INVALID_HANDLE_VALUE)
	{
		goto Error;
	}

	//��ʼ��IOCP(���첽������ɵ�ʱ��)
	//������ɶ˿�
	//���������߳�(�غ�����ɶ˿��ϵȴ��첽��������)
	InitializeIOCP();

	return TRUE;

Error:
	WSACloseEvent(m_ListenEventHandle);
	m_ListenEventHandle = WSA_INVALID_EVENT;
	closesocket(m_ListenSocket);
	m_ListenSocket = INVALID_SOCKET;
	return FALSE;
}
DWORD _CIOCPServer::ListenThreadProcedure(LPVOID ParameterData)
{
	_CIOCPServer* This = (_CIOCPServer*)ParameterData;

	int EventIndex = 0;
	WSANETWORKEVENTS NetWorkEvents;
	while (1)
	{
		EventIndex = WaitForSingleObject(This->m_KillEventHandle, 100);
		EventIndex = EventIndex - WAIT_OBJECT_0;
		if (EventIndex == 0)
		{
			//����������������m_KillEventHandle�¼�
			break;
		}

		DWORD v1;
		//�ȴ������¼����ţ������׽������ţ�
		v1 = WSAWaitForMultipleEvents(1,
			&This->m_ListenEventHandle,
			FALSE,
			100,
			FALSE);

		if (v1 == WSA_WAIT_TIMEOUT)
		{
			//���¼�û������
			continue;
		}

		//������FD_ACCEPT����FD_CLOSE�¼��Ż���е�����
		v1 = WSAEnumNetworkEvents(This->m_ListenSocket,
			//����¼����� ���Ǿͽ����¼�ת����һ�������¼������ж�
			This->m_ListenEventHandle,
			&NetWorkEvents);

		if (v1 == SOCKET_ERROR)
		{
			break;
		}
		if (NetWorkEvents.lNetworkEvents == FD_ACCEPT)    //�����׽������š���>
		{                    //!!!!�˴���BUG  ��&����==?????
			if (NetWorkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
			{
				//����ͻ�����������
				This->OnAccept();

			}
			else
			{
				break;
			}
		}
		else
		{
			//��ɾ��һ���û��Ǵ����ִ�е����ﲻҪ�������˳�ѭ��
			//break;   //������������bug
		}
	}
	return 0;
}
void _CIOCPServer::OnAccept()
{
	int Result = 0;


	//���������û�IP��ַ
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKADDR_IN ClientAddress = { 0 };
	int ClientAddressLength = sizeof(SOCKADDR_IN);

	//ͨ�����ǵļ����׽���������һ����֮���ź�ͨ�ŵ��׽���
	ClientSocket = accept(m_ListenSocket,
		(sockaddr*)&ClientAddress,
		&ClientAddressLength);

	if (ClientSocket == SOCKET_ERROR)
	{
		return;
	}

	//����������Ϊÿһ��������ź�ά����һ����֮���������ݽṹ������Ϊ�û������±�����
	PCONTEXT_OBJECT ContextObject = AllocateContextObject();

	if (ContextObject == NULL)
	{
		closesocket(ClientSocket);
		ClientSocket = INVALID_SOCKET;
		return;
	}

	//��Ա��ֵ
	ContextObject->ClientSocket = ClientSocket;

	//�����ڴ�
	ContextObject->ReceiveWsaBuffer.buf = (char*)ContextObject->BufferData;
	ContextObject->ReceiveWsaBuffer.len = sizeof(ContextObject->BufferData);

	//�����ɵ�ͨ���׽�������ɶ˿ھ�������
	HANDLE Handle = CreateIoCompletionPort((HANDLE)ClientSocket,
		m_CompletionPortHandle, (ULONG_PTR)ContextObject, 0);

	if (Handle != m_CompletionPortHandle)
	{
		delete ContextObject;
		ContextObject = NULL;
		if (ClientSocket != INVALID_SOCKET)
		{
			closesocket(ClientSocket);
			ClientSocket = INVALID_SOCKET;
		}
		return;
	}




	//�������Ӽ��Է������Ƿ���� �����Сʱ���ڴ��׽ӿڵ�����һ����û��
	//���ݽ�����TCP���Զ����Է���һ�����ִ��


	//����: �����׽��ֵ�ѡ� Set KeepAlive ����������� SO_KEEPALIVE
	m_KeepAliveTime = 3;
	BOOL IsOk = TRUE;
	
	if (setsockopt(ContextObject->ClientSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&IsOk, sizeof(IsOk)) != 0)
	{

	}
	//���ó�ʱ��ϸ��Ϣ
	tcp_keepalive v1;
	v1.onoff = 1;                          //���ñ���
	v1.keepalivetime = m_KeepAliveTime;    //����������û�����ݣ�����̽���
	v1.keepaliveinterval = 1000 * 10;      //���Լ��Ϊ10��
	WSAIoctl
		(
			ContextObject->ClientSocket,
			SIO_KEEPALIVE_VALS,
			&v1,
			sizeof(v1),
			NULL,
			0,
			(unsigned long *)&IsOk,
			0,
			NULL
		);

	_CCriticalSection CriticalSection(m_CriticalSection);
	m_ConnectionContextObjectList.AddTail(ContextObject);   //���뵽���ǵ��ڴ��б���

	OVERLAPPEDEX *OverlappedEx = new OVERLAPPEDEX(IO_INITIALIZE);

	IsOk = FALSE;
	//����ɶ˿���Ͷ��һ������
	//�����̻߳�ȴ���ɶ˿ڵ����״̬
	IsOk = PostQueuedCompletionStatus(m_CompletionPortHandle,
		0, (ULONG_PTR)ContextObject, &OverlappedEx->m_Overlapped);

	if ((!IsOk && GetLastError() != ERROR_IO_PENDING))
	{
		//���Ͷ��ʧ��,�����±����Ļ��յ��ڴ�� �Ƴ�
		RemoveContextObject(ContextObject);
		return;
	}

	//�������û�����������ߵ�����
	//����������û�Ͷ��PostRecv����
	PostReceive(ContextObject);
}
VOID _CIOCPServer::PostReceive(CONTEXT_OBJECT* ContextObject)
{
	//�����Ǹ����ߵ��û�Ͷ��һ���������ݵ�����
	//���������õ����(�û���������)
	//�����߳�(�غ�����ɶ˿�)����Ӧ������ProcessIOMessage����
	OVERLAPPEDEX* OverLappedEx = new OVERLAPPEDEX(IO_RECEIVE);

	DWORD ReturnLength;
	ULONG Flags = MSG_PARTIAL;
	int IsOk = WSARecv(ContextObject->ClientSocket,
		&ContextObject->ReceiveWsaBuffer,    //�������ݵ��ڴ�
		1,
		&ReturnLength,                       //TransferBufferLength
		&Flags,
		&OverLappedEx->m_Overlapped,
		NULL);

	if (IsOk == SOCKET_ERROR&&WSAGetLastError() != WSA_IO_PENDING)
	{
		//�����ʹ���
		RemoveContextObject(ContextObject);
	}
}
PCONTEXT_OBJECT _CIOCPServer::AllocateContextObject()
{
	int a = GetLastError();
	PCONTEXT_OBJECT ContextObject = NULL;
	//�����ٽ���
	_CCriticalSection CriticalSection(m_CriticalSection);
	//�ж��ڴ���Ƿ�Ϊ��
	if (m_FreeContextObjectList.IsEmpty() == FALSE)
	{
		//�ڴ��ȡ�ڴ�
		ContextObject = m_FreeContextObjectList.RemoveHead();
	}
	else
	{
		ContextObject = new CONTEXT_OBJECT;
	}
	if (ContextObject != NULL)
	{
		ContextObject->InitMember();  //��ʼ����Ա����
	}
	return ContextObject;
}
VOID _CIOCPServer::RemoveContextObject(CONTEXT_OBJECT* ContextObject)
{
	_CCriticalSection CriticalSection(m_CriticalSection);
	//���ڴ��в��Ҹ��û������±��������ݽṹ
	if (m_ConnectionContextObjectList.Find(ContextObject))
	{
		//ȡ���ڵ�ǰ�׽��ֵ��첽IO��ǰ��δ��ɵ��첽����ȫ������ȡ��
		CancelIo((HANDLE)ContextObject->ClientSocket);
		//�ر��׽���
		closesocket(ContextObject->ClientSocket);
		ContextObject->ClientSocket = INVALID_SOCKET;
		//�жϻ���û���첽�����ڵ�ǰ���׽�����
		while (!HasOverlappedIoCompleted((LPOVERLAPPED)ContextObject))
		{
			Sleep(1);
		}
		//�����ڴ�ṹ�������ڴ��
		MoveContextObjectToFreePool(ContextObject);
	}
}
VOID _CIOCPServer::MoveContextObjectToFreePool(CONTEXT_OBJECT* ContextObject)
{
	_CCriticalSection CriticalSection(m_CriticalSection);

	POSITION Position = m_ConnectionContextObjectList.Find(ContextObject);
	if (Position)
	{
		ContextObject->m_ReceivedCompressedBufferData.ClearArray();
		ContextObject->m_ReceivedDecompressedBufferData.ClearArray();
		ContextObject->m_SendCompressedBufferData.ClearArray();
		memset(ContextObject->BufferData, 0, PACKET_LENGTH);
		m_FreeContextObjectList.AddTail(ContextObject);      //�������ڴ��
		m_ConnectionContextObjectList.RemoveAt(Position);    //���ڴ�ṹ���Ƴ�
	}

}
BOOL _CIOCPServer::InitializeIOCP(VOID)
{
	m_CompletionPortHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (m_CompletionPortHandle == NULL)   //������ɶ˿�
	{
		return FALSE;
	}
	//�̳߳�
	SYSTEM_INFO SystemInfo;
	//���pc���м���
	GetSystemInfo(&SystemInfo);
	m_ThreadPoolMin = 1;
	m_ThreadPoolMax = SystemInfo.dwNumberOfProcessors * 2;

	ULONG  WorkThreadCount = 2;
	HANDLE WorkThreadHandle = NULL;
	for (int i = 0; i < WorkThreadCount; i++)
	{
		WorkThreadHandle = CreateThread(NULL,
			0,
			(LPTHREAD_START_ROUTINE)WorkThreadProcedure,
			(void*)this,
			0,
			NULL);
		if (WorkThreadHandle == NULL)
		{
			CloseHandle(m_CompletionPortHandle);
			return FALSE;
		}
		m_WorkThreadCount++;
		CloseHandle(WorkThreadHandle);
	}
}
DWORD _CIOCPServer::WorkThreadProcedure(LPVOID ParameterData)
{
	_CIOCPServer* This = (_CIOCPServer*)ParameterData;

	HANDLE CompletionPortHandle = This->m_CompletionPortHandle;
	DWORD ReturnLength = 0;    //�ж������ݵõ����
	PCONTEXT_OBJECT ContextObject = NULL;
	LPOVERLAPPED    Overlapped = NULL;
	OVERLAPPEDEX*   OverlappedEx = NULL;

	ULONG v3 = 0;       //���ڹ������߳���Ŀ������
	BOOL v1 = FALSE;


	InterlockedIncrement(&This->m_CurrentThreadCount);  //ԭ����
	InterlockedIncrement(&This->m_BusyThreadCount);     //��������Ŀ

	while (This->m_IsWorking == FALSE)
	{
		InterlockedDecrement(&This->m_BusyThreadCount);
		//�ú�����������������ú�������˵��������õ������
		BOOL IsOk = GetQueuedCompletionStatus(
			CompletionPortHandle,
			&ReturnLength,                 //��ɶ�������
			(PULONG_PTR)&ContextObject,    //���key
			&Overlapped, 60000);

		/*OVERLAPPEDEX *OverlappedEx = new OVERLAPPEDEX(IO_INITIALIZE);
		IsOk = FALSE;
		IsOk = PostQueuedCompletionStatus(m_CompletionPortHandle,
			0, (ULONG_PTR)ContextObject, &OverlappedEx->m_Overlapped);
			*/


		DWORD LastError = GetLastError();
		OverlappedEx = CONTAINING_RECORD(Overlapped, OVERLAPPEDEX, m_Overlapped);

		v3 = InterlockedIncrement(&This->m_BusyThreadCount);

		if (!IsOk && LastError != WAIT_TIMEOUT)
		{
			if (ContextObject && This->m_IsWorking == FALSE&& ReturnLength == 0)
			{
				//���Է��׽��ַ����ر�
				This->RemoveContextObject(ContextObject);
			}
			continue;
		}
		if (!v1)
		{
			if (v3 == This->m_CurrentThreadCount)
			{
				if (v3 < This->m_ThreadPoolMax)
				{
					if (ContextObject != NULL)
					{
						HANDLE ThreadHandle = (HANDLE)CreateThread(NULL,
							0,
							(LPTHREAD_START_ROUTINE)WorkThreadProcedure,
							(void*)This,
							0,
							NULL);
						InterlockedIncrement(&This->m_WorkThreadCount);

						CloseHandle(ThreadHandle);
					}
				}
			}
			//���̳߳�����һ���߳�
			if (!IsOk && LastError == WAIT_TIMEOUT)
			{
				if (ContextObject == NULL)
				{
					{
						if (This->m_CurrentThreadCount > This->m_ThreadPoolMin)
						{
							break;
						}
					}
					v1 = TRUE;
				}
			}
		}
		if (!v1)
		{
			//����õ����(�������� IO_INITIALIZE  IO_RECEIVE)
			
			if (IsOk && OverlappedEx != NULL && ContextObject != NULL)
			{
				try
				{
					//����õ���ɵĴ�����
					This->HandleIO(OverlappedEx->m_PackType, ContextObject, ReturnLength);

					//û���ͷ��ڴ�
					ContextObject = NULL;
				}
				catch(...){}
			}
		}
		if (OverlappedEx)
		{
			delete OverlappedEx;
			OverlappedEx = NULL;
		}
	}
	InterlockedDecrement(&This->m_WorkThreadCount);
	InterlockedDecrement(&This->m_CurrentThreadCount);
	InterlockedDecrement(&This->m_BusyThreadCount);
	return 0;
}
BOOL _CIOCPServer::HandleIO(PACKET_TYPE PacketType, PCONTEXT_OBJECT ContextObject, DWORD BufferLength)
{
	BOOL v1 = FALSE;
	if (IO_INITIALIZE == PacketType)
	{
		//������Ͷ������
		v1 = OnInitializing(ContextObject, BufferLength);
	}

	if (IO_RECEIVE == PacketType)
	{
		//�ͻ��˷�������
		v1 = OnReceiving(ContextObject, BufferLength);
	}

	if (IO_SEND == PacketType)
	{
		//�������ݵ��ͻ���
		v1 = OnSending(ContextObject, BufferLength);
	}
	return v1;
}
BOOL _CIOCPServer::OnInitializing(PCONTEXT_OBJECT ContextObject, DWORD BufferLength)
{

	//MessageBox(NULL, "HelloInit", "HelloInit", 0);

	return TRUE;
}
BOOL _CIOCPServer::OnReceiving(PCONTEXT_OBJECT ContextObject, DWORD BufferLength)
{
	_CCriticalSection CriticalSection(m_CriticalSection);
	try
	{
		if (BufferLength == 0)
		{
			//�Է��ر����׽���
			MessageBox(NULL, "�ر��׽���", "�ر��׽���", 0);
			RemoveContextObject(ContextObject);
			return FALSE;
		}

		//���ӵ������ݿ�����m_ReceivedCompressedBufferData
		ContextObject->m_ReceivedCompressedBufferData.WriteArray(
			(PBYTE)ContextObject->BufferData, BufferLength);

		//��ȡ���ݰ���ͷ��(���ݰ���ͷ���ǲ�����ѹ����)
		while (ContextObject->m_ReceivedCompressedBufferData.GetArrayLength() > PACKET_HEADER_LENGTH)  //?????��m_ReceivedCompressedBufferData����m_ReceivedDecompressedBufferData
		{
			//�������ݰ�ͷ����־
			char v1[PACKET_FLAG_LENGTH] = { 0 };

			//�������ݰ�ͷ����־
			CopyMemory(v1, ContextObject->m_ReceivedCompressedBufferData.GetArray(), PACKET_FLAG_LENGTH);

			//У�����ݰ�ͷ����־
			if (memcmp(m_PacketHeaderFlag, v1, PACKET_FLAG_LENGTH) != 0)
			{
				throw "Bad Buffer";
			}

			//��ȡ���ݰ��ܴ�С
			ULONG PackTotalLength = 0;
			CopyMemory(&PackTotalLength,
				ContextObject->m_ReceivedCompressedBufferData.GetArray(PACKET_FLAG_LENGTH),
				sizeof(ULONG));

			//ȡ�����ݰ��ܳ�
			if (PackTotalLength &&
				(ContextObject->m_ReceivedCompressedBufferData.GetArrayLength()) >= PackTotalLength)
			{
				//[Alessio][ѹ���ĳ���+13][û��ѹ���ĳ���][HelloWorld(��ʵ����)]

				ULONG DecompressedLength = 0;

				//ȡ[Alessio]
				ContextObject->m_ReceivedCompressedBufferData.ReadArray((PBYTE)v1, PACKET_FLAG_LENGTH);
				//ȡ[ѹ���ĳ���+13]
				ContextObject->m_ReceivedCompressedBufferData.ReadArray((PBYTE)&PackTotalLength, sizeof(ULONG));
				//ȡ[û��ѹ���ĳ���]
				ContextObject->m_ReceivedCompressedBufferData.ReadArray((PBYTE)&DecompressedLength, sizeof(ULONG));

				//ֻʣ[HelloWorld(��ʵ����)]

				ULONG CompressedLength = PackTotalLength - PACKET_HEADER_LENGTH;

				//�������ѹ������
				PBYTE CompressedData = new BYTE[CompressedLength];

				//������Ž�ѹ������
				PBYTE DecompressedData = new BYTE[DecompressedLength];

				if (CompressedData == NULL || DecompressedData == NULL)
				{
					throw "Bad Allocate";
				}

				//�����ݰ��л�ȡѹ��������� ��[HelloWorld(��ʵ����)]
				ContextObject->m_ReceivedCompressedBufferData.ReadArray(CompressedData, CompressedLength);


				//��ѹ������
				int IsOk = uncompress(DecompressedData,
					&DecompressedLength, CompressedData, CompressedLength);

				if (IsOk == Z_OK)
				{
					ContextObject->m_ReceivedDecompressedBufferData.ClearArray();
					ContextObject->m_ReceivedCompressedBufferData.ClearArray();

					//������ʵ����
					ContextObject->m_ReceivedDecompressedBufferData.WriteArray(DecompressedData,
						DecompressedLength);

					delete[] CompressedData;
					delete[] DecompressedData;

					m_WindowNotifyProcedure(ContextObject);   //֪ͨ����
				}
				else
				{
					delete[] CompressedData;
					delete[] DecompressedData;
					throw "Bad Buffer";
				}

			}
			else
			{
				break;
			}
		}

		//��һ�ε��첽�����Ѿ��õ���� ����Ͷ���µ��첽����
		PostReceive(ContextObject);
	}
	catch (...)
	{
		ContextObject->m_ReceivedDecompressedBufferData.ClearArray();
		ContextObject->m_ReceivedCompressedBufferData.ClearArray();

		PostReceive(ContextObject);
	}

	return TRUE;
}
//׼����������
VOID _CIOCPServer::OnPrepareSending(CONTEXT_OBJECT* ContextObject, PBYTE BufferData, ULONG BufferLength)
{
	if (ContextObject == NULL)
	{
		return;
	}
	try
	{
		if (BufferLength > 0)
		{
			unsigned long CompressedLength = (double)BufferLength*1.001 + 12;
			LPBYTE CompressedData = new BYTE[CompressedLength];
			int IsOk = compress(CompressedData, &CompressedLength, (LPBYTE)BufferData, BufferLength);

			if (IsOk != Z_OK)
			{
				delete[] CompressedData;
				return;
			}
			ULONG PackTotalLength = CompressedLength + PACKET_HEADER_LENGTH;
			//�������ݰ�ͷ��
			ContextObject->m_SendCompressedBufferData.WriteArray((LPBYTE)m_PacketHeaderFlag,PACKET_FLAG_LENGTH);
			//[Shine]
			ContextObject->m_SendCompressedBufferData.WriteArray((PBYTE)&PackTotalLength,sizeof(ULONG));
			//[Shine][PackTotalLength]
			ContextObject->m_SendCompressedBufferData.WriteArray((PBYTE)&BufferLength, sizeof(ULONG));
			//[Shine][PackTotalLength][BufferLength]
			ContextObject->m_SendCompressedBufferData.WriteArray(CompressedData,CompressedLength);
			//[Shine][PackTotalLength][BufferLength][��ʵ����]
			delete[] CompressedData;
		}
		OVERLAPPEDEX* OverlappedEx = new OVERLAPPEDEX(IO_SEND);

		//��������Ͷ�ݵ���ɶ˿�
		PostQueuedCompletionStatus(m_CompletionPortHandle, 0, (DWORD)ContextObject, &OverlappedEx->m_Overlapped);
	}
	catch (...) {}
}
BOOL _CIOCPServer::OnSending(CONTEXT_OBJECT* ContextObject, ULONG BufferLength)
{
	try
	{
		DWORD Flags = MSG_PARTIAL;
		//����ɵ����ݴ����ݽṹ���Ƴ�
		ContextObject->m_SendCompressedBufferData.RemoveArray(BufferLength);
		//�жϻ��ж���������Ҫ����
		if (ContextObject->m_SendCompressedBufferData.GetArrayLength() == 0)
		{
			//Ϊ0 �������Ѿ��������
			ContextObject->m_SendCompressedBufferData.ClearArray();
			return true;
		}
		else
		{
			//������������
			OVERLAPPEDEX* OverlappedEx = new OVERLAPPEDEX(IO_SEND);
			ContextObject->SendWsaBuffer.buf = (char*)ContextObject->m_SendCompressedBufferData.GetArray();
			ContextObject->SendWsaBuffer.len = ContextObject->m_SendCompressedBufferData.GetArrayLength();
			int IsOk = WSASend(ContextObject->ClientSocket,
				&ContextObject->SendWsaBuffer,
				1,
				&ContextObject->SendWsaBuffer.len,
				Flags,
				&OverlappedEx->m_Overlapped,
				NULL);
			if (IsOk == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				RemoveContextObject(ContextObject);
			}
		}
	}
	catch(...){}
	return FALSE;
}