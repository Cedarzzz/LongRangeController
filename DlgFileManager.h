#pragma once
#include "_IOCPServer.h"
#include "Common.h"
#include "TrueColorToolBar.h"
#include "afxcmn.h"
#include "afxwin.h"

// CDlgFileManager �Ի���

typedef struct
{
	DWORD FileSizeHigh;
	DWORD FileSizeLow;
}FILE_SIZE;

#define MAKEINT64(low,high)  ((unsigned __int64)(((DWORD)(low))|((unsigned __int64)((DWORD)(high)))<<32))
typedef CList<CString, CString&>List;
class CDlgFileManager : public CDialog
{
	DECLARE_DYNAMIC(CDlgFileManager)

public:
	CDlgFileManager(CWnd* pParent = NULL, _CIOCPServer* IOCPServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // ��׼���캯��
	virtual ~CDlgFileManager();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILE_MANAGER };
#endif
private:
	_CIOCPServer* m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON m_IconHwnd;
	BYTE m_ClientData[0x1000];
	BYTE m_ServerData[0x1000];
	
	CString m_ServerFileFullPath;
	CString m_ClientFileFullPath;
	//�������
	CImageList* m_CImageList_Large;
	CImageList* m_CImageList_Small;
	CTrueColorToolBar m_ToolBar_Dialog_File_Manager_Server_File;   //����������
	CTrueColorToolBar m_ToolBar_Dialog_File_Manager_Client_File;

	CListCtrl m_CListCtrl_Dialog_File_Manager_Server_File;   //�����б��
	CListCtrl m_CListCtrl_Dialog_File_Manager_Client_File;
	CStatusBar        m_StatusBar;
	CProgressCtrl*    m_ProgressCtrl;

	CStatic m_CStatic_Dialog_File_Manager_Server_Position;
	CStatic m_CStatic_Dialog_File_Manager_Client_Position;
	CComboBox m_CComboBox_Dialog_File_Manager_Server_File;
	CComboBox m_CComboBox_Dialog_File_Manager_Client_File;
	//�ļ���ק����
	CListCtrl* m_DragListControl;  //�ļ���ק����  �����
	CListCtrl* m_DropListControl;  //�ļ���ק����  �ͻ���
	HCURSOR    m_CursorHwnd;    //���ͼ��
	BOOL       m_IsDragging = FALSE;    //��ק�ļ�
	BOOL       m_IsStop = FALSE;      //�����ļ��Ĺ����п���ֹͣ
	List       m_ServerDataToClientJob;   //���д�������
	CString    m_SourFileFullPath;   //��������
	CString    m_DestFileFullPath;
	__int64    m_TransferFileLength;  //��ǰ�����ļ����ܴ�С
	ULONG      m_TransferMode = TRANSFER_MODE_NORMAL;
	//������
	__int64    m_Counter;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	VOID CDlgFileManager::FixedServerVolumeData();
	VOID CDlgFileManager::FixedClientVolumeData();
	VOID CDlgFileManager::FixedServerFileData(CString Directory = "");
	VOID CDlgFileManager::GetClientFileData(CString Directory = "");
	VOID CDlgFileManager::FixedClientFileData(BYTE* BufferData, ULONG BufferLength);
	int CDlgFileManager::GetIconIndex(LPCTSTR VolumnName, DWORD FileAttributes);
	CString CDlgFileManager::GetParentDirectory(CString FileFullPath);
	void CDlgFileManager::WindowHandleIO(void);

	afx_msg void OnNMDblclkListDialogFileManagerServerFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListDialogFileManagerClientFile(NMHDR *pNMHDR, LRESULT *pResult);
	//���ض˰�ť����
	afx_msg void OnFileManagerServerFilePrevious();
	afx_msg void OnFileManagerServerFileDelete();
	afx_msg void OnFileManagerServerFileNewFolder();
	afx_msg void OnFileManagerServerFileStop();
	afx_msg void OnFileManagerServerFileViewSmall();
	afx_msg void OnFileManagerServerFileViewList();
	afx_msg void OnFileManagerServerFileViewDetail();

	//�ͻ��˰�ť����
	afx_msg void OnFileManagerClientFilePrevious();
	afx_msg void OnFileManagerClientFileDelete();
	afx_msg void OnFileManagerClientFileNewFolder();
	afx_msg void OnFileManagerClientFileStop();
	afx_msg void OnFileManagerClientFileViewSmall();
	afx_msg void OnFileManagerClientFileViewList();
	afx_msg void OnFileManagerClientFileViewDetail();

	void CDlgFileManager::EnableControl(BOOL IsEnable);
	BOOL CDlgFileManager::DeleteDirectory(LPCTSTR DirectoryFullPath);
	BOOL CDlgFileManager::MakeSureDirectoryPathExists(char* DirectoryFullPath);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLvnBegindragListDialogFileManagerServerFile(NMHDR *pNMHDR, LRESULT *pResult);  //�ӷ���˵��ͻ��˵��ļ�����
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	VOID CDlgFileManager::DropDataOnList();  //�ж����ݴ������� �ֱ���ú���
	VOID CDlgFileManager::CopyServerDataToClient();  //�����ض˵��ͻ���
	BOOL CDlgFileManager::FixedServerDataToClient(LPCTSTR DirectoryFullPath);  //����Ŀ¼�е�����
	BOOL CDlgFileManager::SendServerDataToClient();  //�������ݵ����ض�
	VOID CDlgFileManager::SendTransferMode();  //�����ض���ͬ���ļ�
	VOID CDlgFileManager::EndCopyServerFileToClient();  //��������
	void CDlgFileManager::ShowProgress();
	VOID CDlgFileManager::SendServerFileDataToClient();
};
