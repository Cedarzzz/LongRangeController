// DlgCreateClient.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XsServer.h"
#include "DlgCreateClient.h"
#include "afxdialogex.h"


struct _CONNECT_INFORMATION_
{
	DWORD    CheckFlag;
	char     ConnectIP[20];    //������IP��ַ
	USHORT   ConnectPort;      //���Ӷ˿�
}__ConnectInfo = { 0x87654321,"",0  };

// CDlgCreateClient �Ի���

IMPLEMENT_DYNAMIC(CDlgCreateClient, CDialog)

CDlgCreateClient::CDlgCreateClient(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_CREATE_CLIENT, pParent)
	, m_CEdit_Dialog_Create_Client_Connect_IP(_T("������һ�����ӵ�IP"))
	, m_CEdit_Dialog_Create_Client_Connect_Port(_T("������һ�����ӵ�Port"))
{

}

CDlgCreateClient::~CDlgCreateClient()
{
}

void CDlgCreateClient::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIALOG_CREATE_CLIENT_CONNECT_IP, m_CEdit_Dialog_Create_Client_Connect_IP);
	DDX_Text(pDX, IDC_EDIT_DIALOG_CREATE_CLIENT_CONNECT_PORT, m_CEdit_Dialog_Create_Client_Connect_Port);
}


BEGIN_MESSAGE_MAP(CDlgCreateClient, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgCreateClient::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCreateClient ��Ϣ�������


void CDlgCreateClient::OnBnClickedOk()
{
	CFile v1;

	UpdateData(TRUE);
	USHORT ConnectPort = atoi(m_CEdit_Dialog_Create_Client_Connect_Port);
	strcpy(__ConnectInfo.ConnectIP, m_CEdit_Dialog_Create_Client_Connect_IP);

	if (ConnectPort < 0 || ConnectPort>65536)
	{
		__ConnectInfo.ConnectPort = 2356;
	}
	else
	{
		__ConnectInfo.ConnectPort = ConnectPort;
	}

	char v2[MAX_PATH];
	ZeroMemory(v2, MAX_PATH);

	LONGLONG BufferLength = 0;
	BYTE* BufferData = NULL;
	CString v3;
	CString FileFullPath;
	try
	{
		//�˴��õ�δ����ǰ�ļ���
		GetModuleFileName(NULL, v2, MAX_PATH);
		v3 = v2;
		int Position = v3.ReverseFind('\\');   //����������ҵ�һ�γ��ֵ�\\

		v3 = v3.Left(Position);

		FileFullPath = v3 + "\\XsClient.exe";    //�õ���ǰδ�����ļ���
		//���ļ�
		v1.Open(FileFullPath, CFile::modeRead | CFile::typeBinary);

		BufferLength = v1.GetLength();
		BufferData = new BYTE[BufferLength];
		ZeroMemory(BufferData, BufferLength);

		//��ȡ�ļ�����
		v1.Read(BufferData, BufferLength);
		//�ر��ļ�
		v1.Close();


		//���ڴ�������0x87654321
		int Offset = SeMemoryFind((char*)BufferData, (char*)&__ConnectInfo.CheckFlag,
			BufferLength, sizeof(DWORD));
		//���Լ����õ���Ϣ������exe���ڴ���
		memcpy(BufferData + Offset, &__ConnectInfo, sizeof(__ConnectInfo));
		//���浽�ļ���
		v1.Open(FileFullPath, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite);
		//�����ļ��е�����
		v1.Write(BufferData, BufferLength);
		v1.Close();
		delete[] BufferData;
		MessageBox("���ɳɹ�");
	}
	catch (CMemoryException* e)
	{
		MessageBox("�ڴ治��");
	}
	catch (CFileException* e)
	{
		MessageBox("�ļ���������");
	}
	catch (CException* e)
	{
		MessageBox("δ֪����");
	}
	CDialog::OnOK();
}
int CDlgCreateClient::SeMemoryFind(const char* BufferData, const char* KeyValue, int BufferLength, int KeyLength)
{
	int i, j;
	if (KeyLength == 0 || BufferLength == 0)
	{
		return -1;
	}
	for (i = 0; i < BufferLength; i++)
	{
		for (j = 0; j < KeyLength; j++)
		{
			if (BufferData[i + j] != KeyValue[j])
			{
				break;
			}
		}
		if (j == KeyLength)
		{
			return i;
		}
	}
	return -1;
}