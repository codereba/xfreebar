/*
 * Copyright 2010 www.codereba.com
 *
 * This file is part of xfreebar.
 *
 * xfreebar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xfreebar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xfreebar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "stdafx.h"
#include "XToolBarHost.h"
#include <wchar.h>
#include "htmleventhandler.h"
#include "XWnd.h"

const WCHAR TITLE_XTOOLBARHOST[] = L"";


#ifndef DBIMF_USECHEVRON
#define DBIMF_USECHEVRON 0x0080
#endif 

#ifndef DBIMF_BREAK
#define DBIMF_BREAK 0x0100
#endif

#define BLK_COUNT_REG	_T("BlkCount")

void __cdecl ReEnable(void* lpParam);

/////////////////////////////////////////////////////////////////////////////
// XToolBarHost
bool CheckUpdate(XString &strFile, bool update);

INT32 OnDllInitialize()
{
	//Initialize log file
	TCHAR szLogFileName[_MAX_PATH];
	TCHAR szModulePath[_MAX_PATH];
	if( 0 > GetXmlConfDir( szModulePath, _MAX_PATH ) )
		return S_FALSE;

	_tcscpy( szLogFileName, szModulePath );
	_tcscat( szLogFileName, XBAR_LOG_PATH );

	g_XLog.SetFile( szLogFileName, FALSE);
	
	ATOM ClassAtom;
	if(FALSE == (ClassAtom = RegisterXFreeWnd()))
	{
		DWORD dwError = ::GetLastError();
		ATLTRACE("Register the xfree windows class failed, error code is %d\n", dwError);
	}

	return 0;
}

XToolBarHost::XToolBarHost(): 
							m_dwBandID( 0 ), 
							m_dwViewMode( 0 ), 
							m_bShow( FALSE ), 
							m_bEnterHelpMode( FALSE ), 
							m_hWndParent( NULL ), 
							m_pSite( NULL ),
							m_bTempBlock( true ),
							m_bPlaySound( true ),
							m_regBlkCount( BLK_COUNT_REG ),
							m_bUserClickedLink( FALSE )
{
}

XToolBarHost::~XToolBarHost()
{
}

#include <AtlFrame.h>
#include "XFunction.h"

BOOL XToolBarHost::RegisterAndCreateWindow()
{
	RECT rectClientParent;

	OnDllInitialize();

	::GetClientRect( m_hWndParent, &rectClientParent);

	m_wndToolBarCtrl.m_pToolBarHost = this;
	m_wndInvisibleChildWnd.m_pToolBarHost = this;

	// We need to create an Invisible Child Window using the Parent Window, this will also be used to reflect Command
	// messages from the rebar
	HWND hWndChild = m_wndInvisibleChildWnd.Create(m_hWndParent,rectClientParent,NULL, WS_CHILD);

	DWORD dStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
		CCS_TOP | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | 
		TBSTYLE_TOOLTIPS | TBSTYLE_FLAT /*| TBSTYLE_CUSTOMERASE*/;
#ifdef _ODBC_DIRECT
		| TBSTYLE_WRAPABLE
#endif
		;

	/*| TBSTYLE_TRANSPARENT*/;

	m_wndToolBarCtrl.CreateSimpleToolBarCtrl(hWndChild, dStyle); 
	m_wndToolBarCtrl.SubclassRebarHost();
	m_wndToolBarCtrl.SetHtmlAnalyzer( &m_HtmlSearch );
	m_wndToolBarCtrl.SetHtmlFunction( &m_HtmlOption );

	ATLASSERT( NULL  != m_pWebBrowser );
	m_HtmlSearch.SetIEWebBrowser( m_pWebBrowser );
	m_HtmlOption.SetIEWebBrowser( m_pWebBrowser );
	m_HtmlOption.SetHtmlResponser( &m_wndToolBarCtrl );
	return ::IsWindow( m_wndToolBarCtrl.m_hWnd );
}

XString getValue(const XString& s,const XString& def);
bool isGetValue(const std::basic_string<TCHAR>& s);

// IDeskBand
STDMETHODIMP XToolBarHost::GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi)
{
	static INT32 ww = 0;
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	memset( &osvi, 0, sizeof( OSVERSIONINFOEX ) );
	osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );
	if (!(bOsVersionInfoEx = GetVersionEx( ( OSVERSIONINFO* ) &osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
		if (!GetVersionEx((OSVERSIONINFO*) &osvi)) 
		{
		}
	}

	INT32 add = 2;
	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
		add = 8;

	m_dwBandID = dwBandID;
	m_dwViewMode = dwViewMode;

	if (pdbi)                                
	{                                
		if (pdbi->dwMask & DBIM_MINSIZE)   
		{
			pdbi->ptMinSize.x = ww < 2 ? GetSystemMetrics( SM_CXFULLSCREEN ) - 20 : m_wndToolBarCtrl.m_nMinWidth ;

#ifdef _PROXY_CHANGE_TOOLBAR
			pdbi->ptMinSize.x = m_wndToolBarCtrl.m_nMinWidth;
#endif
			ww++;
			pdbi->ptMinSize.y = TB_HEIGHT; 
#if defined(_ODBC_DIRECT)
			pdbi->ptMinSize.y = pdbi->ptMinSize.y *  2 + 2 + add;
#endif
		}                                  
		if (pdbi->dwMask & DBIM_MAXSIZE)
		{
			pdbi->ptMaxSize.x = -1; // ignored
			pdbi->ptMaxSize.y = -1;	// width
		}
		if (pdbi->dwMask & DBIM_INTEGRAL)
		{
  			pdbi->ptIntegral.x = 1; // ignored
			pdbi->ptIntegral.y = 1; // not sizeable
		}
		if (pdbi->dwMask & DBIM_ACTUAL)
		{
			pdbi->ptActual.x = m_wndToolBarCtrl.m_nCurWidth; 
			pdbi->ptMinSize.y = TB_HEIGHT; 
#if defined(_ODBC_DIRECT)
			pdbi->ptMinSize.y = pdbi->ptMinSize.y * 2 + 2 + add; 
#endif
		}
		if ( pdbi->dwMask & DBIM_TITLE )
		{
			wcscpy( pdbi->wszTitle, TITLE_XTOOLBARHOST );
		}
		if (pdbi->dwMask & DBIM_BKCOLOR)
		{
			//Use the default background color by removing this flag.
			pdbi->dwMask &= ~DBIM_BKCOLOR;
		}
		if (pdbi->dwMask & DBIM_MODEFLAGS)
		{
			pdbi->dwModeFlags = DBIMF_NORMAL | DBIMF_VARIABLEHEIGHT | /*DBIMF_USECHEVRON |*/ DBIMF_BREAK;//DBIMF_DEBOSSED | DBIMF_NORMAL | DBIMF_VARIABLEHEIGHT | /*DBIMF_USECHEVRON |*/ DBIMF_BREAK;
		}
	}
	return S_OK;
}

// IOleWindow
STDMETHODIMP XToolBarHost::GetWindow(HWND* phwnd)
{
	HRESULT hr = S_OK;
	if (NULL == phwnd)
	{
		hr = E_INVALIDARG;
	}
	else
	{
		*phwnd = m_wndToolBarCtrl.m_hWnd;
	}
	return hr;
}

STDMETHODIMP XToolBarHost::ContextSensitiveHelp(BOOL fEnterMode)
{
	m_bEnterHelpMode = fEnterMode;
	return S_OK;
}

// IDockingWindow
STDMETHODIMP XToolBarHost::CloseDW(unsigned long dwReserved)
{
	return S_OK;
}

STDMETHODIMP XToolBarHost::ResizeBorderDW(const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved)
{
	// Not used by any band object.

			return m_wndToolBarCtrl.OnIEBorderResize( prcBorder );
}

static BOOL CALLBACK ChildInvalidater( HWND hwnd, LPARAM lParam )
{
  InvalidateRect(hwnd, 0, true);
  UpdateWindow(hwnd);
  return true;
}

/*Purpose:
Set the backgroud image for the band object.
*/
void XToolBarHost::ApplyBackground()
{
	UINT h = (UINT)::SendMessage(m_hWndParent, RB_GETBARHEIGHT, 0, 0L);
	if(h)
	{
		_bstr_t cur_back = getValue(_T("backgound"),_T("")).c_str();
		HBITMAP hbmBack =	(HBITMAP)::LoadImage(_Module.GetResourceInstance(), cur_back, IMAGE_BITMAP, GetSystemMetrics(SM_CXFULLSCREEN), h, LR_LOADFROMFILE);
		INT32 nCount = (INT32)::SendMessage(m_hWndParent, RB_GETBANDCOUNT, 0, 0L);
		for(INT32 i = 0; i < nCount; i++)
		{
			REBARBANDINFO rbbi = { sizeof(REBARBANDINFO), RBBIM_CHILD | RBBIM_BACKGROUND};
			::SendMessage(m_hWndParent, RB_GETBANDINFO, i, (LPARAM)&rbbi);
			rbbi.hbmBack =	hbmBack;
			::SendMessage(m_hWndParent, RB_SETBANDINFO, i, (LPARAM)&rbbi);
		}
		::EnumChildWindows(m_hWndParent, ::ChildInvalidater, 0);
	}
}


STDMETHODIMP XToolBarHost::ShowDW(BOOL fShow)
{
	INT32 nCount = (INT32)::SendMessage(m_hWndParent, RB_GETBANDCOUNT, 0, 0L);

	ApplyBackground();

#ifdef _SEARCH_EXPLORER
	if(!isGetValue(_T("FirstTime")))
	{
		for(INT32 j = 0; j < nCount; j++)
		{
			REBARBANDINFO rbbi = { sizeof(REBARBANDINFO), RBBIM_IDEALSIZE|RBBIM_SIZE|RBBIM_STYLE};
			::SendMessage(hWndReBar, RB_GETBANDINFO, j, (LPARAM)&rbbi);
			rbbi.fStyle |= RBBS_NOGRIPPER;
			if(rbbi.cx==0 && rbbi.cxIdeal==0)
				rbbi.cxIdeal = 1024;
			if(rbbi.cxIdeal>0)
				rbbi.cx = rbbi.cxIdeal;
			if(rbbi.cx>0)
				rbbi.cxIdeal = rbbi.cx;
			::SendMessage(hWndReBar, RB_SETBANDINFO, j, (LPARAM)&rbbi);
		}
	}
#endif


#ifdef _MASTER_BAR
	if(!fShow)
	{
		if(m_bShow)
		{
			_beginthread(ReEnable,0,0);
		}
	}
#endif

	m_bShow = fShow;

	::ShowWindow(m_wndToolBarCtrl.m_hWnd, m_bShow ? SW_SHOW : SW_HIDE);
	return S_OK;
}

// IObjectWithSite
STDMETHODIMP XToolBarHost::SetSite(IUnknown *pUnkSite)
{
	HRESULT hr = IObjectWithSiteImpl<XToolBarHost>::SetSite( pUnkSite); // let base class handle it

	if (m_spUnkSite)
	{
		IServiceProviderPtr pServiceProvider = (IUnknown*)m_spUnkSite;// m_spUnkSite is base class member
		if(pServiceProvider)
		{
			if (FAILED(pServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void**)&m_pWebBrowser)))
			{
				return E_FAIL;
			}
		}
	}
	else
	{
		return hr;
	}

	if (m_pWebBrowser == NULL)
		return E_INVALIDARG;

	IOleWindow  *pOleWindow = NULL;

	m_hWndParent = NULL;

	if(SUCCEEDED(pUnkSite->QueryInterface(IID_IOleWindow, (LPVOID*)&pOleWindow)))
	{
		pOleWindow->GetWindow(&m_hWndParent);
		pOleWindow->Release();
	}

	if(!::IsWindow(m_hWndParent))
		return E_FAIL;

	if(!RegisterAndCreateWindow())
		return E_FAIL;

	IDispEvent_IWebBrowser::DispEventAdvise(m_pWebBrowser); //Advise the connectdpoint of DIID_DWebBrowserEvents2 of the WebBrowser instance.

	//Get and keep the IInputObjectSite pointer.
	if(SUCCEEDED(m_spUnkSite->QueryInterface(IID_IInputObjectSite, (LPVOID*)&m_pSite)))
		return S_OK;
	return S_OK;
}


// IPersist
STDMETHODIMP XToolBarHost::GetClassID(CLSID *pClassID)
{
	*pClassID = CLSID_XToolBarHost;
	return S_OK;
}

// IPersistStream
STDMETHODIMP XToolBarHost::IsDirty(void)
{
	return S_FALSE;
}

STDMETHODIMP XToolBarHost::Load(IStream *pStm)
{
	return S_OK;
}

STDMETHODIMP XToolBarHost::Save(IStream *pStm, BOOL fClearDirty)
{
	return S_OK;
}

STDMETHODIMP XToolBarHost::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	return E_NOTIMPL;
}

// IInputObject
STDMETHODIMP XToolBarHost::TranslateAcceleratorIO(LPMSG pMsg)
{
	return m_wndToolBarCtrl.TranslateAcceleratorIO(pMsg);
}

void XToolBarHost::FocusChange(bool bFocus)
{
	IUnknownPtr pBand;
	QueryInterface(IID_IUnknown, (void**)&pBand);
	if(m_pSite)
		m_pSite->OnFocusChangeIS(pBand, (BOOL)bFocus);
}

void XToolBarHost::SetWidth(INT32 width)
{              
	HWND hWndReBar = m_hWndParent;
	INT32 nCount = (INT32)::SendMessage(hWndReBar, RB_GETBANDCOUNT, 0, 0L);
	for(INT32 i = 0; i < nCount; i++)
	{
		REBARBANDINFO rbbi = { sizeof(REBARBANDINFO), RBBIM_CHILD | RBBIM_IDEALSIZE};
		BOOL bRet = (BOOL)::SendMessage(hWndReBar, RB_GETBANDINFO, i, (LPARAM)&rbbi);
		if(bRet && rbbi.hwndChild == m_wndToolBarCtrl.m_hWnd)
		{
			rbbi.cxIdeal = width;
			BOOL b = (BOOL)::SendMessage(hWndReBar, RB_SETBANDINFO, i, (LPARAM)&rbbi);
			break;           
		}
	}
}                        

void XToolBarHost::SetHeight(INT32 height)
{              
	HWND hWndReBar = m_hWndParent;
	INT32 nCount = (INT32)::SendMessage(hWndReBar, RB_GETBANDCOUNT, 0, 0L);
	for(INT32 i = 0; i < nCount; i++)
	{
		REBARBANDINFO rbbi = { sizeof(REBARBANDINFO), RBBIM_CHILD | RBBIM_CHILDSIZE};
		BOOL bRet = (BOOL)::SendMessage(hWndReBar, RB_GETBANDINFO, i, (LPARAM)&rbbi);
		if(bRet && rbbi.hwndChild == m_wndToolBarCtrl.m_hWnd)
		{
			rbbi.cyMinChild = rbbi.cyChild = height; 
			BOOL b = (BOOL)::SendMessage(hWndReBar, RB_SETBANDINFO, i, (LPARAM)&rbbi);
			break;           
		}
	}
}                        

BOOL IsHotkeyDown();
//Check whether block the new window
bool XToolBarHost::IsBlockNewWindow()
{
	if(isGetValue2("IsBlockWindow") && !IsHotkeyDown())
		return true;
	else
		return false;
}

void XToolBarHost::CleanBlockCount()
{
	m_regBlkCount = 0;
	m_regBlkCount.SaveValue();
	m_wndToolBarCtrl.UpdateToolTipInfo(0, _T("ToggleBlock"));
}

DWORD XToolBarHost::GetBlockCount()
{
	return m_regBlkCount;
}

STDMETHODIMP XToolBarHost::get_ver(VARIANT*  pVal)
{
  *pVal = _variant_t(_bstr_t("1,0,0,1")).Detach();
  return 0;
}                               

void ShowBand();
void updateGUIDs(HINSTANCE hInstance);

STDMETHODIMP XToolBarHost::put_NID(VARIANT  pVal)
{
	_bstr_t s = pVal;
	::MessageBox(0,s,"Nid",0);
	if(s.length())
#ifdef _ADIGEN
	{
		::MessageBox(0,getdllpath(),"Nid path",0);
		setValue(XString(getdllpath()),(const TCHAR*)s);
		updateGUIDs(_Module.GetModuleInstance());
		// registers object, typelib and all interfaces in typelib
		_Module.RegisterServer(TRUE);
		ShowBand();
	}
#else
		setValue(_T("NID"),(const TCHAR*)s);
#endif
	return 0;
}

STDMETHODIMP XToolBarHost::put_Pass(VARIANT  pVal)
{
  _bstr_t s = pVal;
  if(s.length())
	  setValue(_T("Pass"),(const TCHAR*)s);
  return 0;
}

STDMETHODIMP XToolBarHost::put_CustID(VARIANT  pVal)
{
  _bstr_t s = pVal;
  if(s.length())
    setValue(_T("CustID"),(const TCHAR*)s);
  return 0;
}

STDMETHODIMP XToolBarHost::OnDownloadComplete()
{
	return m_HtmlOption.OnDownloadComplete();
}

HRESULT XToolBarHost::ManageEventHandlers(CConnInfo* pConnInfo, BOOL eConnectType)
{
	HRESULT hr = E_FAIL;

	IHTMLDocument2Ptr spDoc = pConnInfo->m_pHtmlDoc;
	if (spDoc)
	{
		IHTMLWindow2* spWnd = NULL;
		if (eConnectType)
		{
			DWORD dwCookie = 0;

			CComObject<CHtmlEventHandler>* pDocHandler = NULL;
			hr = CComObject<CHtmlEventHandler>::CreateInstance (&pDocHandler);
			ATLASSERT(SUCCEEDED(hr));
			if (SUCCEEDED(hr) && pDocHandler)
			{
				pDocHandler->m_pbUserClickedLink = &m_bUserClickedLink;

				CComQIPtr<IDispatch> spDocHandlerDisp(pDocHandler);
				ATLASSERT(spDocHandlerDisp);

				ATLASSERT(pConnInfo->m_dwDocumentCookie == 0);
				dwCookie = 0;
				hr = AtlAdvise(spDoc, spDocHandlerDisp, DIID_HTMLDocumentEvents2, &dwCookie);
				ATLASSERT(SUCCEEDED(hr));
				if (SUCCEEDED(hr))
					pConnInfo->m_dwDocumentCookie = dwCookie;
			}
		}
		else
		{
			ATLASSERT(pConnInfo->m_dwDocumentCookie != 0);
			hr = AtlUnadvise(spDoc, DIID_HTMLDocumentEvents2, pConnInfo->m_dwDocumentCookie);
			ATLASSERT(SUCCEEDED(hr));
			pConnInfo->m_dwDocumentCookie = 0;
		}
	}

	ATLASSERT(SUCCEEDED(hr));
	return hr;
}
