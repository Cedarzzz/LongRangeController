// DlgWindowManager.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XsServer.h"
#include "DlgWindowManager.h"
#include "afxdialogex.h"


// CDlgWindowManager �Ի���

IMPLEMENT_DYNAMIC(CDlgWindowManager, CDialog)

CDlgWindowManager::CDlgWindowManager(CWnd* pParent,
	_CIOCPServer* IOCPServer, PCONTEXT_OBJECT ContextObject)
	: CDialog(IDD_DIALOG_WINDOW_MANAGER, pParent)
{
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;
	

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
}

CDlgWindowManager::~CDlgWindowManager()
{
}

void CDlgWindowManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIALOG_WINDOW_MANAGER_SHOW, m_CListCtrl_Dialog_Window_Manager_Show);
}


BEGIN_MESSAGE_MAP(CDlgWindowManager, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DIALOG_WINDOW_MANAGER_SHOW, &CDlgWindowManager::OnNMRClickListDialogWindowManagerShow)
	ON_COMMAND(ID_MENU_DIALOG_WINDOW_MANAGER_LIST_SHOW_REFRESH, &CDlgWindowManager::OnMenuDialogWindowManagerListShowRefresh)
END_MESSAGE_MAP()


// CDlgWindowManager ��Ϣ�������


void CDlgWindowManager::OnClose()
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


BOOL CDlgWindowManager::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_IconHwnd, FALSE);
	//��ÿͻ���IP��ַ
	CString v1;
	sockaddr_in ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength);
	v1.Format("\\\\%s - Զ�̴��ڹ���", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//���öԻ������
	SetWindowText(v1);

	//���öԻ����ϵ�����
	LOGFONT LogFont;
	CFont* v2 = m_CListCtrl_Dialog_Window_Manager_Show.GetFont();
	v2->GetLogFont(&LogFont);
	//����Height Width����
	LogFont.lfHeight = LogFont.lfHeight*1.3;   //�޸�����ĸ߱���
	LogFont.lfWidth = LogFont.lfWidth*1.3;     //�޸�����Ŀ����
	CFont v3;
	v3.CreateFontIndirect(&LogFont);
	m_CListCtrl_Dialog_Window_Manager_Show.SetFont(&v3);
	m_CListCtrl_Dialog_Window_Manager_Show.SetFont(&v3);
	v3.Detach();

	//���ض˴��ص�����
	char* BufferData = (char*)(m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(0));

	m_CListCtrl_Dialog_Window_Manager_Show.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);

	switch (BufferData[0])
	{
	case CLIENT_WINDOW_MANAGER_REPLY:
	{
		m_CListCtrl_Dialog_Window_Manager_Show.InsertColumn(0, "���ھ��", LVCFMT_LEFT, 80);
		m_CListCtrl_Dialog_Window_Manager_Show.InsertColumn(1, "��������", LVCFMT_LEFT, 300);
		m_CListCtrl_Dialog_Window_Manager_Show.InsertColumn(2, "����״̬", LVCFMT_LEFT, 300);
		ShowWindowList();
		break;
	}
	default:
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
void CDlgWindowManager::ShowWindowList(void)
{
	char* BufferData = (char*)(m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(1));
	DWORD Offset = 0;
	char* WindowTitleName = NULL;
	bool bDel = false;


	m_CListCtrl_Dialog_Window_Manager_Show.DeleteAllItems();
	CString v1;
	int i;
	for (i = 0; Offset < m_ContextObject->m_ReceivedDecompressedBufferData.GetArrayLength() - 1; i++)
	{
		LPDWORD Hwnd = LPDWORD(BufferData + Offset);  //���ھ��
		WindowTitleName = (char*)BufferData + Offset + sizeof(HWND);  //���ڱ���
		v1.Format("%5u", *Hwnd);
		m_CListCtrl_Dialog_Window_Manager_Show.InsertItem(i, v1);
		m_CListCtrl_Dialog_Window_Manager_Show.SetItemText(i, 1, WindowTitleName);
		m_CListCtrl_Dialog_Window_Manager_Show.SetItemText(i, 2, "��ʾ");  //����״̬����Ϊ��ʾ

		m_CListCtrl_Dialog_Window_Manager_Show.SetItemData(i, *Hwnd);
		Offset += sizeof(DWORD) + lstrlen(WindowTitleName) + 1;
	}
	v1.Format("���ڸ��� / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Dialog_Window_Manager_Show.SetColumn(2, &lvc);


}
void CDlgWindowManager::WindowHandleIO(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(0)[0])
	{
	case CLIENT_WINDOW_MANAGER_REPLY:
	{
		ShowWindowList();
		break;
	}
	}

}

void CDlgWindowManager::OnNMRClickListDialogWindowManagerShow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_DIALOG_WINDOW_MANAGER_LIST_SHOW_MAIN);
	CPoint Point;
	GetCursorPos(&Point);
	SetForegroundWindow();
	Menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		Point.x, Point.y, this, NULL);

	*pResult = 0;
}


void CDlgWindowManager::OnMenuDialogWindowManagerListShowRefresh()
{
	BYTE IsToken = CLIENT_WINDOW_MANAGER_REFRESH_REQUEST;
	m_IOCPServer->OnPrepareSending(m_ContextObject, &IsToken, 1);
}
