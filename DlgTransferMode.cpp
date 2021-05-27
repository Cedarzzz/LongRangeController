// DlgTransferMode.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XsServer.h"
#include "DlgTransferMode.h"
#include "afxdialogex.h"


// CDlgTransferMode �Ի���

IMPLEMENT_DYNAMIC(CDlgTransferMode, CDialog)

CDlgTransferMode::CDlgTransferMode(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_TRANSFER_MODE, pParent)
	, m_CString_Dialog_Transfer_Mode_Tips(_T(""))
{

}

CDlgTransferMode::~CDlgTransferMode()
{
}

void CDlgTransferMode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_DIALOG_TRANSFER_MODE_TIPS, m_CString_Dialog_Transfer_Mode_Tips);
}

void CDlgTransferMode::OnEndDialog(UINT id)
{
	EndDialog(id);
}


BEGIN_MESSAGE_MAP(CDlgTransferMode, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_DIALOG_TRANSFER_MODE_COVER, IDC_BUTTON_DIALOG_TRANSFER_MODE_JUMP_ALL, OnEndDialog)
END_MESSAGE_MAP()


// CDlgTransferMode ��Ϣ�������

BOOL CDlgTransferMode::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString Tips;
	Tips.Format("ʧ��..\" %s \" ", m_FileName);

	for (int i = 0; i < Tips.GetLength(); i += 120)
	{
		Tips.Insert(i, "\n");
		i += 1;
	}
	SetDlgItemText(IDC_STATIC_DIALOG_TRANSFER_MODE_TIPS, Tips);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
