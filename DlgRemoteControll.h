#pragma once
#include "_IOCPServer.h"
#include "Common.h"

// CDlgRemoteControll �Ի���

class CDlgRemoteControll : public CDialog
{
	DECLARE_DYNAMIC(CDlgRemoteControll)

public:
	CDlgRemoteControll(CWnd* pParent = NULL, _CIOCPServer* IOCPServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // ��׼���캯��
	virtual ~CDlgRemoteControll();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REMOTE_CONTROLL };
#endif
private:
	_CIOCPServer* m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON m_IconHwnd;
	LPBITMAPINFO m_BitmapInfo;  //����
	HBITMAP m_BitmapHandle;  //����
	PVOID m_BitmapData;  //����
	HDC m_WindowDCHandle;  //����
	HDC m_WindowMemoryDCHandle;  //���˵Ĺ�����
	POINT m_CursorPosition;  //�洢���λ��
	ULONG m_HorizontalScrollPosition;
	ULONG m_VerticalScrollPosition;

	BOOL m_IsTraceCursor = FALSE;  //���ٹ��켣
	BOOL m_IsControl = FALSE;  //Զ�̿���
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	void CDlgRemoteControll::WindowHandleIO(void);
	VOID CDlgRemoteControll::DrawFirstScreen(void);  //��ʾ��һ֡����
	VOID CDlgRemoteControll::DrawNextScreen(void);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	VOID CDlgRemoteControll::OnPrepareSending(MSG* Msg);
	BOOL CDlgRemoteControll::SaveSnapShotData(void);
	VOID CDlgRemoteControll::UpdataClipBoardData(char* BufferData, ULONG BufferLength);
	VOID CDlgRemoteControll::SendClipboardData(void);
};
