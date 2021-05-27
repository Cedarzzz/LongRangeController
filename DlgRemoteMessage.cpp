// DlgRemoteMessage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XsServer.h"
#include "DlgRemoteMessage.h"
#include "afxdialogex.h"


// CDlgRemoteMessage �Ի���

IMPLEMENT_DYNAMIC(CDlgRemoteMessage, CDialog)

CDlgRemoteMessage::CDlgRemoteMessage(CWnd* pParent, _CIOCPServer* IOCPServer,
	PCONTEXT_OBJECT ContextObject)
	: CDialog(IDD_DIALOG_REMOTE_MESSAGE, pParent)
{
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;
	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
}

CDlgRemoteMessage::~CDlgRemoteMessage()
{
}

void CDlgRemoteMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DIALOG_REMOTE_MESSAGE_MAIN, m_CEdit_Dialog_Remote_Message_Main);
}


BEGIN_MESSAGE_MAP(CDlgRemoteMessage, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgRemoteMessage ��Ϣ�������


BOOL CDlgRemoteMessage::OnInitDialog()
{
	CDialog::OnInitDialog();
	//����ͼ��
	SetIcon(m_IconHwnd, FALSE);

	BYTE IsToken = CLIENT_GO_ON;
	m_IOCPServer->OnPrepareSending(m_ContextObject, &IsToken, sizeof(BYTE));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgRemoteMessage::OnClose()
{
	CancelIo((HANDLE)m_ContextObject->ClientSocket);
	closesocket(m_ContextObject->ClientSocket);

	CDialog::OnClose();
}


BOOL CDlgRemoteMessage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		//���Esc���˳�����
		if (pMsg->wParam == VK_ESCAPE)
		{
			PostMessage(WM_CLOSE);
			return true;
		}

		//�ж��Ƿ���m_CEdit_Dialog_Remote_Message_Main�ؼ��ϵ���˻س���
		if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_CEdit_Dialog_Remote_Message_Main.m_hWnd)
		{
			OnSending();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}
void CDlgRemoteMessage::OnSending()
{
	//���m_CEdit_Dialog_Remote_Message_Main�ؼ��ϵ����ݳ���
	int BufferLength = m_CEdit_Dialog_Remote_Message_Main.GetWindowTextLength();
	if (!BufferLength)
	{
		return;
	}
	CString v1;
	//���m_CEdit_Dialog_Remote_Message_Main�ؼ��ϵ���������
	m_CEdit_Dialog_Remote_Message_Main.GetWindowText(v1);
	//IOͨ���׽���ֻ֧��char������
	char* BufferData = (char*)VirtualAlloc(NULL, BufferLength, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	memset(BufferData, 0, sizeof(char)*BufferLength);
	sprintf(BufferData, "%s", v1.GetBuffer(0));

	//���m_CEdit_Dialog_Remote_Message_Main�ؼ��ϵ�����
	m_CEdit_Dialog_Remote_Message_Main.SetWindowText(NULL);

	//����ͨ���������еķ������ݺ���
	m_IOCPServer->OnPrepareSending(m_ContextObject,
		(LPBYTE)BufferData, strlen(BufferData));

	if (BufferData != NULL)
	{
		VirtualFree(BufferData, BufferLength, MEM_RELEASE);
		BufferData = NULL;
	}
}
