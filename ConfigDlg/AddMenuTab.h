#pragma once


// CAddMenuTab �Ի���
#include "TabPageSSL.h"
class CAddMenuTab : public CTabPageSSL
{
	DECLARE_DYNAMIC(CAddMenuTab)

public:
	CAddMenuTab(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddMenuTab();

// �Ի�������
	enum { IDD = IDD_TAB_ADDMENU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton();
};
