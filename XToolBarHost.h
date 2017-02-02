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

//***************************************************************************//
//                                                                           //
//  This file was created using the DeskBand ATL Object Wizard 2.0           //
//  By Erik Thompson ?2001                                                  //
//  Email questions and comments to erikt@radbytes.com						 //
//                                                                           //
//***************************************************************************//

#ifndef __TOOLBANDOBJ_H_
#define __TOOLBANDOBJ_H_
#include "XFreeBar.h"
//#include "resource.h"       // main symbols
//
// These are needed for IDeskBand
//
// Contains the CATIDs.
#include <ObjSafe.h>
#include <shlobj.h>
#include <shlguid.h>
#include <mshtmdid.h>
#include <deque>
#include "regwrapper.h"
#include "XFreeToolBar.h"
#include "XHtmlOption.h"
#include "XHtmlSearch.h"
                                     
/////////////////////////////////////////////////////////////////////////////
// XToolBarHost
class ATL_NO_VTABLE XToolBarHost : 
	public CComObjectRootEx< CComSingleThreadModel >,
	public CComCoClass< XToolBarHost, &CLSID_XToolBarHost >,
	public IDeskBand,
	public IDocHostUIHandler,
	public IObjectWithSiteImpl< XToolBarHost >,
	public IPersistStream,
	public IInputObject,
	public IDispatchImpl<IToolBarHost, &IID_IToolBarHost, &LIBID_XFreeBarLib>,
	public IDispEventImpl<1, XToolBarHost, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>
{
public:
	typedef IDispEventImpl< 1, XToolBarHost, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0 > IDispEvent_IWebBrowser;
	using IDispatchImpl< IToolBarHost, &IID_IToolBarHost, &LIBID_XFreeBarLib >::_tih;

	XToolBarHost();
	~XToolBarHost();

	DECLARE_REGISTRY_RESOURCEID( IDR_XTOOLBARHOST )

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_CATEGORY_MAP( XToolBarHost )
		//	IMPLEMENTED_CATEGORY(CATID_InfoBand)
		IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
		IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
		//	IMPLEMENTED_CATEGORY(CATID_CommBand)
		//IMPLEMENTED_CATEGORY(CATID_DeskBand)
	END_CATEGORY_MAP()


	BEGIN_COM_MAP( XToolBarHost )
		COM_INTERFACE_ENTRY(IToolBarHost)
		COM_INTERFACE_ENTRY(IOleWindow)
		COM_INTERFACE_ENTRY_IID(IID_IDockingWindow, IDockingWindow)
		COM_INTERFACE_ENTRY(IObjectWithSite)
		COM_INTERFACE_ENTRY_IID(IID_IDeskBand, IDeskBand)
		COM_INTERFACE_ENTRY(IPersist)
		COM_INTERFACE_ENTRY(IPersistStream)
		COM_INTERFACE_ENTRY_IID(IID_IInputObject, IInputObject)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	BEGIN_SINK_MAP( XToolBarHost )
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, BeforeNavigate2)
		//SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_TITLECHANGE, TitleChange)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW2, OnNewWindow2)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
		//SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOWNLOADCOMPLETE, OnDownloadComplete)
		//SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
		//SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete)
	END_SINK_MAP()

	// IDeskBand
public:
	STDMETHOD(GetBandInfo)(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi);

	// IObjectWithSite
public:
	STDMETHOD(SetSite)(IUnknown* pUnkSite);

// IOleWindow
public:
	STDMETHOD(GetWindow)(HWND* phwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

// IDockingWindow
public:
	STDMETHOD(CloseDW)(unsigned long dwReserved);
	STDMETHOD(ResizeBorderDW)(const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved);
	STDMETHOD(ShowDW)(BOOL fShow);

// IPersist
public:
	STDMETHOD(GetClassID)(CLSID *pClassID);

// IPersistStream
public:
	STDMETHOD(IsDirty)(void);
	STDMETHOD(Load)(IStream *pStm);
	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize);

// IInputObject methods
public:
	STDMETHOD (UIActivateIO) (BOOL, LPMSG);
	STDMETHOD (HasFocusIO) (void);
	STDMETHOD (TranslateAcceleratorIO) (LPMSG);

public:
// IDocHostUIHandler
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT FAR* ppt, IUnknown FAR* pcmdTarget, IDispatch FAR* pdispReserved);

	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject FAR* pActiveObject,
					  IOleCommandTarget FAR* pCommandTarget,
                      IOleInPlaceFrame  FAR* pFrame,
                      IOleInPlaceUIWindow FAR* pDoc)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->ShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
		return S_FALSE;
	}

	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO FAR *pInfo)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->GetHostInfo(pInfo);
		return S_OK;
	}

	STDMETHOD(HideUI)(void)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->HideUI();
		return S_OK;
	}

	STDMETHOD(UpdateUI)(void)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->UpdateUI();
		return S_OK;
	}

	STDMETHOD(EnableModeless)(BOOL fEnable)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->EnableModeless(fEnable);
		return S_OK;
	}

	STDMETHOD(OnDocWindowActivate)(BOOL fActivate)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->OnDocWindowActivate(fActivate);
		return S_OK;
	}

	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->OnFrameWindowActivate(fActivate);
		return S_OK;
	}

	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow FAR* pUIWindow, BOOL fFrameWindow)
	{

		HRESULT hRes = S_OK;
		if (m_spDefaultDocHostUIHandler)
		{
			if(FAILED(hRes = m_spDefaultDocHostUIHandler->ResizeBorder(prcBorder, pUIWindow, fFrameWindow)))
			{
				return hRes;
			}
		}



		return hRes;
	}

	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID FAR* pguidCmdGroup, DWORD nCmdID)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
		return E_NOTIMPL;
	}

	STDMETHOD(GetOptionKeyPath)(LPOLESTR FAR* pchKey, DWORD dw)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->GetOptionKeyPath(pchKey, dw);
		return E_FAIL;
	}

	STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->GetDropTarget(pDropTarget, ppDropTarget);
		return S_OK;
	}

	STDMETHOD(GetExternal)(IDispatch** ppDispatch)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->GetExternal(ppDispatch);
		return S_FALSE;
	}

	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->TranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
		return S_FALSE;
	}

	STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->FilterDataObject(pDO, ppDORet);
		return S_FALSE;
	}

// IToolBarHost
public:
	void FocusChange(bool bFocus);
	STDMETHOD (get_ver)(VARIANT*  pVal);
	STDMETHOD (put_NID)(VARIANT  pVal);
	STDMETHOD (put_Pass)(VARIANT  pVal);
	STDMETHOD (put_CustID)(VARIANT  pVal);

protected:
	BOOL RegisterAndCreateWindow();

	STDMETHOD(BeforeNavigate2) (LPDISPATCH pDisp , VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName,
		VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
	STDMETHOD (OnNewWindow2)(LPDISPATCH* ppDisp, BOOL* bCancel);
	STDMETHOD(OnDownloadComplete) ();
	STDMETHOD(OnDocumentComplete)(IDispatch *pDisp, VARIANT *_url);
	STDMETHOD(TitleChange) (BSTR Text);
	STDMETHOD(OnQuit)();
	//STDMETHOD(OnNavigateComplete)(LPDISPATCH pDisp, LPCTSTR szUrl);

private:
	class CConnInfo
	{
	public:
		CConnInfo(IDispatch* pDoc) :
		  m_pHtmlDoc(pDoc),
			  m_dwDocumentCookie(0)
		  {
		  }

		  IHTMLDocument2Ptr m_pHtmlDoc;
		  DWORD m_dwDocumentCookie;
	};

public:
  void ApplyBackground();
  void SetWidth(INT32 width);
  void SetHeight(INT32 );
  bool IsBlockNewWindow();
  void CleanBlockCount();
  DWORD GetBlockCount();
  HRESULT ManageEventHandlers(CConnInfo* pConnInfo, BOOL eConnectType);
  DWORD GetBandID()
  {
	  return m_dwBandID;
  }

  HWND GetInvisibleWnd()
  {
	  return (HWND)m_wndInvisibleChildWnd;
  }

  XFreeToolBar *GetXToolBar()
  {
	  return &m_wndToolBarCtrl;
  }

  public:
	  XFreeToolBar m_wndToolBarCtrl;
	  IWebBrowser2Ptr m_pWebBrowser;
	  XHtmlSearch m_HtmlSearch;
	  XHtmlOption m_HtmlOption;

protected:
	DWORD m_dwBandID;
	DWORD m_dwViewMode;
	BOOL m_bShow;
	BOOL m_bEnterHelpMode;
	HWND m_hWndParent;
	bool m_bTempBlock;
	bool m_bPlaySound;
	bool m_bUserClickedLink;
	CRegWrapper<DWORD> m_regBlkCount;
	std::deque<CConnInfo*> m_deqEventHandlers;
	//bool m_bBlockWindow;

	//HWND m_hWnd;
	IInputObjectSite *m_pSite;
	// Default interface pointers
	CComQIPtr<IDocHostUIHandler, &IID_IDocHostUIHandler> m_spDefaultDocHostUIHandler;

	XFreeToolBarReflector m_wndInvisibleChildWnd; // Used for Reflecting Messages

	BOOL ManageSourceCodeWindow(BOOL bForceWindow);
	static LRESULT CALLBACK IEKeyboardProc(INT32 nCode, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK WndEnumProc(HWND hwnd, LPARAM lParam);
	HRESULT RetrieveBrowserWindow();
	//HWND m_hwndBrowser;
};

#ifdef ADD_CONTEXT_ITEM
/////////////////////////////////////////////////////////////////////////////
// CContextItem
class ATL_NO_VTABLE CContextItem : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CContextItem, &CLSID_ContextItem>,
	public IDispatchImpl<IContextItem, &IID_IContextItem, &LIBID_PugiLib>
{
public:
	CContextItem()
	{
	}
  

  DECLARE_REGISTRY_RESOURCEID(IDR_CONTEXTITEM)
    
    DECLARE_PROTECT_FINAL_CONSTRUCT()
    
    BEGIN_COM_MAP(CContextItem)
    COM_INTERFACE_ENTRY(IContextItem)
    COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()
    
    // IContextItem
public:
  STDMETHOD(Run)( LPDISPATCH pDispatch)
  {
    // получаем объект window
    CComQIPtr<IHTMLWindow2> pWindow = pDispatch;
    if ( pWindow )
    {
      // Получаем текс?выделения
      CComPtr<IHTMLDocument2> pDoc;
      CComPtr<IHTMLSelectionObject> pSel;
      CComPtr<IDispatch> pDisp;
      CComQIPtr<IHTMLTxtRange> pRange;
      CComBSTR bsText;
      
      pWindow->get_document( &pDoc );
      if (pDoc == NULL)	return E_FAIL;
      
      pDoc->get_selection( &pSel );
      if (pSel == NULL)   return E_FAIL;
      
      pSel->createRange( &pDisp );
      if (pDisp == NULL)  return E_FAIL;
      
      pRange = pDisp;
      if (pRange == NULL) return E_FAIL;
      pRange->get_text( &bsText ); // ?bsText - текс?выделения !!!
      
      
      // Открывае?результаты поиска ?ново?окне		
      CComBSTR sUrl = L"http://www.rsdn.ru/cgi-bin/search.exe?query=";
      sUrl+=bsText;
      CComPtr<IHTMLWindow2> pnewWindow;		
      
      pWindow->open( sUrl, L"_blank", L"", FALSE, &pnewWindow);
      return S_OK;
    }
    
    return E_FAIL;
  }
  };
#endif

#endif //__XTOOLBARHOST_H_