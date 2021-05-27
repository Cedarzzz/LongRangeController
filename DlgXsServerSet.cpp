// DlgXsServerSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XsServer.h"
#include "DlgXsServerSet.h"
#include "afxdialogex.h"


// CDlgXsServerSet �Ի���
extern _CConfigFile __ConfigFile;

IMPLEMENT_DYNAMIC(CDlgXsServerSet, CDialog)

CDlgXsServerSet::CDlgXsServerSet(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_SERVER_SET, pParent)
	, m_CEdit_Dialog_Server_Set_Max_Connection(0)
	, m_CEdit_Dialog_Server_Set_Listen_Port(0)
{

}

CDlgXsServerSet::~CDlgXsServerSet()
{
}

void CDlgXsServerSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIALOG_SERVER_SET_MAX_CONNECTION, m_CEdit_Dialog_Server_Set_Max_Connection);
	DDX_Text(pDX, IDC_EDIT_DIALOG_SERVER_SET_LISTEN_PORT, m_CEdit_Dialog_Server_Set_Listen_Port);
	DDV_MinMaxLong(pDX, m_CEdit_Dialog_Server_Set_Max_Connection, 1, 10);
	DDV_MinMaxLong(pDX, m_CEdit_Dialog_Server_Set_Listen_Port, 2048, 65536);
	DDX_Control(pDX, IDC_EDIT_DIALOG_SERVER_SET_APPLY, m_CButton_Dialog_Server_Set_Apply);
}


BEGIN_MESSAGE_MAP(CDlgXsServerSet, CDialog)
	ON_BN_CLICKED(IDC_EDIT_DIALOG_SERVER_SET_APPLY, &CDlgXsServerSet::OnBnClickedEditDialogServerSetApply)
	ON_EN_CHANGE(IDC_EDIT_DIALOG_SERVER_SET_MAX_CONNECTION, &CDlgXsServerSet::OnEnChangeEditDialogServerSetMaxConnection)
	ON_EN_CHANGE(IDC_EDIT_DIALOG_SERVER_SET_LISTEN_PORT, &CDlgXsServerSet::OnEnChangeEditDialogServerSetListenPort)
END_MESSAGE_MAP()


// CDlgXsServerSet ��Ϣ�������


void CDlgXsServerSet::OnBnClickedEditDialogServerSetApply()
{
	UpdateData(TRUE);   //�����޸ĵ�����

	//��ini�ļ�д��ֵ
	__ConfigFile.SetInt("Settings", "ListenPort", m_CEdit_Dialog_Server_Set_Listen_Port);
	__ConfigFile.SetInt("Settings", "MaxConnection", m_CEdit_Dialog_Server_Set_Max_Connection);

	SendMessage(WM_CLOSE);
}
void CDlgXsServerSet::OnEnChangeEditDialogServerSetMaxConnection()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	m_CButton_Dialog_Server_Set_Apply.ShowWindow(SW_NORMAL);
	m_CButton_Dialog_Server_Set_Apply.EnableWindow(TRUE);
}
void CDlgXsServerSet::OnEnChangeEditDialogServerSetListenPort()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	m_CButton_Dialog_Server_Set_Apply.ShowWindow(SW_NORMAL);
	m_CButton_Dialog_Server_Set_Apply.EnableWindow(TRUE);
}


BOOL CDlgXsServerSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	int ListenPort = 0;
	int MaxConnection = 0;

	//���������õ�Dlg
	ListenPort = __ConfigFile.GetInt("Settings", "ListenPort");
	//��ȡini�ļ��еļ����˿�
	MaxConnection = __ConfigFile.GetInt("Settings", "MaxConnection");

	m_CEdit_Dialog_Server_Set_Listen_Port = ListenPort;
	m_CEdit_Dialog_Server_Set_Max_Connection = MaxConnection;

	//���ڿؼ���value����ҪUpdateData    False           v1      �ؼ�(�ڴ�)
	//             ����                  CListControl    v1      ����v1���ǲ����ؼ�
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
