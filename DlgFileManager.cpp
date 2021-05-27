// DlgFileManager.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XsServer.h"
#include "DlgFileManager.h"
#include "afxdialogex.h"
#include "DlgNewFolder.h"
#include "DlgTransferMode.h"


// CDlgFileManager �Ի���

static UINT __Indicators[] = 
{ 
	ID_SEPARATOR,
	ID_SEPARATOR,
	IDR_STATUSBAR_FILE_MANAGER_PROCESS
};

IMPLEMENT_DYNAMIC(CDlgFileManager, CDialog)

CDlgFileManager::CDlgFileManager(CWnd* pParent,
	_CIOCPServer* IOCPServer, PCONTEXT_OBJECT ContextObject)
	: CDialog(IDD_DIALOG_FILE_MANAGER, pParent)
{
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;
	

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

	memset(m_ClientData, 0, sizeof(m_ClientData));
	memcpy(m_ClientData, ContextObject->m_ReceivedDecompressedBufferData.GetArray(1),
		ContextObject->m_ReceivedDecompressedBufferData.GetArrayLength() - 1);

	SHFILEINFO v1;
	//��ͼ��
	HIMAGELIST ImageListHwnd;  //SDK
	//����ϵͳͼ��
	ImageListHwnd = (HIMAGELIST)SHGetFileInfo(
		NULL,
		0,
		&v1,
		sizeof(SHFILEINFO),
		SHGFI_LARGEICON | SHGFI_SYSICONINDEX);

	m_CImageList_Large = CImageList::FromHandle(ImageListHwnd);

	ImageListHwnd = (HIMAGELIST)SHGetFileInfo(NULL, 0, &v1,
		sizeof(SHFILEINFO),SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	m_CImageList_Small = CImageList::FromHandle(ImageListHwnd);

	m_IsDragging = FALSE;
    m_IsStop = FALSE;   


}

CDlgFileManager::~CDlgFileManager()
{
}

void CDlgFileManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIALOG_FILE_MANAGER_SERVER_FILE, m_CListCtrl_Dialog_File_Manager_Server_File);
	DDX_Control(pDX, IDC_LIST_DIALOG_FILE_MANAGER_CLIENT_FILE, m_CListCtrl_Dialog_File_Manager_Client_File);
	DDX_Control(pDX, IDC_STATIC_DIALOG_FILE_MANAGER_SERVER_POSITION, m_CStatic_Dialog_File_Manager_Server_Position);
	DDX_Control(pDX, IDC_STATIC_DIALOG_FILE_MANAGER_CLIENT_POSITION, m_CStatic_Dialog_File_Manager_Client_Position);
	DDX_Control(pDX, IDC_COMBO_DIALOG_FILE_MANAGER_SERVER_FILE, m_CComboBox_Dialog_File_Manager_Server_File);
	DDX_Control(pDX, IDC_COMBO_DIALOG_FILE_MANAGER_CLIENT_FILE, m_CComboBox_Dialog_File_Manager_Client_File);
}


BEGIN_MESSAGE_MAP(CDlgFileManager, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DIALOG_FILE_MANAGER_SERVER_FILE, &CDlgFileManager::OnNMDblclkListDialogFileManagerServerFile)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DIALOG_FILE_MANAGER_CLIENT_FILE, &CDlgFileManager::OnNMDblclkListDialogFileManagerClientFile)
	ON_COMMAND(IDT_TOOLBAR_DIALOG_FILE_MANAGER_SERVER_FILE_PREVIOUS, &CDlgFileManager::OnFileManagerServerFilePrevious)
	ON_COMMAND(IDT_TOOLBAR_DIALOG_FILE_MANAGER_CLIENT_FILE_PREVIOUS, &CDlgFileManager::OnFileManagerClientFilePrevious)
	ON_COMMAND(IDT_TOOLBAR_DIALOG_FILE_MANAGER_SERVER_FILE_DELETE, &CDlgFileManager::OnFileManagerServerFileDelete)
	ON_COMMAND(IDT_TOOLBAR_DIALOG_FILE_MANAGER_SERVER_FILE_NEW_FOLDER, &CDlgFileManager::OnFileManagerServerFileNewFolder)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_DIALOG_FILE_MANAGER_SERVER_FILE, &CDlgFileManager::OnLvnBegindragListDialogFileManagerServerFile)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CDlgFileManager ��Ϣ�������


BOOL CDlgFileManager::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_IconHwnd, FALSE);
	//��ÿͻ���IP��ַ
	CString v1;
	sockaddr_in ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength);  //�õ�����IP
	v1.Format("\\\\%s - Զ���ļ�����", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//���öԻ������
	SetWindowText(v1);

	//��������
	if (!m_ToolBar_Dialog_File_Manager_Server_File.Create(this, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS | CBRS_FLYBY,
		IDR_TOOLBAR_DIALOG_FILE_MANAGER_SERVER_FILE) ||
		!m_ToolBar_Dialog_File_Manager_Server_File.LoadToolBar(IDR_TOOLBAR_DIALOG_FILE_MANAGER_SERVER_FILE))
	{
		return -1;
	}
	m_ToolBar_Dialog_File_Manager_Server_File.LoadTrueColorToolBar(
			24,
			IDB_BITMAP_FILE_MANAGER,
			IDB_BITMAP_FILE_MANAGER,
			IDB_BITMAP_FILE_MANAGER);

	m_ToolBar_Dialog_File_Manager_Server_File.AddDropDownButton(this, IDT_TOOLBAR_DIALOG_FILE_MANAGER_SERVER_FILE_VIEW,
		IDT_TOOLBAR_DIALOG_FILE_MANAGER_SERVER_FILE_VIEW);
	m_ToolBar_Dialog_File_Manager_Server_File.SetButtonText(0, "����");
	m_ToolBar_Dialog_File_Manager_Server_File.SetButtonText(1, "�鿴");
	m_ToolBar_Dialog_File_Manager_Server_File.SetButtonText(2, "ɾ��");
	m_ToolBar_Dialog_File_Manager_Server_File.SetButtonText(3, "�½�");
	m_ToolBar_Dialog_File_Manager_Server_File.SetButtonText(4, "����");
	m_ToolBar_Dialog_File_Manager_Server_File.SetButtonText(5, "ֹͣ");
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	m_CListCtrl_Dialog_File_Manager_Server_File.SetImageList(m_CImageList_Large, LVSIL_NORMAL);
	m_CListCtrl_Dialog_File_Manager_Server_File.SetImageList(m_CImageList_Small, LVSIL_SMALL);
	
	//��������  �ͻ���TOOLBAR
	if (!m_ToolBar_Dialog_File_Manager_Client_File.Create(this, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS | CBRS_FLYBY,
		IDR_TOOLBAR_DIALOG_FILE_MANAGER_CLIENT_FILE) ||
		!m_ToolBar_Dialog_File_Manager_Client_File.LoadToolBar(IDR_TOOLBAR_DIALOG_FILE_MANAGER_CLIENT_FILE))
	{
		return -1;
	}
	m_ToolBar_Dialog_File_Manager_Client_File.LoadTrueColorToolBar(
			24,
			IDB_BITMAP_FILE_MANAGER,
			IDB_BITMAP_FILE_MANAGER,
			IDB_BITMAP_FILE_MANAGER);
	
	m_ToolBar_Dialog_File_Manager_Client_File.AddDropDownButton(this, IDT_TOOLBAR_DIALOG_FILE_MANAGER_CLIENT_FILE_VIEW,
		IDT_TOOLBAR_DIALOG_FILE_MANAGER_CLIENT_FILE_VIEW);
	m_ToolBar_Dialog_File_Manager_Client_File.SetButtonText(0, "����");
	m_ToolBar_Dialog_File_Manager_Client_File.SetButtonText(1, "�鿴");
	m_ToolBar_Dialog_File_Manager_Client_File.SetButtonText(2, "ɾ��");
	m_ToolBar_Dialog_File_Manager_Client_File.SetButtonText(3, "�½�");
	m_ToolBar_Dialog_File_Manager_Client_File.SetButtonText(4, "����");
	m_ToolBar_Dialog_File_Manager_Client_File.SetButtonText(5, "ֹͣ");
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	
	m_CListCtrl_Dialog_File_Manager_Client_File.SetImageList(m_CImageList_Large, LVSIL_NORMAL);
	m_CListCtrl_Dialog_File_Manager_Client_File.SetImageList(m_CImageList_Small, LVSIL_SMALL);

	//����������ڴ�С
	RECT Rect;
	GetClientRect(&Rect);

	CRect v3;
	v3.top = Rect.bottom - 25;
	v3.left = 0;
	v3.right = Rect.right;
	v3.bottom = Rect.bottom;
	if (!m_StatusBar.Create(this) ||
		!m_StatusBar.SetIndicators(__Indicators,
			sizeof(__Indicators) / sizeof(UINT)))
	{
		return -1;
	}
	m_StatusBar.SetPaneInfo(0, m_StatusBar.GetItemID(0), SBPS_STRETCH, NULL);
	m_StatusBar.SetPaneInfo(1, m_StatusBar.GetItemID(1), SBPS_NORMAL, 120);
	m_StatusBar.SetPaneInfo(2, m_StatusBar.GetItemID(2), SBPS_NORMAL, 50);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);   //��ʾ״̬��

	m_StatusBar.MoveWindow(v3);
	m_StatusBar.GetItemRect(1, &Rect);

	Rect.bottom -= 1;

	m_ProgressCtrl = new CProgressCtrl;
	m_ProgressCtrl->Create(PBS_SMOOTH | WS_VISIBLE, Rect, &m_StatusBar, 1);
	m_ProgressCtrl->SetRange(0, 100);   //���ý�������Χ
	m_ProgressCtrl->SetPos(0);


	//----------------------------------------------Server���
	RECT ServerRect;
	m_CStatic_Dialog_File_Manager_Server_Position.GetWindowRect(&ServerRect);
	m_CStatic_Dialog_File_Manager_Server_Position.ShowWindow(SW_HIDE);
	//��ʾ������
	m_ToolBar_Dialog_File_Manager_Server_File.MoveWindow(&ServerRect);

	//----------------------------------------------Client���

	RECT  ClientRect;
	//��̬�ı���
	m_CStatic_Dialog_File_Manager_Client_Position.GetWindowRect(&ClientRect);
	m_CStatic_Dialog_File_Manager_Client_Position.ShowWindow(SW_HIDE);
	//��ʾ������
	m_ToolBar_Dialog_File_Manager_Client_File.MoveWindow(&ClientRect);

	FixedServerVolumeData();
	FixedClientVolumeData();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgFileManager::WindowHandleIO(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(0)[0])
	{
	case CLIENT_FILE_MANAGER_FILE_DATA_REPLY:
	{
		FixedClientFileData(m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(),
			m_ContextObject->m_ReceivedDecompressedBufferData.GetArrayLength() - 1);
		break;
	}
	case CLIENT_FILE_MANAGER_TRANSFER_MODE_REQUEST:
	{
		//�ڿͻ����з����������ļ�
		SendTransferMode();
		break;
	}
	case CLIENT_FILE_MANAGER_FILE_DATA_CONTINUE:
	{
		//��������
		SendServerFileDataToClient();
		break;
	}
	default:
		break;
	}

}
VOID CDlgFileManager::SendTransferMode()
{
	CDlgTransferMode Dialog(this);
	Dialog.m_FileName = m_DestFileFullPath;
	switch (Dialog.DoModal())
	{
	case IDC_BUTTON_DIALOG_TRANSFER_MODE_COVER:
	{
		m_TransferMode = TRANSFER_MODE_COVER;
		break;
	}
	case IDC_BUTTON_DIALOG_TRANSFER_MODE_COVER_ALL:
	{
		m_TransferMode = TRANSFER_MODE_COVER_ALL;
		break;
	}
	case IDC_BUTTON_DIALOG_TRANSFER_MODE_JUMP:
	{
		m_TransferMode = TRANSFER_MODE_JUMP;
		break;
	}
	case IDC_BUTTON_DIALOG_TRANSFER_MODE_JUMP_ALL:
	{
		m_TransferMode = TRANSFER_MODE_JUMP_ALL;
		break;
	}
	case IDCANCEL:
	{
		m_TransferMode = TRANSFER_MODE_CANCEL;
		break;
	}
	}
	if (m_TransferMode == TRANSFER_MODE_CANCEL)
	{
		EndCopyServerFileToClient();   //��������
		return;
	}
	BYTE IsToken[5];
	IsToken[0] = CLIENT_FILE_MANAGER_SET_TRANSFER_MODE;
	memcpy(IsToken + 1, &m_TransferMode, sizeof(m_TransferMode));
	m_IOCPServer->OnPrepareSending(m_ContextObject, (unsigned char*)&IsToken, sizeof(IsToken));
}
void CDlgFileManager::OnClose()
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
//��ô��̾���Ϣ
VOID CDlgFileManager::FixedServerVolumeData()
{
	char VolumeData[0x500] = { 0 };
	CHAR* Travel = NULL;
	m_CListCtrl_Dialog_File_Manager_Server_File.DeleteAllItems();

	while (m_CListCtrl_Dialog_File_Manager_Server_File.DeleteColumn(0) != 0);

	//��ʼ���б���Ϣ
	m_CListCtrl_Dialog_File_Manager_Server_File.InsertColumn(0, "����", LVCFMT_CENTER, 50);
	m_CListCtrl_Dialog_File_Manager_Server_File.InsertColumn(1, "����", LVCFMT_CENTER, 80);
	m_CListCtrl_Dialog_File_Manager_Server_File.InsertColumn(2, "�ļ�ϵͳ", LVCFMT_CENTER, 70);
	m_CListCtrl_Dialog_File_Manager_Server_File.InsertColumn(3, "�ܴ�С", LVCFMT_CENTER, 80);
	m_CListCtrl_Dialog_File_Manager_Server_File.InsertColumn(4, "���ÿռ�", LVCFMT_CENTER, 80);

	m_CListCtrl_Dialog_File_Manager_Server_File.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	GetLogicalDriveStrings(sizeof(VolumeData), (LPSTR)VolumeData);
	Travel = VolumeData;

	CHAR FileSystemType[MAX_PATH];
	unsigned __int64 HardDiskAmount = 0;
	unsigned __int64 HardDiskFreeSpace = 0;
	unsigned long    HardDiskAmountMB = 0;     //�ܴ�С
	unsigned long    HardDiskFreeSpaceMB = 0;  //ʣ��ռ�

	for (int i = 1; *Travel != '\0';i++, Travel += lstrlen(Travel) + 1)
	{
		memset(FileSystemType, 0, sizeof(FileSystemType));  //�ļ�ϵͳ  NTFS
		//�õ��ļ�ϵͳ��Ϣ����С
		GetVolumeInformation(Travel, NULL, 0, NULL, NULL, NULL, FileSystemType, MAX_PATH);

		ULONG FileSystemTypeLength = lstrlen(FileSystemType) + 1;
		if (GetDiskFreeSpaceEx(Travel, (PULARGE_INTEGER)&HardDiskFreeSpace,
			(PULARGE_INTEGER)&HardDiskAmount, NULL))
		{
			HardDiskAmountMB = HardDiskAmount / 1024 / 1024;   //������Ϊ�ֽ� Ҫת����G
			HardDiskFreeSpaceMB = HardDiskFreeSpace / 1024 / 1024;
		}
		else
		{
			HardDiskAmountMB = 0;
			HardDiskFreeSpaceMB = 0;
		}

		int Item = m_CListCtrl_Dialog_File_Manager_Server_File.InsertItem(i, Travel,
			GetIconIndex(Travel, GetFileAttributes(Travel)));   //���ϵͳͼ��

		m_CListCtrl_Dialog_File_Manager_Server_File.SetItemData(Item, 1);  //���ｫ����Ŀ¼���������ݲ��뵽����
		m_CListCtrl_Dialog_File_Manager_Server_File.SetItemText(Item, 2, FileSystemType);

		SHFILEINFO v1;
		SHGetFileInfo(Travel, FILE_ATTRIBUTE_NORMAL, &v1,
			sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);

		m_CListCtrl_Dialog_File_Manager_Server_File.SetItemText(Item, 1, v1.szTypeName);

		CString v2;
		v2.Format("%10.lf GB", (float)HardDiskAmountMB / 1024);
		m_CListCtrl_Dialog_File_Manager_Server_File.SetItemText(Item, 3, v2);
		v2.Format("%10.lf GB", (float)HardDiskFreeSpaceMB / 1024);
		m_CListCtrl_Dialog_File_Manager_Server_File.SetItemText(Item, 4, v2);
	}
}
VOID CDlgFileManager::FixedClientVolumeData()
{
	m_CListCtrl_Dialog_File_Manager_Client_File.DeleteAllItems();
	m_CListCtrl_Dialog_File_Manager_Client_File.InsertColumn(0, "����", LVCFMT_CENTER, 50);
	m_CListCtrl_Dialog_File_Manager_Client_File.InsertColumn(1, "����", LVCFMT_CENTER, 80);
	m_CListCtrl_Dialog_File_Manager_Client_File.InsertColumn(2, "�ļ�ϵͳ", LVCFMT_CENTER, 70);
	m_CListCtrl_Dialog_File_Manager_Client_File.InsertColumn(3, "�ܴ�С", LVCFMT_CENTER, 80);
	m_CListCtrl_Dialog_File_Manager_Client_File.InsertColumn(4, "���ÿռ�", LVCFMT_CENTER, 80);

	m_CListCtrl_Dialog_File_Manager_Client_File.SetExtendedStyle(LVS_EX_FULLROWSELECT);


	char* Travel = NULL;
	Travel = (char*)m_ClientData;   //ȥ������Ϣͷ��һ���ֽ���

	int i = 0;
	ULONG v1 = 0;
	for (i = 0; Travel[i] != '\0';)
	{
		//�����������ж�ͼ�������
		if (Travel[i] == 'A' || Travel[i] == 'B')
		{
			v1 = 6;
		}
		else
		{
			switch (Travel[i + 1])   //�������ж������������� �鿴���ض�
			{
			case DRIVE_REMOVABLE:
				v1 = 2 + 5;
				break;
			case DRIVE_FIXED:
				v1 = 3 + 5;
				break;
			case DRIVE_REMOTE:
				v1 = 4 + 5;
				break;
			case DRIVE_CDROM:
				v1 = 9;
				break;
			default:
				v1 = 0;
				break;

			}
		}
		CString v2;
		//��ʽ���̷�
		v2.Format("%c:\\", Travel[i]);
		int Item = m_CListCtrl_Dialog_File_Manager_Client_File.InsertItem(i, v2, v1);
		m_CListCtrl_Dialog_File_Manager_Client_File.SetItemData(Item, 1);  //�����ݲ���ʾ �����ڵ�ǰ����    1����Ŀ¼ 0�����ļ�

		unsigned long    HardDiskAmountMB = 0;     //�ܴ�С
	    unsigned long    HardDiskFreeSpaceMB = 0;  //ʣ��ռ�
		memcpy(&HardDiskAmountMB, Travel + i + 2, 4);
		memcpy(&HardDiskFreeSpaceMB, Travel + i + 6, 4);

		v2.Format("%10.lf GB", (float)HardDiskAmountMB / 1024);
		m_CListCtrl_Dialog_File_Manager_Client_File.SetItemText(Item, 3, v2);
		v2.Format("%10.lf GB", (float)HardDiskFreeSpaceMB / 1024);
		m_CListCtrl_Dialog_File_Manager_Client_File.SetItemText(Item, 4, v2);

		i += 10;

		CString v7;
		v7 = Travel + i;
		m_CListCtrl_Dialog_File_Manager_Client_File.SetItemText(Item, 1, v7);

		i += strlen(Travel + i) + 1;

		CString FileSystemType;
		FileSystemType = Travel + i;
		m_CListCtrl_Dialog_File_Manager_Client_File.SetItemText(Item, 2, FileSystemType);
		i += strlen(Travel + i) + 1;

	}
}
//��÷�����ļ�����
VOID CDlgFileManager::FixedServerFileData(CString Directory)
{
	if (Directory.GetLength() == 0)
	{
		//ȡ��ControlList�е���
		int Item = m_CListCtrl_Dialog_File_Manager_Server_File.GetSelectionMark();

		//�����ѡ��
		if (Item != -1)
		{
			//��ø����е���������
			if (m_CListCtrl_Dialog_File_Manager_Server_File.GetItemData(Item) == 1)
			{
				//��Ŀ¼
				Directory = m_CListCtrl_Dialog_File_Manager_Server_File.GetItemText(Item, 0);
			}
		}
		//����Ͽ���õ�·��
		else
		{
			//
		}
	}
	if (Directory == "..")
	{
		//������һ��Ŀ¼
		m_ServerFileFullPath = GetParentDirectory(m_ServerFileFullPath);   //�ʼ�

	}
	//ˢ�µ�ǰ��
	else if (Directory != ".")   //��ϵͳ�е�ÿ��Ŀ¼�ж������һ��.��..Ŀ¼
	{
		m_ServerFileFullPath += Directory;
		if (m_ServerFileFullPath.Right(1) != "\\")
		{
			//������ַ������ұ߲�����'\\'
			m_ServerFileFullPath += "\\";
		}

	}
	if (m_ServerFileFullPath.GetLength() == 0)
	{
		FixedServerVolumeData();
		return;
	}

	//�����յ��ļ�·�����뵽�ؼ���
	m_CComboBox_Dialog_File_Manager_Server_File.InsertString(0, m_ServerFileFullPath);
	m_CComboBox_Dialog_File_Manager_Server_File.SetCurSel(0);

	//ɾ��ControlList�ϵ���
	m_CListCtrl_Dialog_File_Manager_Server_File.DeleteAllItems();
	while (m_CListCtrl_Dialog_File_Manager_Server_File.DeleteColumn(0) != 0);

	//��ʼ���б���Ϣ
	m_CListCtrl_Dialog_File_Manager_Server_File.InsertColumn(0, "����", LVCFMT_LEFT, 150);
	m_CListCtrl_Dialog_File_Manager_Server_File.InsertColumn(1, "��С", LVCFMT_LEFT, 100);
	m_CListCtrl_Dialog_File_Manager_Server_File.InsertColumn(2, "����", LVCFMT_LEFT, 100);
	m_CListCtrl_Dialog_File_Manager_Server_File.InsertColumn(3, "�޸�����", LVCFMT_LEFT, 115);


	int v10 = 0;

	//�Լ���ControlList�ؼ���д��һ��..Ŀ¼��һ��˫���ͷ�����һ�㣩
	m_CListCtrl_Dialog_File_Manager_Server_File.SetItemData(m_CListCtrl_Dialog_File_Manager_Server_File.InsertItem(v10++, "..",
		GetIconIndex(NULL, FILE_ATTRIBUTE_DIRECTORY)), 1);

	//ѭ�����δ�����������(Ŀ¼���ļ�)
	for (int i = 0; i < 2; i++)
	{
		CFileFind FileFind;
		BOOL IsLoop;
		IsLoop = FileFind.FindFile(m_ServerFileFullPath + "*.*");
		while (IsLoop)
		{
			IsLoop = FileFind.FindNextFile();
			if (FileFind.IsDots())
			{
				continue;
			}
			BOOL IsInsert = !FileFind.IsDirectory() == i;
			if (!IsInsert)
			{
				continue;
			}
			int Item = m_CListCtrl_Dialog_File_Manager_Server_File.InsertItem(v10++, FileFind.GetFileName(),
				GetIconIndex(FileFind.GetFileName(), GetFileAttributes(FileFind.GetFilePath())));
			//�����Ŀ¼������������Ϊ1
			m_CListCtrl_Dialog_File_Manager_Server_File.SetItemData(Item, FileFind.IsDirectory());
			SHFILEINFO v1;
			SHGetFileInfo(FileFind.GetFileName(), FILE_ATTRIBUTE_NORMAL, &v1, sizeof(SHFILEINFO),
				SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);

			if (FileFind.IsDirectory())
			{
				m_CListCtrl_Dialog_File_Manager_Server_File.SetItemText(Item, 2, "�ļ���");
			}
			else
			{
				m_CListCtrl_Dialog_File_Manager_Server_File.SetItemText(Item, 2, v1.szTypeName);
			}
			CString v2;
			v2.Format("%10d KB", FileFind.GetLength() / 1024 + (FileFind.GetLength() % 1024 ? 1 : 0));
			m_CListCtrl_Dialog_File_Manager_Server_File.SetItemText(Item, 1, v2);
			CTime Time;
			FileFind.GetLastWriteTime(Time);
			m_CListCtrl_Dialog_File_Manager_Server_File.SetItemText(Item, 3, Time.Format("%Y-%m-%d %H:%M"));

		}

	}
}
//��ÿͻ����ļ�����
VOID CDlgFileManager::FixedClientFileData(BYTE* BufferData, ULONG BufferLength)
{
	//SHFILEINFO v1;
	//�ؽ�����
	m_CListCtrl_Dialog_File_Manager_Client_File.DeleteAllItems();
	while (m_CListCtrl_Dialog_File_Manager_Client_File.DeleteColumn(0) != 0);

	//��ʼ���б���Ϣ
	m_CListCtrl_Dialog_File_Manager_Client_File.InsertColumn(0, "����", LVCFMT_LEFT, 150);
	m_CListCtrl_Dialog_File_Manager_Client_File.InsertColumn(1, "��С", LVCFMT_LEFT, 100);
	m_CListCtrl_Dialog_File_Manager_Client_File.InsertColumn(2, "����", LVCFMT_LEFT, 100);
	m_CListCtrl_Dialog_File_Manager_Client_File.InsertColumn(3, "�޸�����", LVCFMT_LEFT, 115);
	int v10 = 0;

	//�Լ���ControlList�ؼ���д��һ��..Ŀ¼��һ��˫���ͷ�����һ�㣩
	m_CListCtrl_Dialog_File_Manager_Client_File.SetItemData(m_CListCtrl_Dialog_File_Manager_Client_File.InsertItem(v10++, "..",
		GetIconIndex(NULL, FILE_ATTRIBUTE_DIRECTORY)), 1);
	if (BufferLength != 0)
	{
		for (int i = 0; i < 2; i++)
		{
			//����Token
			char* Travel = (char*)(BufferData + 1);
			for (char* v1 = Travel; Travel - v1 < BufferLength - 1;)
			{
				char* FileName = NULL;
				DWORD FileSizeHigh = 0;       //�ļ����ֽڴ�С
				DWORD FileSizeLow = 0;        //�ļ����ֽڴ�С
				int Item = 0;
				bool IsInsert = false;
				FILETIME FileTime;

				int v3 = *Travel ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
				//iΪ0ʱ��Ŀ¼iΪ1ʱ���ļ�
				IsInsert = !(v3 == FILE_ATTRIBUTE_DIRECTORY) == i;

				FileName = ++Travel;
				if (IsInsert)
				{
					Item = m_CListCtrl_Dialog_File_Manager_Client_File.InsertItem(v10++, FileName, GetIconIndex(FileName, v3));
					m_CListCtrl_Dialog_File_Manager_Client_File.SetItemData(Item, v3 == FILE_ATTRIBUTE_DIRECTORY);  //��������
					SHFILEINFO v2;
					SHGetFileInfo(FileName, FILE_ATTRIBUTE_NORMAL | v3, &v2, sizeof(SHFILEINFO),
						SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
					m_CListCtrl_Dialog_File_Manager_Client_File.SetItemText(Item, 2, v2.szTypeName);

				}
				//�õ��ļ���С
				Travel += strlen(FileName) + 1;
				if (IsInsert)
				{
					memcpy(&FileSizeHigh, Travel, 4);
					memcpy(&FileSizeLow, Travel+4, 4);
					CString v7;
					v7.Format("%10d KB", (FileSizeHigh*(MAXDWORD + 1)) / 1024 + FileSizeLow / 1024 + (FileSizeLow % 1024  ? 1 : 0));
					m_CListCtrl_Dialog_File_Manager_Client_File.SetItemText(Item, 1, v7);
					memcpy(&FileTime, Travel + 8, sizeof(FILETIME));
					CTime Time(FileTime);
					m_CListCtrl_Dialog_File_Manager_Client_File.SetItemText(Item, 3, Time.Format("%Y-%m-%D %H:%M"));
				}
				Travel += 16;
			}
		}
	}

	//�ָ�����
	m_CListCtrl_Dialog_File_Manager_Client_File.EnableWindow(TRUE);

}
//��ù����Ӱ
int CDlgFileManager::GetIconIndex(LPCTSTR VolumnName, DWORD FileAttributes)
{
	SHFILEINFO v1;
	if (FileAttributes == INVALID_FILE_ATTRIBUTES)
		FileAttributes = FILE_ATTRIBUTE_NORMAL;
	else
		FileAttributes |= FILE_ATTRIBUTE_NORMAL;
	SHGetFileInfo(
		VolumnName,
		FileAttributes,
		&v1,
		sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
	return v1.iIcon;
}
//������һ��Ŀ¼
CString CDlgFileManager::GetParentDirectory(CString FileFullPath)
{
	CString v1 = FileFullPath;
	int Index = v1.ReverseFind('\\');
	if (Index == -1)
	{
		return v1;
	}
	CString v2 = v1.Left(Index);
	Index = v2.ReverseFind('\\');
	if (Index == -1)
	{
		v1 = "";
		return v1;
	}
	v1 = v2.Left(Index);
	if (v1.Right(1) != "\\")
	{
		v1 += "\\";
	}
	return v1;
}
//˫����ControlList�е���
void CDlgFileManager::OnNMDblclkListDialogFileManagerServerFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (m_CListCtrl_Dialog_File_Manager_Server_File.GetSelectedCount() == 0 ||
		m_CListCtrl_Dialog_File_Manager_Server_File.GetItemData(m_CListCtrl_Dialog_File_Manager_Server_File.GetSelectionMark()) != 1)
	{
		//����Ĳ���Ŀ¼
		return;
	}
	FixedServerFileData();
	*pResult = 0;
}
void CDlgFileManager::OnNMDblclkListDialogFileManagerClientFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (m_CListCtrl_Dialog_File_Manager_Client_File.GetSelectedCount() == 0 ||
		m_CListCtrl_Dialog_File_Manager_Client_File.GetItemData(m_CListCtrl_Dialog_File_Manager_Client_File.GetSelectionMark()) != 1)
	{
		return;
	}
	GetClientFileData();  //��ͻ��˷���Ϣ

	*pResult = 0;
}
VOID CDlgFileManager::GetClientFileData(CString Directory)
{
	if (Directory.GetLength() == 0)
	{
		//ȡ��ControlList�е���
		int Item = m_CListCtrl_Dialog_File_Manager_Client_File.GetSelectionMark();

		//�����ѡ��
		if (Item != -1)
		{
			//��ø����е���������
			if (m_CListCtrl_Dialog_File_Manager_Client_File.GetItemData(Item) == 1)
			{
				//��Ŀ¼
				Directory = m_CListCtrl_Dialog_File_Manager_Client_File.GetItemText(Item, 0);
			}
		}
		//����Ͽ���õ�·��
		else
		{
			//
		}
	}
	if (Directory == "..")
	{
		//������һ��Ŀ¼
		m_ClientFileFullPath = GetParentDirectory(m_ClientFileFullPath);   //�ʼ�

	}
	//ˢ�µ�ǰ��
	else if (Directory != ".")   //��ϵͳ�е�ÿ��Ŀ¼�ж������һ��.��..Ŀ¼
	{
		m_ClientFileFullPath += Directory;
		if (m_ClientFileFullPath.Right(1) != "\\")
		{
			//������ַ������ұ߲�����'\\'
			m_ClientFileFullPath += "\\";
		}

	}
	if (m_ClientFileFullPath.GetLength() == 0)
	{
		//�����Ŀ¼ ʵ�ʾ���Ҫˢ�¾�
		FixedClientVolumeData();
		return;
	}

	ULONG BufferLength = m_ClientFileFullPath.GetLength() + 2;
	BYTE* BufferData = (BYTE*)new BYTE[BufferLength];

	BufferData[0] = CLIENT_FILE_MANAGER_FILE_DATA_REQUEST;
	memcpy(BufferData + 1, m_ClientFileFullPath.GetBuffer(0), BufferLength - 1);
	m_IOCPServer->OnPrepareSending(m_ContextObject, BufferData, BufferLength);
	delete[] BufferData;
	BufferData = NULL;

	//�õ�����ǰ���ô���
	m_CListCtrl_Dialog_File_Manager_Client_File.EnableWindow(FALSE);  //���ܵ��
	m_ProgressCtrl->SetPos(0);  //��ʼ��������

}
void CDlgFileManager::OnFileManagerServerFilePrevious()
{
	FixedServerFileData("..");
}
void CDlgFileManager::OnFileManagerServerFileDelete()
{
	CString v1;
	if (m_CListCtrl_Dialog_File_Manager_Server_File.GetSelectedCount() > 1)
	{
		v1.Format("ȷ��Ҫ����%d��ɾ����?", m_CListCtrl_Dialog_File_Manager_Server_File.GetSelectedCount());
	}
	else
	{
		int Item = m_CListCtrl_Dialog_File_Manager_Server_File.GetSelectionMark();
		if (Item == -1)
		{
			return;
		}
		CString FileName = m_CListCtrl_Dialog_File_Manager_Server_File.GetItemText(m_CListCtrl_Dialog_File_Manager_Server_File.GetSelectionMark(), 0);

		//ɾ�����ǲ���Ŀ¼
		if (m_CListCtrl_Dialog_File_Manager_Server_File.GetItemData(Item) == 1)
		{
			v1.Format("ȷʵҪɾ���ļ��С�%s����������������ɾ����?", FileName);
		}
		else
		{
			v1.Format("ȷʵҪ�ѡ�%s��ɾ����?", FileName);
		}

	}
	if (::MessageBox(m_hWnd, v1, "ȷ��ɾ��", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		return;
	}
	//�������û��ҵ�
	EnableControl(FALSE);

	POSITION Position = m_CListCtrl_Dialog_File_Manager_Server_File.GetFirstSelectedItemPosition();
	while (Position)
	{
		int Item = m_CListCtrl_Dialog_File_Manager_Server_File.GetNextSelectedItem(Position);
		CString FileFullPath = m_ServerFileFullPath + m_CListCtrl_Dialog_File_Manager_Server_File.GetItemText(Item, 0);
		//�����Ŀ¼
		if (m_CListCtrl_Dialog_File_Manager_Server_File.GetItemData(Item))
		{
			FileFullPath += '\\';
			DeleteDirectory(FileFullPath);
		}
		else
		{
			DeleteFile(FileFullPath);
		}
	}
	//�����ļ�������
	EnableControl(TRUE);
	//ˢ������
	FixedServerFileData(".");
}
void CDlgFileManager::OnFileManagerServerFileNewFolder()
{
	if (m_ServerFileFullPath == "")
	{
		return;
	}

	CDlgNewFolder Dialog(this);
	if (Dialog.DoModal() == IDOK&&Dialog.m_CEdit_Dialog_New_Folder_Main.GetLength())
	{
		//�������Ŀ¼
		CString v1;
		v1 = m_ServerFileFullPath + Dialog.m_CEdit_Dialog_New_Folder_Main + "\\";
		MakeSureDirectoryPathExists(v1.GetBuffer());
		FixedServerFileData(".");
	}
}
void CDlgFileManager::OnFileManagerServerFileStop()
{

}
void CDlgFileManager::OnFileManagerServerFileViewSmall()
{

}
void CDlgFileManager::OnFileManagerServerFileViewList()
{

}
void CDlgFileManager::OnFileManagerServerFileViewDetail()
{

}
void CDlgFileManager::OnFileManagerClientFilePrevious()
{
	GetClientFileData("..");
}
void CDlgFileManager::OnFileManagerClientFileDelete()
{

}
void CDlgFileManager::OnFileManagerClientFileNewFolder()
{

}
void CDlgFileManager::OnFileManagerClientFileStop()
{

}
void CDlgFileManager::OnFileManagerClientFileViewSmall()
{

}
void CDlgFileManager::OnFileManagerClientFileViewList()
{

}
void CDlgFileManager::OnFileManagerClientFileViewDetail()
{

}
void CDlgFileManager::EnableControl(BOOL IsEnable)
{
	m_CListCtrl_Dialog_File_Manager_Client_File.EnableWindow(IsEnable);
	m_CListCtrl_Dialog_File_Manager_Server_File.EnableWindow(IsEnable);
	m_CComboBox_Dialog_File_Manager_Client_File.EnableWindow(IsEnable);
	m_CComboBox_Dialog_File_Manager_Server_File.EnableWindow(IsEnable);
}
BOOL CDlgFileManager::DeleteDirectory(LPCTSTR DirectoryFullPath)
{
	WIN32_FIND_DATA v1;
	char BufferData[MAX_PATH] = { 0 };

	wsprintf(BufferData, "%s\\*.*", DirectoryFullPath);

	HANDLE FileHandle = FindFirstFile(BufferData, &v1);
	if (FileHandle == INVALID_HANDLE_VALUE)  //���û���ҵ����߲���ʧ��
	{
		return FALSE;
	}

	do 
	{
		if (v1.cFileName[0] == '.'&&strlen(v1.cFileName)<=2)
		{
			continue;
		}
		else
		{
			if (v1.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				char v2[MAX_PATH];
				wsprintf(v2, "%s\\%s", DirectoryFullPath, v1.cFileName);
				DeleteDirectory(v2);
			}
			else
			{
				char v2[MAX_PATH];
				wsprintf(v2, "%s\\%s", DirectoryFullPath, v1.cFileName);
				DeleteFile(v2);
			}
		}
	} while (FindNextFile(FileHandle, &v1));

	FindClose(FileHandle);   //�رղ��Ҿ��

	if (!RemoveDirectory(DirectoryFullPath))
	{
		return FALSE;
	}
	return TRUE;

}
//���Ŀ¼
BOOL CDlgFileManager::MakeSureDirectoryPathExists(char* DirectoryFullPath)
{
	char* Travel = NULL;
	char* BufferData = NULL;
	DWORD DirectoryAttributes = 0;
	__try
	{
		BufferData = (char*)malloc(sizeof(char)*(strlen(DirectoryFullPath) + 1));
		if (BufferData == NULL)
		{
			return FALSE;
		}
		strcpy(BufferData, DirectoryFullPath);
		Travel = BufferData;

		if (*(Travel + 1) == ':')
		{
			Travel++;
			Travel++;
			if (*Travel && (*Travel == '\\'))
			{
				Travel++;
			}
		}
		//���Ŀ¼
		while (*Travel)
		{
			if (*Travel == '\\')
			{
				*Travel = '\0';
				//�鿴�Ƿ�ΪĿ¼ Ŀ¼�Ƿ����
				DirectoryAttributes = GetFileAttributes(BufferData);
				if (DirectoryAttributes == 0xffffffff)
				{
					if (!CreateDirectory(BufferData, NULL))
					{
						if (GetLastError() != ERROR_ALREADY_EXISTS)
						{
							free(BufferData);
							return FALSE;
						}

					}

				}
				else
				{
					if ((DirectoryAttributes&FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
					{
						free(BufferData);
						BufferData = NULL;
						return FALSE;
					}
				}
				*Travel = '\\';
			}
			Travel = CharNext(Travel);
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		if (BufferData != NULL)
		{
			free(BufferData);
			BufferData = NULL;
		}
		return FALSE;
	}
	if (BufferData != NULL)
	{
		free(BufferData);
		BufferData = NULL;
	}
	return TRUE;
}
//�ӷ���˵��ͻ��˵��ļ����� 
void CDlgFileManager::OnLvnBegindragListDialogFileManagerServerFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (m_ServerFileFullPath.IsEmpty() || m_ClientFileFullPath.IsEmpty())
	{
		return;
	}
	if (m_CListCtrl_Dialog_File_Manager_Server_File.GetSelectedCount() > 1)
	{
		//���ѡ�����
		m_CursorHwnd = AfxGetApp()->LoadCursor(IDC_CURSOR_MULTI_DRAG);
	}
	else
	{
		//���ѡ����
		m_CursorHwnd = AfxGetApp()->LoadCursor(IDC_CURSOR_SINGLE_DRAG);
	}
	m_IsDragging = TRUE;
	m_DragListControl = &m_CListCtrl_Dialog_File_Manager_Server_File;
	m_DropListControl = &m_CListCtrl_Dialog_File_Manager_Client_File;

	SetCapture();

	*pResult = 0;
}
void CDlgFileManager::OnMouseMove(UINT nFlags, CPoint point)
{
	//�ж��Ƿ���ק�ļ�
	if (m_IsDragging)
	{
		CPoint Point(point);  //������λ��
		ClientToScreen(&Point);  //ת����������Ϊ�������Ļ��
		//��������ô��ھ��
		CWnd* v1 = WindowFromPoint(Point);

		//�������ǵĴ��ڷ�Χ��
		if (v1->IsKindOf(RUNTIME_CLASS(CListCtrl)))
		{
			//�ı������ʽ
			SetCursor(m_CursorHwnd);
			return;
		}
		else
		{
			SetCursor(LoadCursor(NULL, IDC_NO));   //�������ڻ�Ϊ�����ʽ
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}
void CDlgFileManager::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_IsDragging)
	{
		ReleaseCapture();  //�ͷ����Ĳ���
		m_IsDragging = FALSE;

		CPoint Point(point);   //��õ�ǰ���λ�� �����������Ļ��
		ClientToScreen(&Point);  //ת��������ڵ�ǰ�û��Ĵ��ڵ�λ��

		CWnd* v1 = WindowFromPoint(Point);   //�жϵ�ǰ����·����޿ؼ�

		if (v1->IsKindOf(RUNTIME_CLASS(CListCtrl)))  //�����һ��ListControl
		{
			m_DropListControl = (CListCtrl*)v1;  //���浱ǰ���ھ��
			DropDataOnList();   //������
		}
	}

	CDialog::OnLButtonUp(nFlags, point);
}
//�ж����ݴ������� �ֱ���ú���
VOID CDlgFileManager::DropDataOnList()
{
	//�ж��Ǵ�����������
	if (m_DragListControl == m_DropListControl)
	{
		return;
	}
	if ((CWnd*)m_DropListControl == &m_CListCtrl_Dialog_File_Manager_Server_File)
	{
		//�ͻ��������ض�
	}
	else if ((CWnd*)m_DropListControl == &m_CListCtrl_Dialog_File_Manager_Client_File)
	{
		//���ض���ͻ���
		CopyServerDataToClient();
	}
	else
	{
		return;
	}
}
//�����ض˵��ͻ���
VOID CDlgFileManager::CopyServerDataToClient()
{
	//����һ��ģ��
	m_ServerDataToClientJob.RemoveAll();
	POSITION Position = m_CListCtrl_Dialog_File_Manager_Server_File.GetFirstSelectedItemPosition();
	while (Position)
	{
		int Item = m_CListCtrl_Dialog_File_Manager_Server_File.GetNextSelectedItem(Position);
		CString FileFullPath = NULL;
		
		FileFullPath = m_ServerFileFullPath + m_CListCtrl_Dialog_File_Manager_Server_File.GetItemText(Item, 0);
		
		//�����Ŀ¼
		if (m_CListCtrl_Dialog_File_Manager_Server_File.GetItemData(Item))
		{
			FileFullPath += '\\';
			FixedServerDataToClient(FileFullPath.GetBuffer(0));
		}
		else
		{
			//���ļ��ж��Ƿ�Ϸ�
			HANDLE FileHandle = CreateFile(FileFullPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if (FileHandle == INVALID_HANDLE_VALUE)
			{
				continue;
			}
			m_ServerDataToClientJob.AddTail(FileFullPath);

			CloseHandle(FileHandle);
		}

	}
	if (m_ServerDataToClientJob.IsEmpty())
	{
		//���ѡ���п�Ŀ¼ ������
		::MessageBox(m_hWnd, "�ļ���Ϊ��", "����", MB_OK | MB_ICONWARNING);
		return;
	}
	EnableControl(TRUE);
	SendServerDataToClient();   //���͵�һ������

}
//����Ŀ¼�е�����  �ʼ�
BOOL CDlgFileManager::FixedServerDataToClient(LPCTSTR DirectoryFullPath)
{
	CHAR BufferData[MAX_PATH];
	CHAR *Slash = NULL;
	memset(BufferData, 0, sizeof(BufferData));

	if (DirectoryFullPath[strlen(DirectoryFullPath) - 1] != '\\')
	{
		Slash = "\\";
	}
	else
	{
		Slash = "";
	}
	sprintf(BufferData, "%s%s*.*", DirectoryFullPath, Slash);

	WIN32_FIND_DATA WinFindData;
	HANDLE FileHandle = FindFirstFile(BufferData, &WinFindData);
	if (FileHandle == INVALID_HANDLE_VALUE)  //��û�в��ҵ������ʧ��
	{
		return FALSE;
	}
	do
	{
		//����������Ŀ¼ '.'��'..'
		if (WinFindData.cFileName[0] == '.')
		{
			continue;
		}
		if (WinFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CHAR v1[MAX_PATH];
			sprintf(v1, "%s%s%s", DirectoryFullPath, Slash, WinFindData.cFileName);
			FixedServerDataToClient(v1);  //�����Ŀ¼��������Ŀ¼���еݹ�
		}
		else
		{
			CString FileFullPath;
			FileFullPath.Format("%s%s%s", DirectoryFullPath, Slash, WinFindData.cFileName);
			m_ServerDataToClientJob.AddTail(FileFullPath);
			//���ļ����в���
		}
	} while (FindNextFile(FileHandle, &WinFindData));
	FindClose(FileHandle);  //�رղ��Ҿ��
	return true;
}
//�������ݵ����ض�
BOOL CDlgFileManager::SendServerDataToClient()
{
	if (m_ServerDataToClientJob.IsEmpty())
	{
		return FALSE;
	}

	CString DestinationDirectory = m_ClientFileFullPath;  //ȷ��Ŀ��·��
	m_SourFileFullPath = m_ServerDataToClientJob.GetHead();  //��õ�һ�����������

	DWORD FileSizeHigh = 0;
	DWORD FileSizeLow = 0;
	HANDLE FileHandle;
	CString v1 = m_SourFileFullPath;  //Զ���ļ�

	//�õ�Ҫ�����Զ���ļ�·��
	v1.Replace(m_ServerFileFullPath, m_ClientFileFullPath);
	m_DestFileFullPath = v1;  //�����õ�����

	//�򿪱����ļ�
	FileHandle = CreateFile(m_SourFileFullPath.GetBuffer(0),
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	//��ȡ�ļ���С
	FileSizeLow = GetFileSize(FileHandle, &FileSizeHigh);
	//�������ݰ� �����ļ�����
	m_TransferFileLength = (FileSizeHigh * (MAXDWORD + 1)) + FileSizeLow;
	CloseHandle(FileHandle);

	ULONG BufferLength = v1.GetLength() + 10;
	BYTE* BufferData = (BYTE*)LocalAlloc(LPTR, BufferLength);
	memset(BufferData, 0, BufferLength);

	BufferData[0] = CLIENT_FILE_MANAGER_SEND_FILE_INFORMATION;

	memcpy(BufferData + 1, &FileSizeHigh, sizeof(DWORD));     
	memcpy(BufferData + 5, &FileSizeLow, sizeof(DWORD));

	memcpy(BufferData + 9, v1.GetBuffer(0), v1.GetLength() + 1);

	m_IOCPServer->OnPrepareSending(m_ContextObject, BufferData, BufferLength);

	LocalFree(BufferData);
	//�������б���ɾ���Լ�
	m_ServerDataToClientJob.RemoveHead();
	return TRUE;
}
VOID CDlgFileManager::EndCopyServerFileToClient()
{
	m_Counter = 0;
	m_TransferFileLength = 0;

	//������
	ShowProgress();
	if (m_ServerDataToClientJob.IsEmpty() || m_IsStop)
	{
		m_ServerDataToClientJob.RemoveAll();
		m_IsStop = FALSE;
		EnableControl(TRUE);     //�û����Ե���ؼ�
		m_TransferMode = TRANSFER_MODE_NORMAL;
		GetClientFileData(".");  //ˢ���б�
	}
	else
	{
		Sleep(5);
		SendServerDataToClient();
	}
	return;
}
void CDlgFileManager::ShowProgress()
{
	if ((int)m_Counter == -1)
	{
		m_Counter = m_TransferFileLength;
	}

	int Progress = (float)(m_Counter * 100) / m_TransferFileLength;
	m_ProgressCtrl->SetPos(Progress);

	if (m_Counter == m_TransferFileLength)
	{
		m_Counter = m_TransferFileLength = 0;
	}
}
VOID CDlgFileManager::SendServerFileDataToClient()
{
	FILE_SIZE* v1 = (FILE_SIZE*)(m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(1));
	LONG FileSizeHigh = v1->FileSizeHigh;
	LONG FileSizeLow  = v1->FileSizeLow;

	m_Counter = MAKEINT64(v1->FileSizeLow, v1->FileSizeHigh);

	ShowProgress();  //��ʾ������

	if (m_Counter == m_TransferFileLength || m_IsStop || v1->FileSizeLow == -1)
	{
		//v1->FileSizeLow == -1 �ǶԷ�ѡ��������   m_Counter == m_TransferFileLength ����˵�ǰ����
		EndCopyServerFileToClient();  //������һ������ �����ڵĻ�
		return;
	}
	HANDLE FileHandle;
	FileHandle = CreateFile(m_SourFileFullPath.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	SetFilePointer(FileHandle, FileSizeLow, &FileSizeHigh, FILE_BEGIN);  //�����ļ��Ĵ�С

	int v3 = 9;  //���ݰ�ͷ����С

	DWORD NumberofBytesToRead = MAX_SEND_BUFFER - v3;
	DWORD NumberofBytesRead = 0;
	BYTE* BufferData = (BYTE*)LocalAlloc(LPTR, MAX_SEND_BUFFER);

	if (BufferData == NULL)
	{
		CloseHandle(FileHandle);
		return;
	}
	BufferData[0] = CLIENT_FILE_MANAGER_FILE_DATA;
	memcpy(BufferData + 1, &FileSizeHigh, sizeof(FileSizeHigh));
	memcpy(BufferData + 5, &FileSizeLow, sizeof(FileSizeLow));
	//���ļ��ж�ȡ����
	ReadFile(FileHandle, BufferData + v3, NumberofBytesToRead, &NumberofBytesRead, NULL);
	CloseHandle(FileHandle);

	if (NumberofBytesRead > 0)
	{
		ULONG BufferLength = NumberofBytesRead + v3;
		m_IOCPServer->OnPrepareSending(m_ContextObject, BufferData, BufferLength);
	}
	LocalFree(BufferData);
	
}