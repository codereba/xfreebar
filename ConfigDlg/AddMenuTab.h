#pragma once


// CAddMenuTab 对话框
#include "TabPageSSL.h"
class CAddMenuTab : public CTabPageSSL
{
	DECLARE_DYNAMIC(CAddMenuTab)

public:
	CAddMenuTab(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddMenuTab();

// 对话框数据
	enum { IDD = IDD_TAB_ADDMENU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton();
};
