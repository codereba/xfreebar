// AdvancedTab.cpp : implementation file
//

#include "stdafx.h"
#include "CTabCtrlSSL_demo.h"
#include "AdvancedTab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvancedTab dialog


CAdvancedTab::CAdvancedTab (CWnd* pParent /*=NULL*/)
	: CTabPageSSL (CAdvancedTab::IDD, pParent) {
	//{{AFX_DATA_INIT(CAdvancedTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAdvancedTab::DoDataExchange (CDataExchange* pDX) {
	CTabPageSSL::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvancedTab)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvancedTab, CTabPageSSL)
	//{{AFX_MSG_MAP(CAdvancedTab)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED, OnButtonAdvanced)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvancedTab message handlers

void CAdvancedTab::OnButtonAdvanced () {
	AfxMessageBox (_T("Button clicked handled in CTabPageSSL-derived class"));
}