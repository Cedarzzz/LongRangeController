#pragma once


// CDlgNewFolder �Ի���

class CDlgNewFolder : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewFolder)

public:
	CDlgNewFolder(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgNewFolder();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NEW_FOLDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_CEdit_Dialog_New_Folder_Main;
};
