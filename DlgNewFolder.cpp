// DlgNewFolder.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XsServer.h"
#include "DlgNewFolder.h"
#include "afxdialogex.h"


// CDlgNewFolder �Ի���

IMPLEMENT_DYNAMIC(CDlgNewFolder, CDialog)

CDlgNewFolder::CDlgNewFolder(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_NEW_FOLDER, pParent)
	, m_CEdit_Dialog_New_Folder_Main(_T(""))
{

}

CDlgNewFolder::~CDlgNewFolder()
{
}

void CDlgNewFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIALOG_NEW_FOLDER_MAIN, m_CEdit_Dialog_New_Folder_Main);
}


BEGIN_MESSAGE_MAP(CDlgNewFolder, CDialog)
END_MESSAGE_MAP()


// CDlgNewFolder ��Ϣ�������
