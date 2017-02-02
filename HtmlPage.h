#if !defined(AFX_OPTIONS_H__7D5CE5E2_2312_11D3_B5C4_0020AFDD89D7__INCLUDED_)
#define AFX_OPTIONS_H__7D5CE5E2_2312_11D3_B5C4_0020AFDD89D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Options.h : header file
// COPYRIGHT (C) 1999-2000 by Mike Melnikov zmike@andnow.ru
//

#include <ATLHOST.H>
#include <ATLDLGS.H>
#include "resource.h"
#include <ATLcrack.H>

class CHtmlPage : public 
                           //CWindowImpl<CHtmlPage,CAxWindow>, 
                            CAxDialogImpl<CHtmlPage>,
                           public IDispEventImpl<IDC_EXPLORER1, CHtmlPage>
{
 public:
    enum {IDD = IDD_UPDATE_VER};

//   DECLARE_WND_CLASS(_T("MSHTML_Toolbar_Page"))

  BEGIN_SINK_MAP(CHtmlPage)
	  //SINK_ENTRY(IDC_EXPLORER1, 108 , OnProgressChange)//, VTS_I4 VTS_I4)
	  //SINK_ENTRY(IDC_EXPLORER1, 113 , OnTitleChange)//, VTS_BSTR)
	  //SINK_ENTRY(IDC_EXPLORER1, 259 , OnDocumentComplete) //, VTS_DISPATCH VTS_PVARIANT)
	  //SINK_ENTRY(IDC_EXPLORER1, 102 , OnStatusTextChange) //, VTS_BSTR)
	  //SINK_ENTRY(IDC_EXPLORER1, 250 , OnBeforeNavigate2Explorer1) //, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
  END_SINK_MAP()
                                  
  _bstr_t                m_url;
  IWebBrowser2Ptr        m_parentBrowser;
  bool                   m_wasShow;
  CComPtr<IWebBrowser2>  m_browser;

 protected:

  LRESULT OnInitDialog(HWND,LPARAM);

	void __stdcall OnProgressChange(long Progress, long ProgressMax);
	void __stdcall OnTitleChange(wchar_t* Text);
	void __stdcall OnDocumentComplete(LPDISPATCH pDisp, VARIANT FAR* URL);
	void __stdcall OnStatusTextChange(wchar_t* Text);
  void __stdcall OnBeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);

  BEGIN_MSG_MAP_EX(CHtmlPage)
    MSG_WM_INITDIALOG(OnInitDialog)
    //MSG_WM_ACTIVATE(OnLooseFocus)
    MSG_WM_SIZE(OnSize)
		COMMAND_RANGE_HANDLER(IDOK, IDNO, OnCloseCmd)
//		CHAIN_MSG_MAP( CAxDialogImpl<CHtmlPage> )
  END_MSG_MAP()

	void OnSize(UINT wParam, CSize size);

  void OnLooseFocus(UINT uMsg, BOOL wParam, HWND lParam);

  bool m_wasComplete;

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		::EndDialog(m_hWnd, wID);
		return 0;
	}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONS_H__7D5CE5E2_2312_11D3_B5C4_0020AFDD89D7__INCLUDED_)
