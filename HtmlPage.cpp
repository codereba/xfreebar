// Options.cpp : implementation file
// COPYRIGHT (C) 1999-2000 by Mike Melnikov zmike@andnow.ru
//

#include "stdafx.h"
#include "HtmlPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW                   
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
                                            

LRESULT CHtmlPage::OnInitDialog(HWND,LPARAM)
{
  m_wasComplete = false;
  m_wasShow = false;                            

  GetDlgControl(IDC_EXPLORER1, __uuidof(IWebBrowser2), (void**)&m_browser);
  //AtlAdviseSinkMap(this, true);
  m_browser->Navigate(m_url,0,0,0,0);
  return 0;
}

void CHtmlPage::OnProgressChange(long Progress, long ProgressMax) 
{
}

void CHtmlPage::OnTitleChange(wchar_t* Text) 
{
}                

void CHtmlPage::OnDocumentComplete(LPDISPATCH pDisp, VARIANT FAR* URL) 
{
	LPDISPATCH pDispatch;
	m_browser->get_Document(&pDispatch);

  IHTMLDocument2Ptr lpHtmlDocument = pDispatch;

  IHTMLElement *lpBodyElm;
	lpHtmlDocument->get_body(&lpBodyElm);

  if(lpBodyElm)
  {
/*    IHTMLElement2 *lpBody;
    lpBodyElm->QueryInterface(IID_IHTMLElement2,(void**)&lpBody);
    lpBodyElm->Release();

    long h;
    lpBody->get_scrollHeight(&h);
*/
    
  }

  m_wasComplete = true;
}

void CHtmlPage::OnBeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{
  _bstr_t url = URL->bstrVal; 
  if(m_wasComplete)//url!=m_url)
  {
    m_parentBrowser->Navigate(url,0,0,0,0);

    m_parentBrowser = NULL;
    DestroyWindow();
    *Cancel = 1;
  }
}


void CHtmlPage::OnStatusTextChange(wchar_t* Text) 
{
}
                         
void CHtmlPage::OnSize(UINT wParam, CSize size)
{
  ::MoveWindow(GetDlgItem(IDC_EXPLORER1),0,0,size.cx,size.cy,TRUE);
}

void CHtmlPage::OnLooseFocus(UINT uMsg, BOOL wParam, HWND lParam)
{
  if(m_wasShow && wParam==FALSE && m_parentBrowser!=NULL)
  {
     DestroyWindow();
  }
}

