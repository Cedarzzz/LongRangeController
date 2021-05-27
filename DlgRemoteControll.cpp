// DlgRemoteControll.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XsServer.h"
#include "DlgRemoteControll.h"
#include "afxdialogex.h"

enum
{
	ID_MENU_DIALOG_REMOTE_CONTROLL_CONTROL = 0x1010,
    ID_MENU_DIALOG_REMOTE_CONTROLL_SEND_CTRL_ALT_DEL,
	ID_MENU_DIALOG_REMOTE_CONTROLL_TRACE_CURSOR,   //������ʾԶ�����
	ID_MENU_DIALOG_REMOTE_CONTROLL_BLOCK_INPUT,    //����Զ�̼��������
	ID_MENU_DIALOG_REMOTE_CONTROLL_SAVE_DIB,       //����ͼƬ
	ID_MENU_DIALOG_REMOTE_CONTROLL_GET_CLIPBOARD,  //��ȡ������
	ID_MENU_DIALOG_REMOTE_CONTROLL_SET_CLIPBOARD,  //���ü�����

};



extern "C" VOID CopyScreenData(PVOID SourceData, PVOID DestinationData, ULONG BufferLength);
// CDlgRemoteControll �Ի���

IMPLEMENT_DYNAMIC(CDlgRemoteControll, CDialog)

CDlgRemoteControll::CDlgRemoteControll(CWnd* pParent,
	_CIOCPServer* IOCPServer, PCONTEXT_OBJECT ContextObject)
	: CDialog(IDD_DIALOG_REMOTE_CONTROLL, pParent)
{
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;
	
	m_HorizontalScrollPosition = 0;
	m_VerticalScrollPosition = 0;

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

	if (m_ContextObject == NULL)
	{
		return;
	}
	ULONG BufferLength = m_ContextObject->m_ReceivedDecompressedBufferData.GetArrayLength() - 1;
	m_BitmapInfo = (BITMAPINFO*) new BYTE[BufferLength];
	if (m_BitmapInfo == NULL)
	{
		return;
	}
	//����λͼ��Ϣ
	memcpy(m_BitmapInfo, m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(1), BufferLength);

}

CDlgRemoteControll::~CDlgRemoteControll()
{
	if (m_BitmapInfo != NULL)
	{
		delete m_BitmapInfo;
		m_BitmapInfo = NULL;
	}
	
	::ReleaseDC(m_hWnd, m_WindowDCHandle);
	
	//���չ�����
	if (m_WindowMemoryDCHandle != NULL)
	{
		DeleteDC(m_WindowMemoryDCHandle);
		//���ٹ���
		DeleteObject(m_BitmapHandle);
		if (m_BitmapData != NULL)
		{
			m_BitmapData = NULL;
		}
		m_WindowMemoryDCHandle = NULL;
	}

}

void CDlgRemoteControll::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRemoteControll, CDialog)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CDlgRemoteControll ��Ϣ�������


BOOL CDlgRemoteControll::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_IconHwnd, FALSE);
	//��ÿͻ���IP��ַ
	CString v1;
	sockaddr_in ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength);
	v1.Format("\\\\%s - Զ�̿���", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//���öԻ������
	SetWindowText(v1);

	m_WindowDCHandle = ::GetDC(m_hWnd);
	m_WindowMemoryDCHandle = CreateCompatibleDC(m_WindowDCHandle);

	m_BitmapHandle = CreateDIBSection(m_WindowDCHandle, m_BitmapInfo,
		DIB_RGB_COLORS, &m_BitmapData, NULL, NULL);
	//��ʼ��������
	SelectObject(m_WindowMemoryDCHandle, m_BitmapHandle);

	SetWindowPos(NULL, 0, 0, m_BitmapInfo->bmiHeader.biWidth/2, m_BitmapInfo->bmiHeader.biHeight/2, 0);

	SetScrollRange(SB_HORZ, 0, m_BitmapInfo->bmiHeader.biWidth);  //ָ����������Χ�������Сֵ
	SetScrollRange(SB_VERT, 0, m_BitmapInfo->bmiHeader.biHeight);

	//���ϵͳ�˵�
	CMenu* Menu = GetSystemMenu(FALSE);
	if (Menu != NULL)
	{
		Menu->AppendMenu(MF_SEPARATOR);
		Menu->AppendMenu(MF_STRING, ID_MENU_DIALOG_REMOTE_CONTROLL_CONTROL, "������Ļ(&Y)");
		Menu->AppendMenu(MF_STRING, ID_MENU_DIALOG_REMOTE_CONTROLL_TRACE_CURSOR, "���ٱ��ض����(&T)");
		Menu->AppendMenu(MF_STRING, ID_MENU_DIALOG_REMOTE_CONTROLL_BLOCK_INPUT, "�������ض����ͼ���(&L)");
		Menu->AppendMenu(MF_STRING, ID_MENU_DIALOG_REMOTE_CONTROLL_SAVE_DIB, "�������(&S)");
		Menu->AppendMenu(MF_SEPARATOR);
		Menu->AppendMenu(MF_STRING, ID_MENU_DIALOG_REMOTE_CONTROLL_GET_CLIPBOARD, "��ȡ������(&R)");
		Menu->AppendMenu(MF_STRING, ID_MENU_DIALOG_REMOTE_CONTROLL_SET_CLIPBOARD, "���ļ�����(&L)");
		Menu->AppendMenu(MF_SEPARATOR);
	}

	//�ͻ����е����λ��
	m_CursorPosition.x = 0;
	m_CursorPosition.y = 0;

	//�ش�����
	BYTE IsToken = CLIENT_GO_ON;
	m_IOCPServer->OnPrepareSending(m_ContextObject, &IsToken, sizeof(BYTE));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
void CDlgRemoteControll::WindowHandleIO(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_ReceivedDecompressedBufferData.GetArray()[0])
	{
	case CLIENT_REMOTE_CONTROLL_FIRST_SCREEN:
	{
		DrawFirstScreen();  //��ʾ��һ֡����
		break;
	}
	case CLIENT_REMOTE_CONTROLL_NEXT_SCREEN:
	{
#define ALOGORITHM_DIFF 1

		if (m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(0)[1] == ALOGORITHM_DIFF)
		{
			DrawNextScreen();
		}
		break;
	}
	case CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REPLY:
	{
		//�����յ��Ŀͻ������ݷ��뵽���ض˵ļ��а���
		UpdataClipBoardData((char*)m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(1),
			m_ContextObject->m_ReceivedDecompressedBufferData.GetArrayLength() - 1);
		break;
	}
	
	}
}
void CDlgRemoteControll::OnClose()
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
VOID CDlgRemoteControll::DrawFirstScreen(void)
{
	//�õ����ض˷��������� �������ݿ������������еĹ�����
	memcpy(m_BitmapData, m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(1), m_BitmapInfo->bmiHeader.biSizeImage);
	PostMessage(WM_PAINT);
}
VOID CDlgRemoteControll::DrawNextScreen(void)
{
	BOOL IsChanged = FALSE;
	ULONG v1 = 1 + 1 + sizeof(POINT) + sizeof(BYTE);

	//ǰһ֡����
	LPVOID PreviousScreenData = m_BitmapData;
	//��õ�ǰ֡����
	LPVOID CurrentScreenData = m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(v1);

	ULONG CurrentScreenLength = m_ContextObject->m_ReceivedDecompressedBufferData.GetArrayLength() - v1;

	POINT PreviousCursorPosition;

	memcpy(&PreviousCursorPosition, &m_CursorPosition, sizeof(POINT));
	
	//���¹��λ��
	memcpy(&m_CursorPosition, m_ContextObject->m_ReceivedDecompressedBufferData.GetArray(2), sizeof(POINT));

	//����ƶ���
	if (memcmp(&PreviousCursorPosition, &m_CursorPosition, sizeof(POINT)) != 0)
	{
		IsChanged = TRUE;
	}
	//��Ļ�Ƿ�仯
	if (CurrentScreenLength > 0)
	{
		IsChanged = TRUE;
	}

	CopyScreenData(PreviousScreenData, CurrentScreenData, CurrentScreenLength);

	if (IsChanged)
	{
		//����б仯��ӦPAINT��Ϣ
		PostMessage(WM_PAINT);
	}

}
void CDlgRemoteControll::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()

	//���������еĹ��߸�����
	BitBlt(m_WindowDCHandle, 0, 0,
		m_BitmapInfo->bmiHeader.biWidth,
		m_BitmapInfo->bmiHeader.biHeight,
		m_WindowMemoryDCHandle,
		m_HorizontalScrollPosition,
		m_VerticalScrollPosition,
		SRCCOPY);

	if (m_IsTraceCursor)
	{
		DrawIconEx(m_WindowDCHandle,
			m_CursorPosition.x - m_HorizontalScrollPosition,
			m_CursorPosition.y - m_VerticalScrollPosition,
			m_IconHwnd,
			0, 0,
			0,
			NULL,
			DI_NORMAL | DI_COMPAT
			);
	}

}


void CDlgRemoteControll::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO ScrollInfo;
	int i;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &ScrollInfo);

	switch (nSBCode)
	{
	case SB_LINEUP:
	{
		i = nPos - 1;
		break;
	}
	case SB_LINEDOWN:
	{
		i = nPos + 1;
		break;
	}
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
	{
		i = ScrollInfo.nTrackPos;
		break;
	}
	default:
		return;
	}

	i = max(i, ScrollInfo.nMin);
	i = min(i, (int)(ScrollInfo.nMax - ScrollInfo.nPage + 1));

	RECT Rect;
	GetClientRect(&Rect);

	if ((Rect.bottom + i) > m_BitmapInfo->bmiHeader.biHeight)
	{
		i = m_BitmapInfo->bmiHeader.biHeight - Rect.bottom;
	}

	InterlockedExchange((PLONG)&m_VerticalScrollPosition, i);

	SetScrollPos(SB_VERT, i);
	OnPaint();

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CDlgRemoteControll::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO ScrollInfo;
	int i;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &ScrollInfo);

	switch (nSBCode)
	{
	case SB_LINEUP:
	{
		i = nPos - 1;
		break;
	}
	case SB_LINEDOWN:
	{
		i = nPos + 1;
		break;
	}
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
	{
		i = ScrollInfo.nTrackPos;
		break;
	}
	default:
		return;
	}

	i = max(i, ScrollInfo.nMin);
	i = min(i, (int)(ScrollInfo.nMax - ScrollInfo.nPage + 1));

	RECT Rect;
	GetClientRect(&Rect);

	if ((Rect.right + i) > m_BitmapInfo->bmiHeader.biWidth)
	{
		i = m_BitmapInfo->bmiHeader.biWidth - Rect.right;
	}

	InterlockedExchange((PLONG)&m_HorizontalScrollPosition, i);

	SetScrollPos(SB_HORZ, i);
	OnPaint();


	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
//����Ϣ���Ͱ���ϵͳ�˵��ȴ����ڴ�����Ϣ
void CDlgRemoteControll::OnSysCommand(UINT nID, LPARAM lParam)
{
	CMenu* Menu = GetSystemMenu(FALSE);
	switch (nID)
	{
	case ID_MENU_DIALOG_REMOTE_CONTROLL_CONTROL:  //Զ�̿���
	{
		m_IsControl = !m_IsControl;
		//���ò˵���ʽ
		Menu->CheckMenuItem(ID_MENU_DIALOG_REMOTE_CONTROLL_CONTROL, m_IsControl ? MF_CHECKED : MF_UNCHECKED);
		break;
	}
	case ID_MENU_DIALOG_REMOTE_CONTROLL_TRACE_CURSOR:  //���ٱ��ض����
	{
		m_IsTraceCursor = !m_IsTraceCursor;
		Menu->CheckMenuItem(ID_MENU_DIALOG_REMOTE_CONTROLL_TRACE_CURSOR, m_IsTraceCursor ? MF_CHECKED : MF_UNCHECKED);

		break;
	}
	case ID_MENU_DIALOG_REMOTE_CONTROLL_BLOCK_INPUT:   //�����ͻ������ͼ���
	{
		BOOL IsChecked = Menu->GetMenuState(ID_MENU_DIALOG_REMOTE_CONTROLL_BLOCK_INPUT, MF_BYCOMMAND) & MF_CHECKED;
		Menu->CheckMenuItem(ID_MENU_DIALOG_REMOTE_CONTROLL_BLOCK_INPUT, IsChecked ? MF_UNCHECKED : MF_CHECKED);

		BYTE IsToken[2];
		IsToken[0] = CLIENT_REMOTE_CONTROLL_BLOCK_INPUT_REQUEST;
		IsToken[1] = !IsChecked;
		m_IOCPServer->OnPrepareSending(m_ContextObject, IsToken, sizeof(IsToken));

		break;
	}
	case ID_MENU_DIALOG_REMOTE_CONTROLL_SAVE_DIB:  //�����ͼ
	{
		SaveSnapShotData();
		break;
	}
	case ID_MENU_DIALOG_REMOTE_CONTROLL_GET_CLIPBOARD:  //��ȡ�ͻ��˼���������
	{
		BYTE IsToken = CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REQUEST;
		m_IOCPServer->OnPrepareSending(m_ContextObject, &IsToken, sizeof(IsToken));
		break;
	}
	case ID_MENU_DIALOG_REMOTE_CONTROLL_SET_CLIPBOARD:  //���ÿͻ��˼���������
	{
		//�ӵ�ǰ�������еļ������л�ȡ���ݺ��͵��ͻ���
		SendClipboardData();
		break;
	}
	}

	CDialog::OnSysCommand(nID, lParam);
}
BOOL CDlgRemoteControll::PreTranslateMessage(MSG* pMsg)
{
	//���˼��������Ϣ
#define MAKEDWORD(h,l)   (((unsigned long)h<<16) | 1)
	
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	{
		MSG Msg;
		memcpy(&Msg, pMsg, sizeof(MSG));
		Msg.lParam = MAKEDWORD(HIWORD(pMsg->lParam) + m_VerticalScrollPosition, LOWORD(pMsg->lParam) + m_HorizontalScrollPosition);
		Msg.pt.x += m_HorizontalScrollPosition;
		Msg.pt.y += m_VerticalScrollPosition;
		OnPrepareSending(&Msg);
		break;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	{
		if (pMsg->wParam != VK_LWIN&&pMsg->wParam != VK_RWIN)
		{
			MSG Msg;
		    memcpy(&Msg, pMsg, sizeof(MSG));
		    Msg.lParam = MAKEDWORD(HIWORD(pMsg->lParam) + m_VerticalScrollPosition, LOWORD(pMsg->lParam) + m_HorizontalScrollPosition);
		    Msg.pt.x += m_HorizontalScrollPosition;
		    Msg.pt.y += m_VerticalScrollPosition;
		    OnPrepareSending(&Msg);
		}
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return true;

		break;
	}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
VOID CDlgRemoteControll::OnPrepareSending(MSG* Msg)
{
	if (!m_IsControl)
	{
		return;
	}
	LPBYTE BufferData = new BYTE[sizeof(MSG) + 1];
	BufferData[0] = CLIENT_REMOTE_CONTROLL_CONTROL_REQUEST;
	memcpy(BufferData + 1, Msg, sizeof(MSG));
	m_IOCPServer->OnPrepareSending(m_ContextObject, BufferData, sizeof(MSG) + 1);
	delete[] BufferData;
}
BOOL CDlgRemoteControll::SaveSnapShotData(void)
{
	//λͼ����
	CString FileFullPath = CTime::GetCurrentTime().Format("%Y-%m-%d_%H-%M-%S.bmp");
	CFileDialog Dialog(FALSE, "bmp", FileFullPath, OFN_OVERWRITEPROMPT, "λͼ�ļ�(*.bmp)|*.bmp|", this);
	if (Dialog.DoModal() != IDOK)
		return FALSE;

	BITMAPFILEHEADER BitmapFileHeader;
	LPBITMAPINFO BitmapInfo = m_BitmapInfo;
	CFile FileHandle;
	//����һ���ļ�
	if (!FileHandle.Open(Dialog.GetPathName(), CFile::modeWrite| CFile::modeCreate))
	{
		return FALSE;
	}
	//BITMAPINFO��С
	int BitmapInfoLength = sizeof(BITMAPINFO);
	//Э��  TCP У��ֵ
	BitmapFileHeader.bfType      = ((WORD)('M' << 8) | 'B');
	BitmapFileHeader.bfSize      = BitmapInfo->bmiHeader.biSizeImage + sizeof(BITMAPFILEHEADER);
	BitmapFileHeader.bfReserved1 = 0;
	BitmapFileHeader.bfReserved2 = 0;
	BitmapFileHeader.bfOffBits   = sizeof(BITMAPFILEHEADER) + BitmapInfoLength;

	FileHandle.Write(&BitmapFileHeader, sizeof(BITMAPFILEHEADER));
	FileHandle.Write(BitmapInfo, BitmapInfoLength);

	FileHandle.Write(m_BitmapData, BitmapInfo->bmiHeader.biSizeImage);
	FileHandle.Close();
	return true;
}
VOID CDlgRemoteControll::UpdataClipBoardData(char* BufferData, ULONG BufferLength)
{
	if (!::OpenClipboard(NULL))
	{
		return;
	}
	//��ռ�����
	::EmptyClipboard();
	//�����ڴ�
	HGLOBAL GlobalHandle = GlobalAlloc(GPTR, BufferLength);
	if (GlobalHandle != NULL)
	{
		char* v5 = (LPTSTR)GlobalLock(GlobalHandle);  //��������ҳ��
		memcpy(v5, BufferData, BufferLength);
		GlobalUnlock(GlobalHandle);
		SetClipboardData(CF_TEXT, GlobalHandle);
		GlobalFree(GlobalHandle);
	}
	CloseClipboard();
}
VOID CDlgRemoteControll::SendClipboardData(void)
{
	//�򿪼������豸
	if (!::OpenClipboard(NULL))
	{
		return;
	}
	HGLOBAL GlobalHandle = GetClipboardData(CF_TEXT);  //������һ���ڴ�
	if (GlobalHandle == NULL)
	{
		CloseClipboard();
		return;
	}
	//ͨ�������������������ڴ�
	int BufferLength = GlobalSize(GlobalHandle) + 1;
	char* v5 = (LPSTR)GlobalLock(GlobalHandle);
	LPBYTE BufferData = new BYTE[BufferLength];
	
	BufferData[0] = CLIENT_REMOTE_CONTROLL_SET_CLIPBOARD_REPLY;
	memcpy(BufferData + 1, v5, BufferLength - 1);
	::GlobalUnlock(GlobalHandle);
	::CloseClipboard();
	m_IOCPServer->OnPrepareSending(m_ContextObject, (PBYTE)BufferData, BufferLength);
	delete[] BufferData;
}