#include "StdAfx.h"
#include "XFreeToolBar.h"
#include "ConfigDlg.h"
#include "XHtmlEventObject.h"
#include "XToolBarHost.h"
#include <set>
#include "CmnUtil.h"
#include "regoper.h"
#include  <atlframe.h>
#include  <ConfigDlg.h>
#include "XToolBarDraw.h"
#include "XWnd.h"

using namespace XML;

//UINT32 XBTNINFO::uBtnCount = 0;

extern XString gAppendCommand;
extern XString gAppendCommandDisplay;

_bstr_t gurl;
XString g_last_url;
static WNDPROC g_fSuperIEFrameProc = NULL;

// format of backgound for button icons
#if defined(_LOOKJET) || defined(_SEARCHHIT) || defined(_ADRRESSS_BAR_REDIRECT)
const INT32 maskClr = CLR_NONE;
#elif defined(_ODBC_DIRECT) || defined(_MASTER_BAR)
const INT32 maskClr = 0xff00ff;
#else
const INT32 maskClr = CLR_DEFAULT;
#endif

#define CHEVRON_BUTTON_WIDTH 15
#define CHEVRON_BUTTON_ID ( INT32 )( WM_USER + MAX_BUTTON_COUNT )
#define FILLED_BUTTON_ID ( INT32 )( WM_USER + MAX_BUTTON_COUNT - 1)

// here we store temp data that we generate during showing menus
vectorDel< _XFreeMenuItemData* >   _XFreeMenuItemData::MenuItems;

// some toolbar specific includes
#if defined (_ODBC_DIRECT) || defined(_MASTER_BAR)
#include "Toolband7\CPage.cpp"
#endif
#ifdef _PROXY_CHANGE_TOOLBAR
#include "Toolband26\Proxy.cpp"
#endif
#ifdef _ODBC_DIRECT
#include "PostInc.cpp"
#endif

#ifdef _ADIGEN 
static _bstr_t sProfileName;
#endif

#define SAFE_RELEASE_BUF( bufptr ) if(NULL != bufptr) { free(bufptr); bufptr = NULL; }
// "main" crop
XFreeToolBar::XFreeToolBar() :
									m_bSplitFocused( FALSE ),
									m_bLBtnDown( FALSE ),
									m_pToolBarHost( NULL ),
									m_nLastPresedButton( -1 ),
									m_hImageDll( NULL ),
									m_hTickerThread( 0 ),
									m_nCurWidth( 0 ),
									m_nMinWidth( 0 ),
									m_pChevDrop( NULL ),
									m_nChevBtnIndex( -1 ),
									//m_pXBtnInfos( NULL ),
									m_bToolbarInited( FALSE ),
									m_bLButtonDown( FALSE ),
									m_uBtnCount( 0 ),
									m_nDragingImage( 0 ),
									m_nDragingButton( 0 ),
									m_bHtmlRespInited( FALSE ), 
									m_RebarContainer( NULL, this, 1 ),
									m_WebBrowserContainer( NULL, this, 2 )

{
	InitXBtnInfos();
}


INT32 XFreeToolBar::InitXBtnInfos()
{
	m_pXBtnInfos = ( XBTNINFO* )malloc( XFREEBAR_MAX_BTN_COUNT * sizeof( XBTNINFO ) );
	if( NULL == m_pXBtnInfos )
	{
		return XERROR_NO_ENOUGH_MEM;
	}
	memset( m_pXBtnInfos, 0, sizeof( XBTNINFO ) * XFREEBAR_MAX_BTN_COUNT );
	m_pBtnDatas = ( TBBUTTON* )malloc( XFREEBAR_MAX_BTN_COUNT * sizeof( TBBUTTON ) );
	if( NULL == m_pBtnDatas )
	{
		return XERROR_NO_ENOUGH_MEM;
	}
	memset( m_pBtnDatas, 0, sizeof( TBBUTTON ) * XFREEBAR_MAX_BTN_COUNT );
	for( INT32 i = 0; i < XFREEBAR_MAX_BTN_COUNT; i ++ )
	{
		m_pXBtnInfos[i].pBtnData = &m_pBtnDatas[i];
	}
	return XSUCCESS;
}

INT32 XFreeToolBar::DestroyXBtnInfos()
{
	if( NULL != m_pXBtnInfos )
	{
		free( m_pXBtnInfos );
		m_pXBtnInfos = NULL;
	}

	if( NULL != m_pBtnDatas )
	{
		free( m_pBtnDatas );
		m_pBtnDatas = NULL;
	}
	return XSUCCESS;
}

// clear all resources 
XFreeToolBar::~XFreeToolBar()
{
	if( NULL != m_hWnd )  
		DestroyWindow();

	if( NULL != m_hImageDll )
		FreeLibrary(m_hImageDll);

	m_CustomImage.clear();
	if(m_pChevDrop != NULL)
	{
		m_pChevDrop->DestroyWindow();
		m_pChevDrop->m_hWnd = NULL;
		delete m_pChevDrop;
	}

	DestroyXBtnInfos();
}

// here we store temp data that we generate during showing menus
//vectorDel<XFreeToolBar::_XFreeMenuItemData*>   XFreeToolBar::_XFreeMenuItemData::MenuItems;

//Show/Hide and Enable/Disable the hilight button, the input param can be the keyword number.
void XFreeToolBar::EnableHighlight(BOOL bEnable)
{
	INT32 nHilightIndex = g_pXConfig->MapBtnConfIndexFromCmd( _T("Highlight") );
	if( nHilightIndex >= 0 )
	{
		EnableButton( MapBtnIDFromIndex( nHilightIndex ), bEnable );
		CheckButton( MapBtnIDFromIndex( nHilightIndex ), FALSE );
		//TBInfoSetter(this, nHilightIndex).style( BTNS_DROPDOWN ); //BTNS_CHECK );
	}
}

#ifdef _ODBC_DIRECT
#include "EnumResNameProc.inl"
#endif

//Calculate the width of this toolbar.
VOID XFreeToolBar::CalcXFreeBarWidth()
{
	INT32 nBtnCount;
	RECT rc;

	nBtnCount = GetButtonCount();
	m_nCurWidth = m_nMinWidth = 0;
	for( INT32 i = 0; i < nBtnCount; i++ )
	{
		//TBBUTTONINFO tbi;
		//tbi.cbSize = sizeof TBBUTTONINFO;
		//tbi.dwMask = TBIF_STATE | TBIF_SIZE;
		//INT32 nRet = GetButtonInfo(MapBtnIDFromIndex(i),&tbi);
		//if(nRet == -1)
		//{
		//	ATLTRACE(" Get button %d info on CalcXFreeBarWidth failed\n", i);
		//}
		//if(!(tbi.fsState & TBSTATE_HIDDEN))
		//{
		
		GetItemRect(i, &rc);
		m_nCurWidth += rc.right - rc.left;
		if(i < g_pXConfig->m_BtnConfs.size() && g_pXConfig->m_BtnConfs[i]->isShownAlways())
			m_nMinWidth = m_nCurWidth;
		//}
	}
	m_pToolBarHost->SetWidth( m_nCurWidth );
}

INT32 XFreeToolBar::SetBtnsWidth( DWORD dwMode )
{
	INT32 nRet;
	INT32 nBtnCount;
	XBTNINFO *pXBtnInfo;
	TBBUTTON TBBtn;
	RECT rc;
	
	nRet = XSUCCESS;
	nBtnCount = GetButtonCount();
	for( INT32 i = 0; i < nBtnCount; i++ )
	{
		ATLASSERT( FALSE != GetButton( i, &TBBtn ) );
		ATLASSERT( NULL != TBBtn.dwData );
		pXBtnInfo = ( XBTNINFO* )TBBtn.dwData;

		if( ( pXBtnInfo->dwStyle & FIXED_WIDTH_BTN && ( dwMode == MODE_ALL_WIDTH || dwMode == MODE_FIXED_WIDTH ) )
			|| ( !( pXBtnInfo->dwStyle & FIXED_WIDTH_BTN ) && ( dwMode == MODE_ALL_WIDTH || dwMode == MODE_FREE_WIDTH ) ) )
		{
			pXBtnInfo->dwRealWidth = 0;
			if( ( TBBtn.fsState & TBSTATE_HIDDEN ) != TBSTATE_HIDDEN )
			{
				GetItemRect( i, &rc );
				pXBtnInfo->dwRealWidth = rc.right - rc.left;
			}
		}
	}
	return nRet;
}

INT32 XFreeToolBar::SetBtnsWidth( INT32 uStartIndex, INT32 uEndIndex )
{
	ATLASSERT( uStartIndex >= 0);
	ATLASSERT( uEndIndex <= GetButtonCount() );

	INT32 nRet;
	TBBUTTON TBBtn;
	XBTNINFO *pXBtnInfo;
	RECT rc;

	for( INT32 i = uStartIndex; i < uEndIndex; i++ )
	{
		nRet = GetButton( i, &TBBtn );
		if( nRet < 0 )
		{
			ATLASSERT( FALSE );
		}

		ATLASSERT( NULL != TBBtn.dwData );

		if( ( TBBtn.fsState & TBSTATE_HIDDEN ) != TBSTATE_HIDDEN ) 
		{
			pXBtnInfo = ( XBTNINFO* )TBBtn.dwData;
			GetItemRect(i, &rc);
			pXBtnInfo->dwRealWidth = rc.right - rc.left;
		}
	}
	return 0;
}

INT32 XFreeToolBar::SetBtnsWidthByIndexes( UINT32 *pBtnIndexes, UINT32 uBtnCount )
{
	ATLASSERT( pBtnIndexes != 0 );
	ATLASSERT( uBtnCount <= ( UINT )GetButtonCount() );

	INT32 nRet;
	TBBUTTON TBBtn;
	XBTNINFO *pXBtnInfo;
	RECT rc;

	for( INT32 i = 0; i < ( INT32 )uBtnCount; i++ )
	{
		nRet = GetButton( i, &TBBtn );
		if( nRet < 0 )
		{
			ATLASSERT( FALSE );
		}

		ATLASSERT( NULL != TBBtn.dwData );

		if( ( TBBtn.fsState & TBSTATE_HIDDEN ) != TBSTATE_HIDDEN )
		{
			pXBtnInfo = ( XBTNINFO* )TBBtn.dwData;
			GetItemRect( pBtnIndexes[i], &rc );
			pXBtnInfo->dwRealWidth = rc.right - rc.left;
		}
	}
	return 0;
}

// get current HTML document helper
IHTMLDocument2Ptr XFreeToolBar::GetHtmlDocument()
{
  LPDISPATCH pDispatch = 0;
  if(this->m_pToolBarHost)
    m_pToolBarHost->m_pWebBrowser->get_Document(&pDispatch);
  IHTMLDocument2Ptr pHtmlDoc2 = pDispatch;
  if(pDispatch)
    pDispatch->Release();
  return pHtmlDoc2;
}

#if defined(BLUE_ON_OVER_TEXT) || defined(USE_TICKER)
DWORD CXFreeToolBarReflector::OnItemPrePaint(INT32 , LPNMCUSTOMDRAW pNmHdr)
{
	LPNMTBCUSTOMDRAW lpTBCustomDraw = (LPNMTBCUSTOMDRAW)pNmHdr;
	bool bDisabled = ((lpTBCustomDraw->nmcd.uItemState & CDIS_DISABLED) == CDIS_DISABLED);
	if(!bDisabled)
  {
    if(g_pXConfig) // on quit bug fix
      if(lpTBCustomDraw->nmcd.dwItemSpec-WM_USER<g_pXConfig->m_BtnConfs.size()) // generated words
        return g_pXConfig->m_BtnConfs[lpTBCustomDraw->nmcd.dwItemSpec-WM_USER]->OnItemPrePaint(&m_pToolBarHost->m_wndToolBarCtrl,lpTBCustomDraw);
  }
  return CDRF_DODEFAULT;
}
#endif

// process tooltip events
LRESULT CXFreeToolBarReflector::OnToolTip( INT32, LPNMHDR lParam, BOOL &bHandled  )
{
	ATLASSERT( NULL != g_pXConfig );
	LPTOOLTIPTEXT lpTooltip;
	bHandled = FALSE;
	lpTooltip = ( LPTOOLTIPTEXT ) lParam;
	lpTooltip->hinst = _Module.GetModuleInstance();

	//UINT32 id = lpttt->hdr.idFrom - WM_USER - TOOLBAR_IMAGE_INDEX_BEGIN;
	//UINT32 id = (UINT32)::SendMessage( lpttt->hdr.hwndFrom, TB_COMMANDTOINDEX, lpttt->hdr.idFrom, 0);
	UINT32 id = lpTooltip->hdr.idFrom;


	if(id < g_pXConfig->m_BtnConfs.size())
		lpTooltip->lpszText = const_cast< TCHAR* >( g_pXConfig->m_BtnConfs[id]->GetHintMsg().c_str());
	else
	{
		lpTooltip->lpszText = const_cast< TCHAR* >( g_pXConfig->m_Settings.m_wordfindHint.c_str());
	}
	bHandled = TRUE;
	return XSUCCESS;
}

#ifdef _SERVAGE

void __cdecl CloseAll(void* lpParam)
{
  CoInitialize(0);
  Sleep(2000);
  CloseAllRuningIE();
  CoUninitialize();
}

void __cdecl XTicker(void* lpParam)
{
  XFreeToolBar* _this = (XFreeToolBar*)lpParam;
  while(true)
  {
    bool old = g_DO_HTML;
    if(!checkConnection("www.microsoft.com",""))
    {
      _this->m_pToolBarHost->SetHeight(1);
      g_DO_HTML = false;
    }
    else
    {
      _this->m_pToolBarHost->SetHeight(TB_HEIGHT);
      g_DO_HTML = true;
    }
    if(old!=g_DO_HTML)
      _this->OnOptionChange();
    Sleep(20*1000); //each 20 seconds
  }
}
#endif
INT32 XFreeToolBar::GetImageId( XString strImg /*Not Hot Image*/ )
{
	if(IsNumberStr(strImg.c_str()))
	{
		if(strImg.size())
		{
			return atoi(strImg.c_str()) + TOOLBAR_IMAGE_INDEX_BEGIN;
		}
		return -2;
	}


	UINT nID = m_CustomImage[ strImg ];
	 if( nID == 0)
		 return -2;
	 return nID;
}

BOOL XFreeToolBar::AddCustomImage(XString strImg, XString strHotImg)
{
	if(IsNumberStr(strImg.c_str()))
		return TRUE;

	UINT uBtnCount = m_NormList.GetImageCount();
	UINT uCurId = uBtnCount;
	m_CustomImage[strImg] = uCurId;
	if(strImg.find(_T(".ico")) != -1 || strImg.find(_T(".ICO")) != -1)
	{
		m_NormList.AddIcon(LoadIcon(strImg));
	}
	else
	{
		m_NormList.Add(CBitmap(LoadImage(strImg)),maskClr );
	}

	XString strTmp;
	if(strHotImg.empty())
	{
		strTmp = strImg;
	}
	else
	{
		strTmp = strHotImg;
	}

	if(strTmp.find(_T(".ico")) != -1 || strTmp.find(_T(".ICO")) != -1)
	{
		m_HotList.AddIcon(LoadIcon(strTmp));
	}
	else
	{
		m_HotList.Add(CBitmap(LoadImage(strTmp)),maskClr );
	}
	return TRUE;
}

INT32 XFreeToolBar::XToolBarUpdate()
{
	INT32 days;
	// check update
#ifdef CHECK_EACH_START
	setValue(_T("gUpdate"),_T("1"));
#else
	if( isGetValue2( _T( "gUpdate" ) ) ) //prevent recursion
	{
		return XERROR_NO_NEED_UPDATE;
	}
	if( !isGetValue( _T( "UpdateAutomatically" ) ) )  // update process enable
	{
		return XERROR_NO_NEED_UPDATE;
	}

	if(  days = atoi( getValue( _T( "Scope" ), "-1" ).c_str() ) == 0 )   // interval is set
	{
		return XERROR_NO_NEED_UPDATE;
	}

	if( FALSE == checkTime( _T( "LastCheckTime" ), days ) ) // time period passe
	{
		return XERROR_NO_NEED_UPDATE;
	}

	// finish start
	XString verFile = _T("version.txt");
	XString path = verFile;
	XString oldCRC = getValue(verFile);
#ifndef DONT_CHECK_VERSION_FILE
	if( FALSE == CheckUpdate( path, true ) || oldCRC == getValue( verFile ) ) // download version and CRC changed
	{
		return XERROR_NO_NEED_UPDATE;
	}

#endif
	setValue( _T( "gUpdate" ), _T( "1" ) );

#ifdef UPDATE_DLL_FROM_EXE
	if( oldCRC != _T( "1" ) ) // not during first installation
	{
#ifndef _MASTER_BAR
		if( ::MessageBox(0,_T("A new version of the " + sProfileName + " IE ToolbarConfig is available.\nDo you want to update your current " + sProfileName + " IE ToolbarConfig ?"),XTOOLBAR_ALERT_TITLE,MB_YESNO)==IDYES)
#endif
		{
			if( URLDownloadToFile ( NULL, serverpath+exeName, getprogpath(e xeName ), 0, 0 ) == S_OK )
			{
				CloseAllRuningIE();
				setValue( _T( "UpdateBegin" ), _T( "1" ) );
				ShellExecute( NULL, NULL, getprogpath(exeName), NULL, NULL, SW_SHOWNORMAL );
				//m_pToolBarHost->m_pWebBrowser->Quit();// quit itself
			}
			else
				::MessageBox( 0, XTOOLBAR_CRITICAL_TITLE, 0, 0 );
		}
	}
#endif
#endif // CHECK_EACH_START
	return XSUCCESS;
}

#define XERROR_GET_BTN_DATA -211
#define XERROR_XBUTTON_DATA_EXT_IS_NULL -212

LPCTSTR XFreeToolBar::GetErrorMsg( INT32 nErrorCode )
{
	switch( nErrorCode )
	{
	default:
		return "";
	}
	return "";
}

INT32 XFreeToolBar::GetBtnConfigInfo( INT32 nIndex, XToolbarItem **ppBtnConf )
{
	ATLASSERT( nIndex >= 0 );
	INT32 nRet;
	XBTNINFO *pXBtnInfo;
	
	nRet = GetXBtnInfo( nIndex, &pXBtnInfo );
	if( 0 > nRet )
	{
		XLOG( ( 0, _T( "XFreeToolBar::GetBtnConfigInfo Error :%d %s" ), nRet, GetErrorMsg( nRet ) ) );
		return nRet;
	}
	
	ATLASSERT( NULL != pXBtnInfo->lpExtension );
	*ppBtnConf = ( XToolbarItem* )pXBtnInfo->lpExtension;
	return XSUCCESS;
}


// collect present in system fonts
BOOL CALLBACK EnumFontFamiliesExProc( ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, INT32 FontType, LPARAM lParam )
{
	std::set< XString >* FontNames = ( std::set< XString >* )lParam;
	FontNames->insert( ( TCHAR* )lpelfe->elfFullName );
	return TRUE;
}

#define XERROR_NOT_FIND_FONT -221
INT32 XFreeToolBar::SetFonts()
{
	if( m_Font.m_hFont )
		m_Font.DeleteObject();

	INT32 nCharSet =
#ifdef _LANG_HEBREW
		HEBREW_CHARSET;
#else
		DEFAULT_CHARSET;
#endif

	std::set< XString > FontsName;

	//Enum all fonts this dc supports.
	HDC hDC = ::GetDC( NULL );
	LOGFONT LogFont = { 0, 0, 0, 0, 0, 0, 0, 0, nCharSet, 0, 0, 0,0, '\0' };
	EnumFontFamiliesEx( hDC, &LogFont,( FONTENUMPROC )EnumFontFamiliesExProc, reinterpret_cast< LPARAM >( &FontsName ), 0 );
	::ReleaseDC( NULL, hDC );

	//Find the first font in setting and is supported by this dc.
	for( INT32 i = 0; i < g_pXConfig->m_Settings.m_fonts.size(); i++ )
	{
		if( FontsName.find( g_pXConfig->m_Settings.m_fonts[i]->m_strName ) != FontsName.end() )
		{
			m_Font.CreateFont( g_pXConfig->m_Settings.m_fonts[i]->m_FontNum, 0, 0, 0, FW_NORMAL, 0, 0, 0, nCharSet,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS, g_pXConfig->m_Settings.m_fonts[i]->m_strName.c_str());
			break;
		}
	}

	if( NULL == ( HFONT )m_Font )
	{
		return XERROR_NOT_FIND_FONT;
	}

	SetFont( m_Font );
	return XSUCCESS;
}

INT32 XFreeToolBar::InitNormBtn()
{
	INT32 nBtnConfNum = g_pXConfig->m_BtnConfs.size();
	for(INT32 i = 0; i < nBtnConfNum; i++)
	{
		g_pXConfig->m_BtnConfs[i]->SetBtnCmdId( INIT_BTN_ID( i ) );
		g_pXConfig->m_BtnConfs[i]->SetBtnConfIndex( i );
		g_pXConfig->m_BtnConfs[i]->m_Command = g_pXConfig->FindCommandConf( g_pXConfig->m_BtnConfs[ i ]->m_strCmd );

		if( g_pXConfig->m_BtnConfs[i]->m_dwStyle == FIXED_RIGHT_SIDE_BTN )
		{
			m_pXBtnInfos[i].dwStyle = FIXED_RIGHT_SIDE_BTN;
			m_pXBtnInfos[i].bRealHide = TRUE;
		}
		else
		{
			m_pXBtnInfos[i].dwStyle = 0;
			m_pXBtnInfos[i].bRealHide = FALSE;
		}

		m_pXBtnInfos[i].lpExtension = ( LPVOID )g_pXConfig->m_BtnConfs[i];
		m_pXBtnInfos[i].pBtnData->iBitmap = GetImageId( g_pXConfig->m_BtnConfs[i]->GetConfImageIndex() );
		m_pXBtnInfos[i].pBtnData->idCommand = INIT_BTN_ID( i );
		m_pXBtnInfos[i].pBtnData->dwData = ( DWORD_PTR )&m_pXBtnInfos[i];
		m_pXBtnInfos[i].pBtnData->fsState = TBSTATE_ENABLED;
		m_pXBtnInfos[i].pBtnData->iString = i;
		m_pXBtnInfos[i].pBtnData->fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT | TBSTYLE_ALTDRAG | TBSTYLE_AUTOSIZE;
	}

#define END_LEFT_FIXED_BTN_INDEX 1
#define END_SIZE_BTN_INDEX 6
	for( INT32 i = 0; i < END_LEFT_FIXED_BTN_INDEX; i ++ )
	{
		m_pXBtnInfos[i].dwStyle = FIXED_LEFT_SIDE_BTN;//dwGroup = XFREEBAR_GROUP_FIXED_LEFT;
	}
	//for( INT32 i = END_LEFT_FIXED_BTN_INDEX; i < END_SIZE_BTN_INDEX; i ++ )
	//{
	//	m_pXBtnInfos[i].dwGroup = XFREEBAR_GROUP_SIZE;
	//}

	//for( INT32 i = END_SIZE_BTN_INDEX; i < nBtnConfNum; i ++ )
	//{
	//	m_pXBtnInfos[i].dwGroup = XFREEBAR_GROUP_FREE;
	//}
	return 0;
}

#define X_ERROR_PARSE_XML_CONF -31
// not good function name 
// really: yes build toolbar!!
INT32 XFreeToolBar::BuildToolBar()
{
	CWaitCursor wait;

	XToolBarUpdate();

	if( !ParseToolbarFromXML( g_pXConfig ) )
		return X_ERROR_PARSE_XML_CONF;

	if( XToolbarItem* item = g_pXConfig->MapBtnConfFromCmd( "search" ) )
		item->m_strCaption = gAppendCommandDisplay;

	setValue( _T( "showcorrupted" ), _T( "1" ) );

	setValue( _T( "gUpdate" ), _T( "0" ) );

	// is it time to update dll itself
#ifdef UPDATE_DLL
	XString cur_ver = getValue(_T("updateVer"),_T(""));
	if(!g_pXConfig->m_Settings.m_updateVer.empty() && !cur_ver.empty() && cur_ver!=g_pXConfig->m_Settings.m_updateVer) //update dll from server
	{
		if(::MessageBox(0,_T("A new version of the " + sProfileName + " toolbar is available.\nDo you want to install it now?"),XTOOLBAR_ALERT_TITLE,MB_YESNO)==IDYES)
		{
			CloseAllRuningIE();
			m_pToolBarHost->m_pWebBrowser->Quit();
			urlAfterUninstall = urlBeforeUpdate;
			DllUnregisterServer();
		}
	}
#endif

	setValue( _T( "updateVer" ), g_pXConfig->m_Settings.m_updateVer );

	SetFonts();

	m_bToolbarInited = FALSE;

	//Delete all buttons.
	INT32 nBtnCount = GetButtonCount();
	for( INT32 i = nBtnCount - 1; i >= 0; i-- )
		DeleteXBtns( i );

	// build toolbar data
	SetButtonStructSize( sizeof( TBBUTTON ) );
	//memset( m_pXBtnInfos, 0, sizeof( XBTNINFO ) * XFREEBAR_MAX_BTN_COUNT );

	InitChevronBtn();

	SetExtendedStyle( TBSTYLE_EX_DRAWDDARROWS );

	InitNormBtn();
	// apply images and hot images lists to toolbar
	if(!g_pXConfig->m_bmpFileName.empty())
	{	
		XString filebmp = g_pXConfig->m_bmpFileName;

#ifdef _ODBC_DIRECT
#include "getBimapFilename.inl"
#endif

		CSize bSize;
		CBitmap(LoadImage(filebmp)).GetSize(bSize);
		bSize.cx = bSize.cy;
		SetBitmapSize(bSize);

#ifdef _THELOCALSEARCH
		bSize.cx = 40;
		bSize.cy = 24;
#endif

#ifndef _ODBC_DIRECT
		CheckUpdate(filebmp);

		XString filebmp2 = g_pXConfig->m_bmpHotFileName;
		if(!filebmp2.empty())
			CheckUpdate( filebmp2 );

		m_NormList.Destroy(); m_HotList.Destroy();
		m_NormList.CreateFromImage(filebmp.c_str(),bSize.cx,0,maskClr,IMAGE_BITMAP,LR_CREATEDIBSECTION|BMP_LOADFLAG);
		m_HotList.CreateFromImage(filebmp2.empty() ? filebmp.c_str():filebmp2.c_str(),bSize.cx,0,maskClr,IMAGE_BITMAP,LR_CREATEDIBSECTION|BMP_LOADFLAG);
#else
#include "applyBimapFilename.inl"
#endif

		SetHotImageList(m_HotList);
		m_hImageList = m_NormList.m_hImageList;
		SetImageList(m_NormList);
	}

	//Initialze the default toolbar settings.
	g_pXConfig->m_Settings.Init();

	AddLeftSideBtns();
	AddHilightBtns();
	AddChevronBtn();
	AddRightSideBtns();
	
	//EnableButton( FILLED_BUTTON_ID, FALSE );
	//CSize s; GetPadding(&s); SetPadding(s.cx,0);
	//make sure the control is where it should be. 
	AutoSize();

	m_bToolbarInited = TRUE;
	INT32 nBtnConfNum = g_pXConfig->m_BtnConfs.size();
	for( INT32 i = 0; i < nBtnConfNum; i++ )
	{
		g_pXConfig->m_BtnConfs[i]->XChange( this );
	}

	g_pXConfig->OnOptionChange( this );

#ifdef _DEBUG_CHEVRON
#define CHEVRON_TITLE "Chevron"
#define FILLED_TITLE "Filled"
	CHAR TitleStrings[_MAX_PATH];

	INT32 nOffset = 0;
	memcpy(TitleStrings, FILLED_TITLE, sizeof(FILLED_TITLE));
	nOffset += sizeof(FILLED_TITLE);
	memcpy(TitleStrings + nOffset, CHEVRON_TITLE, sizeof(CHEVRON_TITLE));
	nOffset += sizeof(CHEVRON_TITLE);
	*(TitleStrings + nOffset + 1) = '\0';

	AddStrings(TitleStrings);
#endif

	UpdateToolTipInfo( m_pToolBarHost->GetBlockCount(), _T( "ToggleBlock" ) );
	//OnOptionChange();
	EnableHighlight( FALSE );

	//calculate current width of toolbar for correct chevrons behaiviour
	CalcXFreeBarWidth();
	SetBtnsWidth( 0, GetButtonCount() );
	DumpAllButtonInfo();

#ifdef _DEBUG //check that we don't forget about any commands
	for( INT32 wID = MENU_COMMAND_ID_BEGIN; wID < MENU_COMMAND_ID_END; wID++ )
	{
		IMenuItem *item = g_pXConfig->GetMenuItemOnButton( wID );
		if( item && !item->m_strCmd.empty() && g_pXConfig->FindCommandConf( item->m_strCmd ) == 0 )
			::MessageBox( 0,( _T( "Can't find command " ) + item->m_strCmd ).c_str(), XTOOLBAR_ALERT_TITLE, 0 );
	}
#endif

	DefWindowProc( WM_PAINT, 0, 0 );
	return XSUCCESS;
}

#include "winuser.h"
// ripped off from WTL's ATLFRAME_H
// ripped off from WTL's ATLFRAME.H
// build toolbar - called only once during IE instance life
HWND XFreeToolBar::CreateSimpleToolBarCtrl( HWND hWndParent, DWORD dwStyle, DWORD dwExStyle, UINT uID )
{
	INT32 nRet;
#if defined(_ODBC_DIRECT)
	gAppendCommand = getValue(_T("Toolbar Extension"),_T("Business"));
	gAppendCommandDisplay = _T("");
#elif defined(_GERMAN)
	gAppendCommand = getValue(_T("Toolbar Extension"),_T("GSSchnellsuche"));
	gAppendCommandDisplay = _T("Schnellsuche");
#else
	gAppendCommand = getValue(_T("Toolbar Extension"),_T("Google"));;
	gAppendCommandDisplay = _T("Search");
#endif

#ifdef _SERVAGE 
	if(isGetValue2(_T("gUpdate")))
		(HANDLE)_beginthread(CloseAll,0,0);
#endif

	CRect rect( 0, 0, 100, TB_HEIGHT );
	HWND hWnd = CCommandBarCtrlImpl<XFreeToolBar, CCommandBarCtrlBase>::Create( hWndParent, rect, NULL, dwStyle, dwExStyle, uID );//,WS_EX_TOOLWINDOW);
	GetSystemSettings(); //!!!!remove - present in CCommandBarCtrlImpl

	//SetWindowLong( GCL_HCURSOR, NULL );
#if defined(_ODBC_DIRECT)
#include "ToolBand7\UserXml.cpp"
#endif

	//Modify toolbar style according to new text options
	nRet = BuildToolBar();

#if defined( USE_TICKER ) || defined( _SERVAGE ) || defined ( _MB )
	//m_hTickerThread = (HANDLE)_beginthread( TickerThread, 0, this );
#endif
	//TBSTYLE_SEP: The button is a seperator
	/*Create the toolbar steps:
	1.Get the resource handle by the LoadResource function which parameters is _Module.ResourceInstance and uIDResource
	2.Get the pointer that point to resource data, use a _AtlToolbar struct
	3.Create the array of the struct of toolbar button, which common is the TBSTRUCT,that descript the button info,notation:if item member of the 
	struct is 0, indicate it's a seprator.
	4.Send the TB_ADDBUTTONS message,use the array to a parameter
	5.Make some additional initialize, such as to set font etc.notation:TEXTMETRIC struct and the GetTectMetrics function.
	*/
	//RegisterDragDrop function can give the combobox or editbox a drag or drop ability.

	return hWnd;
}

//void XFreeToolBar::SetToolBarButtonInfo(INT32 nID, DWORD dwMask, BYTE fsStyle)
//{
//	TBBUTTONINFO tbi;
//	tbi.cbSize = sizeof(TBBUTTONINFO);
//	tbi.dwMask = dwMask;
//	tbi.fsStyle = fsStyle;
//	SetButtonInfo(nID, &tbi);
//}

INT32 g_ToolWillReload = FALSE;
DWORD CALLBACK ReloadXToolBarThread( LPVOID pParam )
{

	XFreeToolBar *pXToolBar = (XFreeToolBar*)pParam;
	while( g_ToolWillReload )
	{
		Sleep( 1 );
	}

	pXToolBar->BuildToolBar();
	return 0;
}

INT32 XFreeToolBar::ReloadToolBar()
{
	::CreateThread( NULL, 0, ReloadXToolBarThread, (LPVOID)this, NULL, NULL );
	return 0;
}

LRESULT XFreeToolBar::OnConfig()
{
	_bstr_t strTitle = _T("XFree Bar Custom");
	//CConfigPropertySheet SheetConfig( ( TCHAR* )strTitle, 0 );

	//SheetConfig.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	//if( SheetConfig.DoModal() == IDOK )
	//{
		//g_ToolWillReload = TRUE;
		//ReloadToolBar();
	//}

	return 0;
}


LRESULT XFreeToolBar::OnToolbarNeedText( INT32 nBtnId, LPNMHDR pNmHdr, BOOL& bHandled )
{
	CString sToolTip;
	//--make sure this is not a separator
	if( nBtnId != 0 )
	{
		if(!sToolTip.LoadString( nBtnId ))
		{
			bHandled = FALSE;
			return 0;
		}
	}

	LPNMTTDISPINFO pttdi = reinterpret_cast<LPNMTTDISPINFO>(pNmHdr);

	pttdi->lpszText = MAKEINTRESOURCE( nBtnId );
	pttdi->hinst = _Module.GetResourceInstance();
	pttdi->uFlags = TTF_DI_SETITEM;

	//--message processed
	return 0;
}

// we should show drop down menu
LRESULT XFreeToolBar::OnButtonDropDown( INT32, LPNMHDR pNmHdr, BOOL& bHandled)
{
	INT32 nBtnID;
	INT32 nBtnIndex;
	CRect rcBtn;
	
	bHandled = TRUE;
	NMTOOLBAR* pNMTB = (NMTOOLBAR *) pNmHdr;
	rcBtn = pNMTB->rcButton;
	MapWindowPoints( HWND_DESKTOP, ( POINT * )&rcBtn, 2 );

	if( pNMTB->hdr.hwndFrom != m_hWnd ) // another toolbar
	{
		bHandled = FALSE;
		return S_OK;
	}

	nBtnID = pNMTB->iItem;
	nBtnIndex = MapBtnIndexFromID( nBtnID );
	if( nBtnIndex == -1 )
	{
		bHandled = FALSE;
		return S_OK;
	}

	g_pXConfig->m_BtnConfs[ nBtnIndex ]->ShowMenu( this, m_hWnd, rcBtn.left, rcBtn.bottom );
	return S_OK;
}


LRESULT XFreeToolBar::OnQueryDelete(INT32 idCtrl, LPNMHDR pNmHdr, BOOL& bHandled)
{
	bHandled = FALSE;
	//bHandled = TRUE;
	return 0;
}

LRESULT XFreeToolBar::OnQueryInsert(INT32 idCtrl, LPNMHDR pNmHdr, BOOL& bHandled)
{
	bHandled = FALSE;
	//bHandled = TRUE;
	return 0;
}

INT32 CalcChevronPos(HWND hWnd, LPRECT lpChevRc, LPPOINT lpPt)
{
	ATLASSERT(lpPt != NULL && lpChevRc != NULL);

	INT32 nWidScr;
	INT32 nHeightScr;
	INT32 nWidRemain;
	INT32 nHeigtRemain;

	RECT WndRc;
	GetWindowRect(hWnd, &WndRc);
	nWidScr = GetSystemMetrics(SM_CXSCREEN);
	nHeightScr = GetSystemMetrics(SM_CYSCREEN);

	nWidRemain = nWidScr - WndRc.right;
	nHeigtRemain = nHeightScr - WndRc.bottom;
	
	if(nWidRemain < (lpChevRc->right - lpChevRc->left))
	{
		lpPt->x = WndRc.right - (lpChevRc->right - lpChevRc->left);
	}
	else
	{
		lpPt->x = WndRc.right;
	}

	if(nHeigtRemain < (lpChevRc->bottom - lpChevRc->top))
	{
		lpPt->y = WndRc.top - (lpChevRc->bottom - lpChevRc->top);
	}
	else
	{
		lpPt->y = WndRc.bottom;
	}
	return 0;
}

#define CHEVRON_WIDTH 20
INT32 CalcChevronButton(CToolBarCtrl *pToolbar)
{
	UINT32 nBtnCount = pToolbar->GetButtonCount();
	RECT ToolbarRc;
	pToolbar->GetClientRect(&ToolbarRc);
	TBBUTTONINFO tbi;

	//TBInfoSetter(pToolbar, CHEVRON_BUTTON_ID, TRUE).state(TBSTATE_HIDDEN);
	for(UINT32 i = 0; i < nBtnCount; i++)
	{
		tbi.cbSize = sizeof TBBUTTONINFO;
		tbi.dwMask = TBIF_STATE | TBIF_SIZE | TBIF_COMMAND;
		INT32 ret = pToolbar->GetButtonInfo( WM_USER + i, &tbi);
		RECT rc;
		pToolbar->GetItemRect(i, &rc);
		//CString strMsg;
		//strMsg.Format("The button %d left is %d, right is %d, top is %d, bottom is %d\n", 
		//	i, rc.left, rc.right, rc.top, rc.bottom);
		//::OutputDebugStr(strMsg);

		if(rc.left <= (ToolbarRc.right - CHEVRON_WIDTH) && rc.right >= (ToolbarRc.right - CHEVRON_WIDTH) )
		{
			//TBInfoSetter(pToolbar, CHEVRON_BUTTON_ID, TRUE).state(TBSTATE_ENABLED);
			return i;
		}
	}
	//TBInfoSetter(pToolbar, CHEVRON_BUTTON_ID, TRUE).state(TBSTATE_ENABLED);
	return -1;
}

#include <winnt.h>
HRESULT XFreeToolBar::OnIEBorderResize(LPCRECT prcIEBorder)
{
	//DepartToolBar();
	return S_OK;
}

LRESULT XFreeToolBar::OnChevronPushed(INT32, LPNMHDR pNmHdr, BOOL& bHandled)
{
	bHandled = TRUE;

	//if(m_nChevBtnIndex == -1)
	//{
	//	bHandled = FALSE;
	//	return 0;
	//}
	NMREBARCHEVRON* pChevron = (NMREBARCHEVRON*)pNmHdr;
	//if(m_pToolBarHost->GetBandID() != pChevron->wID)
	//{
	//	bHandled = FALSE;
	//	return 0;
	//}

	//INT32 nBtnCount = g_pXConfig->m_BtnConfs.size();
	POINT ChevronPt;
	CalcChevronPos(m_hWnd, &pChevron->rc, &ChevronPt);
		RECT ToolbarRc;
	GetClientRect(&ToolbarRc);
	ScreenToClient(&ToolbarRc);

	if(m_pChevDrop == NULL)
	{
		m_pChevDrop = new CChevDrop();

		if(m_pChevDrop != NULL)
			m_pChevDrop->CreatePopup(m_pToolBarHost->GetInvisibleWnd());
	}

	//TBBUTTON *pTBButton;
	INT32 nAllBtnCount = GetButtonCount();
	//nAllBtnCount -= CHEVRON_BUTTON_COUNT;
	//INT32 nAllBtnCount = g_pXConfig->m_BtnConfs.size();
	INT32 nOtherBtnCount = nAllBtnCount - m_nChevBtnIndex;
	//pTBButton = new TBBUTTON[ nOtherBtnCount ];
	BOOL *pButtonsHide = new BOOL[ nOtherBtnCount ];
	XBTNINFO *pXBtnInfo;
	for( INT32 i = m_nChevBtnIndex; i < nAllBtnCount; i++ )
	{
		//if( i >= nBtnCount)
		//{
		pButtonsHide[ i - m_nChevBtnIndex ] = IsRealHideBtn( i );
		//}
		//else
		//{
		//	pButtonsHide[ i - m_nChevBtnIndex ] = IsRealHideBtn( i );
		//}
		//ATLTRACE("Button %d Hide ? %d\n", i, pButtonsHide[ i - m_nChevBtnIndex ]);
	}

	if(m_pChevDrop != NULL)
	{
		//for(INT32 i = nMasterBtnCount - 1; i >= 0; i --)
		//{
			//TBInfoSetter(this, i).state(TBSTATE_ENABLED);
		//}
		//m_pChevDrop->ShowPopup(m_pToolBarHost->GetInvisibleWnd(), this->m_hWnd, ToolbarRc, ChevronPt);
			//BOOL ShowPopup(	CWindow MsgReceiver,
			//		CToolBarCtrl ToolBar,
			//		DWORD dwItemWidth,
			//		DWORD dwItemHeight,
			//		INT32 nChevIndex,
			//		CPoint ptScreen,
			//		UINT uFlags = XFREE_SINGLE_ROW );

		m_pChevDrop->ShowPopup(m_pToolBarHost->GetInvisibleWnd(), this->m_hWnd, 120, 25, m_nChevBtnIndex, CHEVRON_BUTTON_COUNT, pButtonsHide, ChevronPt);
		delete[] pButtonsHide;
		//for(INT32 i = nMasterBtnCount - 1; i >= m_nChevBtnIndex; i --)
		//{
		//	TBInfoSetter(this, i).state(TBSTATE_HIDDEN);
		//}
	}

	//delete pTBButton;

	//UINT32 nBand = pChevron->uBand;
	//WORD wID = pChevron->wID;
	//::ScreenToClient(m_hWnd, (LPPOINT)&ChevronRc);

 //	CMenu MenuPopup;
	//MenuPopup.CreatePopupMenu();
	//TBBUTTON BtnData;

	//for(INT32 i = nBand; i < nBtnCount; i++)
	//{
	//	if(GetButton(i, &BtnData) == FALSE)
	//		continue;

	//	if(BtnData.iBitmap == -2)
	//		continue;

 //		XFreeToolBar::_XFreeMenuItemData* pMI = new XFreeToolBar::_XFreeMenuItemData();
	//	pMI->fType = MF_STRING;
	//	pMI->lpstrText = (LPTSTR)m_pXFreeToolbar->m_BtnConfs[i]->m_strCaption.c_str();
	//	pMI->iButton = BtnData.iBitmap;
	//	MenuPopup.AppendMenu(MF_OWNERDRAW, MapBtnIDFromIndex( i )/*BtnData.idCommand*/, LPCTSTR(pMI));
	//}
	//::TrackPopupMenu(MenuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, ChevronPt.x, ChevronPt.y, 0, m_hWnd, NULL);
	//XFreeToolBar::_XFreeMenuItemData::MenuItems.clear();

	//CToolMenu   pop;   
    //pop.CreatePopupMenu ();   
   
    // The CToolMenu uses the toolbar pointer to get associated text and image    
    // information. If there is a better way please do so.    
    // This menu was created with local scope, hence the temp pointer    
    // pTBar can be stored i guess...    
    //pop.m_pToolbar = pTBar;     
    //pop.m_pImg = pTBar->GetToolBarCtrl().GetImageList();   
   
// We have to XDetermine the hidden buttons    
   
//  1. Obtain band rectangle (subtract chevron width if you want to)    
//  2. Get the button rectangle    
//  3. Check if the intersecting rectangle is same as the button rectangle    
//  4. If they intersect then the button is shown  

	return 0;
}

//BYTE szAllBtnState[]
// pre process keyboard input 
STDMETHODIMP XFreeToolBar::TranslateAcceleratorIO(LPMSG pMsg)
{
	for(IBandEdits Iter( g_pXConfig->m_vecCombos.size(), g_pXConfig->m_vecCombos.begin() );Iter;Iter++)
		if (Iter->IsFocus(GetFocus()))
			return Iter->TranslateAcceleratorIO(pMsg);
	return S_FALSE;
}

//LRESULT XFreeToolBar::OnMenuSelect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	WORD nID = LOWORD(wParam);
//	WORD wFlags = HIWORD(wParam);
//
//	//make sure thsi is not a separator
//	CString sStatusBarDesc;
//
//	if (!(wFlags & MF_POPUP))
//	{
//		if(nID != 0)
//		{
//			if(!sStatusBarDesc.LoadString(nID))
//			{
//				bHandled = FALSE;
//				return 0;
//			}
//
//			INT32 nPos = sStatusBarDesc.MapBtnConfIndexFromCmd(_T("\n"));
//			if(nPos != -1)
//			{
//				sStatusBarDesc =sStatusBarDesc.Left(nPos + 1);
//				m_pWebBrowser->put_StatusText(_bstr_t(sStatusBarDesc));
//				return 0;
//			}
//		}
//	}
//	return 0;
//}

void XFreeToolBar::UpdateToolBarElement(INT32 nID, bool bEnable)
{
	TBBUTTONINFO tbi;
	GetButtonInfo(nID, &tbi);
	if(bEnable)
		tbi.fsState |= TBSTATE_ENABLED;
	else
		tbi.fsState &= ~TBSTATE_ENABLED;
	SetButtonInfo(nID, &tbi);
}

HWND XFreeToolBar::FindRebar(HWND hWndStart)
{
	HWND hWndFound = NULL;
	if((hWndFound = 
		FindWindowEx(hWndStart, NULL, REBARCLASSNAME, NULL)) != NULL)
		return hWndFound;

	HWND hWndEnum = NULL;
	while((hWndEnum = FindWindowEx(hWndStart, hWndEnum, NULL, NULL)) != NULL)
	{
		if((hWndFound = FindRebar(hWndEnum)) != NULL)
			return hWndFound;
	}
	return NULL;
}

//Message handler
//hook keyboard events, set focus between toolbar or combobox when press the TAB.
LRESULT XFreeToolBar::OnHookKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	if(wParam == VK_TAB)
	{
		HWND hwnd = GetFocus();
		for(IBandEdits Iter( g_pXConfig->m_vecCombos.size(), g_pXConfig->m_vecCombos.begin() );Iter;Iter++)
		{
			if (Iter->IsFocus(hwnd))
			{
				if(::GetKeyState(VK_SHIFT) & 0x80)
					Iter--;
				else
					Iter++;
				if(Iter)
					Iter->SetFocus2();
				else
					::SetFocus(GetParent());
				bHandled = TRUE;
				return S_OK;
			}
		}
		HWND w5 = ::GetParent(::GetParent(hwnd));
		HWND w6 = ::GetParent(w5);
		HWND w1 = GetParent();
		if(w1==w6 && w5!=m_hWnd)
		{
			if(g_pXConfig->m_vecCombos.size())
				g_pXConfig->m_vecCombos[0]->SetFocus();
			bHandled = TRUE;
		}
	}

	//if(wParam == VK_RETURN)
	//{
	//	g_pXConfig->m_EditGetText();
	//	IWebBrowser2 *pIE = RunIe();
	//	pIE->Navigate2(&varURL, &varEmpty, &varEmpty, &postData, &varHeaders);
	//}
	return S_OK;
}

// catch mouse over event
LRESULT XFreeToolBar::OnHookMouseMove(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	RECT rcToolbar;
	DWORD dwPoint = ::GetMessagePos();
	POINT ptCur;
	POINT ptDrag;
	//INT32 nSepHited;
	ptCur.x = GET_X_LPARAM(dwPoint);
	ptCur.y = GET_Y_LPARAM(dwPoint);
	ptDrag = ptCur;

	GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	ScreenToClient(&ptCur);

	INT32 nHit = HitTest(&ptCur);

	//MoveTheImage(ptDrag);

	rcToolbar.left = rcToolbar.right - CHEVRON_BUTTON_WIDTH;
	if(ptCur.x >= rcToolbar.left &&
		ptCur.x <= rcToolbar.right &&
		ptCur.y >= rcToolbar.top &&
		ptCur.y <= rcToolbar.bottom)
	{
		//Invalidate(0);
		//ATLTRACE("Update window on CXFreeToolBar::OnHookMouseMove\n");
		//UpdateWindow();
		SendMessage( m_hWnd, WM_PAINT, 0, 0 );
	}

	static INT32 nSplitterIndex;

	bHandled = FALSE;

	if( m_bLBtnDown )
	{
		g_pXConfig->m_splitters[ nSplitterIndex ]->StopMouseEvents(this);
		return S_OK;
	}

	if( g_pXConfig && nHit >= 0 && nHit < g_pXConfig->m_BtnConfs.size() )
	{
		bHandled = g_pXConfig->m_BtnConfs[ nHit ]->StopMouseEvents();
		m_bSplitFocused = false;
		return S_OK;
	}

	//return S_OK; //test
	//RECT rcBtn;

	//nSepHited = - nHit; //Because the return value of the TB_HITTEST message with separater is negative.
	for(INT32 i = 0; i < g_pXConfig->m_splitters.size(); i++)
	{
		//g_pXConfig->m_splitters[i]->GetSplitterRegion( this, &rcBtn );
		//if(PtInRect( &rcBtn, ptCur ) )
		//{
		//	m_bSplitFocused = true;
		//	nSplitterIndex = i;
		//	return S_OK;
		//}

		ATLTRACE( "The hited button id is %d the item id is %d\n", nHit, g_pXConfig->m_splitters[ i ]->GetBtnCmdId() );
		if( MapBtnIndexFromID( g_pXConfig->m_splitters[ i ]->GetBtnCmdId() ) ==  -( nHit + 1 ) )
		{
			m_bSplitFocused = true;
			nSplitterIndex = i;
			return S_OK;
		}
	}

	m_bSplitFocused = false;
	return S_OK;
}

LRESULT XFreeToolBar::OnHookRebarSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	INT32 nLeftBtnWidth;
	INT32 nRightBtnWidth;
	INT32 nMinLeftWidth;
	INT32 nSizedWidth;
	RECT rcCombo;

#define DEFAULT_COMBOBOX_WIDTH 200
	nSizedWidth = ( INT32 )LOWORD( lParam );
	nLeftBtnWidth = CalcFixedLeftSideBtnWidth();
	nRightBtnWidth = CalcFixedRightSideBtnWidth();

	nMinLeftWidth = nLeftBtnWidth + CHEVRON_BUTTON_WIDTH + TOOLBAR_RESERVE_SPACE + nRightBtnWidth;

	INT32 nBtnIndex;
	IXEditCtrl* pEdit;
	nBtnIndex = 1;
	IBandEdits Iter( g_pXConfig->m_vecCombos.size(), g_pXConfig->m_vecCombos.begin() );
	pEdit = (IXEditCtrl* )Iter;
	GetItemRect( nBtnIndex, &rcCombo );

	if( nSizedWidth > nMinLeftWidth + COMBOBOX_MIN_WIDTH && nSizedWidth < nMinLeftWidth + ( rcCombo.right - rcCombo.left ) )
	{
		rcCombo.right = nSizedWidth - nMinLeftWidth;
		nBtnIndex = 1;
		TBInfoSetter( this, MapBtnIDFromIndex( nBtnIndex ), TRUE ).size( rcCombo.right - rcCombo.left ).text( _T( "" ) );

		pEdit->MoveWindow( &rcCombo );

		SetBtnsWidth( nBtnIndex, nBtnIndex + 1 );
	}
	else if( nSizedWidth > nMinLeftWidth + ( rcCombo.right - rcCombo.left ) )
	{
		if( ( rcCombo.right - rcCombo.left ) < ( nSizedWidth - nMinLeftWidth ) && ( rcCombo.right - rcCombo.left ) < DEFAULT_COMBOBOX_WIDTH )
		{
			rcCombo.right = nSizedWidth - nMinLeftWidth;
			nBtnIndex = 1;
			TBInfoSetter( this, MapBtnIDFromIndex( nBtnIndex ), TRUE ).size( rcCombo.right - rcCombo.left ).text( _T( "" ) );

			pEdit->MoveWindow( &rcCombo );

			SetBtnsWidth( nBtnIndex, nBtnIndex + 1 );
		}
	}

	m_RebarContainer.DefWindowProc(uMsg, wParam, lParam);
	DepartToolBar();
	CalcXFreeBarWidth();

	ATLTRACE("XFreeToolBar::OnHookRebarSize wparam :%d, cx %d, cy %d\n", wParam, LOWORD(lParam), HIWORD(lParam));
	bHandled = TRUE;
	return S_OK;
}

LRESULT XFreeToolBar::OnHookIEFrameSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLTRACE("XFreeToolBar::OnHookIEFrameSize new width is %d\n", HIWORD( lParam ) );
	bHandled = FALSE;
	CallWindowProc(g_fSuperIEFrameProc, hWnd, uMsg, wParam, lParam);
	bHandled = TRUE;
	return 0;
}

LRESULT XFreeToolBar::OnHookSetCursor(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	if(m_bSplitFocused || m_bLBtnDown)
	{
		SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		bHandled = TRUE;
	}
	else if(m_nDragingButton != 0)
	{
		SetCursor(::LoadCursor(NULL, IDC_HAND));
		bHandled = TRUE;
	}
	return S_OK;
}

LRESULT XFreeToolBar::OnHookGetItemRect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	TBBUTTON TBInfo;
	GetButton(wParam, &TBInfo);

	if(TBInfo.idCommand == CHEVRON_BUTTON_ID)
	{
		CRect *pRcChevron = (CRect*)lParam;
		GetWindowRect(pRcChevron);
		ScreenToClient(pRcChevron);
		pRcChevron->right = pRcChevron->Width() - 100;
		pRcChevron->left = pRcChevron->right - CHEVRON_BUTTON_WIDTH;
		pRcChevron->bottom = pRcChevron->Height();
		pRcChevron->top = 0;
		bHandled = TRUE;
		return 0;
	}
	CRect *pRcChevron = (CRect*)lParam;
	pRcChevron->left = pRcChevron->right = pRcChevron->top = pRcChevron->bottom = 0;
	bHandled = TRUE;
	return 0;
}

LRESULT XFreeToolBar::OnHookTBHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = TRUE;

	//ATLTRACE("Enter XFreeToolBar::OnHookTBHitTest\n");
	INT32 nHited;
	TBBUTTON TBInfo;
	UINT32 uBtnID;
	nHited = CallWindowProc(m_pfnSuperWindowProc, m_hWnd, TB_HITTEST, wParam, lParam);

	BOOL nRes;
	nRes = GetButton(nHited, &TBInfo);

	uBtnID = TBInfo.idCommand;
	if(CHEVRON_BUTTON_ID != uBtnID && FALSE != nRes)
	{
		return nHited;
	}
	else
	{
		CPoint *pPt = (CPoint*)lParam;
		CRect rcToolbar;
		CRect rcBtn;
		GetWindowRect(&rcToolbar);
		ScreenToClient(&rcToolbar);

		//ATLTRACE("pt x %d, pt y %d, rect left %d, rect right %d, rect top %d, rect bottom %d\n", pPt->x, pPt->y, 
		//	rcToolbar.left, rcToolbar.right, rcToolbar.top, rcToolbar.bottom);
		rcToolbar.left = rcToolbar.right - CHEVRON_BUTTON_WIDTH;
		if(pPt->x >= rcToolbar.left && 
			pPt->x <= rcToolbar.right &&
			pPt->y >= rcToolbar.top &&
			pPt->y <= rcToolbar.bottom)
		{

			INT32 nCount = GetButtonCount();
			for(INT32 i = nCount - 1; i>= 0; i--)
			{
				GetButton(i, &TBInfo);
				if( CHEVRON_BUTTON_ID == TBInfo.idCommand )
				{
					//ATLTRACE("The hited button id is CHEVRON_BUTTON_ID\n");
					bHandled = TRUE;
					return i;
				}
			}
		}
	}
	bHandled = TRUE;
	return -1;
}

LRESULT XFreeToolBar::OnHookTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//TCHAR szTitle[_MAX_PATH];
	//::GetWindowText(m_hWnd, szTitle, _MAX_PATH);

	//ATLTRACE(_T("The IE title is %s\n"), szTitle);
	//bHandled = TRUE;
	//return 0;

	bHandled = FALSE;
	static INT32 nToolbarWidth = 0;
	CRect rcToolbar;
	GetWindowRect(&rcToolbar);
	if(rcToolbar.Width() != nToolbarWidth)
	{
			//DepartToolBar();
			nToolbarWidth = rcToolbar.Width();	
	}
	
	bHandled = TRUE;
	return 0;
}

LRESULT XFreeToolBar::OnHookPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//::MessageBox(NULL,_T("Entern the XFreeToolBar::OnHookPaint Function"), _T("Tip"), 0);
		//TCHAR szTitle[_MAX_PATH];
	//::GetWindowText(m_hWnd, szTitle, _MAX_PATH);

	//ATLTRACE(_T("The IE title is %s\n"), szTitle);
	//bHandled = TRUE;
	//return 0;
	INT32 nRet;
	nRet = 0;
	static BOOL bFirst = TRUE;
	if(bFirst)
	{
		//__asm int 3;
		CalcXFreeBarWidth();
		//DefWindowProc(uMsg, wParam, lParam);
		bFirst = FALSE;
	}

	//CallWindowProc(m_pfnSuperWindowProc, m_hWnd, WM_PAINT, wParam, lParam);
	//CDC DrawDC = GetDC();
	CPaintDC DrawDC( m_hWnd );
	//DrawDC.Rectangle(600, 2, 800, 5);
	//bHandled = TRUE;
	//return 0;

	//static UINT i = 0;
	//DWORD dwBtnBkClr;
	//i ++;
	//CRect rcToolbar;
	//GetWindowRect(&rcToolbar);
	////ScreenToClient(&rcToolbar);
	//rcToolbar.right = rcToolbar.Width();
	//rcToolbar.left = rcToolbar.right - CHEVRON_BUTTON_WIDTH;
	//rcToolbar.bottom = rcToolbar.Height();
	//rcToolbar.top = rcToolbar.bottom - 22;

	//if((i % 10 ) < 5)
	//{
	//	dwBtnBkClr = PRESSED_BTN_BK_CLR;
	//}
	//else
	//{
	//	dwBtnBkClr = NORMAL_BTN_BK_CLR;
	//}

	//GradientDraw(DrawDC, rcToolbar, CSize(2, 2), dwBtnBkClr, 0);

	//bHandled = TRUE;
	//return 0;

	CPen TBPen;
	CPenHandle OldPen;
	CBrush TBBrush;
	CBrushHandle OldBr;
	CFontHandle OldFont;
	INT32 nOldMode;
	TBBUTTON TBInfo;
	INT32 nBtnCount;
	TCHAR pszText[MAX_PATH];
	//TCHAR *pszText;
	//CString strText;
	RECT rcBtn;
	INT32 nHited;
	HIMAGELIST hImgList;
	HIMAGELIST hHotImgList;
	hImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETIMAGELIST, 0, 0L);
	hHotImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETHOTIMAGELIST, 0, 0L);

	if(NULL == hImgList)
	{
#define GET_IMAGELIST_ERR_ON_PAINT -3
		//::MessageBox(NULL, _T("Error"), _T("Get the image list of the toolbar error"), 0);
		nRet = GET_IMAGELIST_ERR_ON_PAINT;
		goto OnPaint_Exit;
	}

	//TempImgList.FromHandle(hImgList);
	
	//TBPen.CreatePen( PS_SOLID, 1, RGB(10,101,10));
	//TBBrush.CreateSolidBrush(RGB(300,180, 195));
	//DrawDC = GetDC();

	//OldPen = DrawDC.SelectPen(TBPen);
	//pOldBr = DrawDC.SelectPen(TBBrush);
	//OldFont = DrawDC.SelectPen(GetFont());

	IMAGEINFO TmpImgInfo;
	INT32 nVOffset;
	INT32 nHOffset;
	RECT rcImg;
	CPoint ptCur;
	if( FALSE == ::GetCursorPos( &ptCur ) )
	{
		//::MessageBox(NULL, _T("Error"), _T("Get current cursor pos error"), 0);
#define GET_CUR_POS_ERR_ON_PAINT -14
		nRet = GET_CUR_POS_ERR_ON_PAINT;
		goto OnPaint_Exit;
	}

	ScreenToClient(&ptCur);

	UINT32 uID;
	INT32 uImg;
	UINT32 uStyle;
	UINT32 uState;
	DWORD dwBtnBkClr = 0;
	DWORD dwBtnFrClr = 0;
	DWORD dwIDCmd;
	CSize szRound;
#define ROUND_LEN 3
	szRound.cx = ROUND_LEN;
	szRound.cy = ROUND_LEN;

	//ATLTRACE("SendMessage TB_HITTEST on XFreeToolBar::OnHookPaint\n");
	//DrawDC = GetDC();
	nHited = ::SendMessage( m_hWnd, TB_HITTEST, 0, (LPARAM)&ptCur );
	//ATLTRACE("SendMessage TB_HITTEST on XFreeToolBar::OnHookPaint return %d\n", nHited);
	//ATLTRACE("Cursor Pos x:%d, y:%d, Hited button is %d\n",ptCur.x, ptCur.y, nHited);
	nBtnCount = ::SendMessage( m_hWnd, TB_BUTTONCOUNT, 0, 0L );
	TBBUTTONINFO TmpBtnInfo;
	OldFont = DrawDC.SelectFont( GetFont() );
	nOldMode = DrawDC.SetBkMode( TRANSPARENT );
	//CRect rcToolbarTest;
	//GetWindowRect(&rcToolbarTest);
	//ScreenToClient(&rcToolbarTest);
	//DrawDC.Rectangle(&rcToolbarTest);
	//CRect rcChevron;
	//GetWindowRect(&rcChevron);
	////ScreenToClient(&rcChevron);
	//rcChevron.right = rcChevron.Width();
	//rcChevron.left = rcChevron.right - CHEVRON_BUTTON_WIDTH;
	//rcChevron.bottom = rcChevron.Height();
	//rcChevron.top = 0;


	for( INT32 i = 0; i < nBtnCount; i ++ )
	{
		//if( i == 10 )
		//{
		//	int k = 0;
		//}

		if(GetButton(i, &TBInfo) == FALSE)
		{
			continue;
		}
		dwIDCmd = TBInfo.idCommand;

		TBInfoGetter(this, dwIDCmd, &TmpBtnInfo, TRUE).style().image().state();//.text();
		
		uState = TmpBtnInfo.fsState;
		//ATLTRACE("Button %d String is %d Btimap is %d Command id is %d, State is %d, Chevron id is %d\n", i, TBInfo.iString, TBInfo.iBitmap, dwIDCmd, uState, CHEVRON_BUTTON_ID);
		if(uState & TBSTATE_HIDDEN)
		{
			continue;
		}

		uStyle = TmpBtnInfo.fsStyle;
		uImg = TmpBtnInfo.iImage;


		//GetButtonInfo(i, uID, uStyle, uImg);
		GetItemRect( i, &rcBtn );
		//ATLTRACE("Button rectangle left :%d, top :%d, right :%d, bottom :%d\n", rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
		
		//if(i == 0 || i  == 1)
		//{
		//	CRect rcToolbar;
		//	GetWindowRect(&rcToolbar);
		//	ATLTRACE("The toolbar width %d calc width %d\n", rcToolbar.Width(), m_nCurWidth);
		//	ATLTRACE("The Button %d left: %d, top: %d, right: %d, bottom: %d\n", i, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
		//}
		if(uStyle & BTNS_SEP)
		{
			if( rcBtn.left > rcBtn.right )
			{
				INT32 nLeft;
				nLeft = rcBtn.left;
				rcBtn.right = rcBtn.left;
				rcBtn.left = nLeft;
			}
			//ATLTRACE("Draw Separater\n");
			if( 0 == IsSplitter( i ) )
			{
				DrawSplitter( DrawDC, rcBtn );
			}
			else
			{
				DrawSeparator( DrawDC, rcBtn );
			}
			continue;
		}

		DEFLATE_RECT(rcBtn, 1, 1);

		//if((rcBtn.right - rcBtn.left) > 0 &&
		//	((rcBtn.left > rcChevron.left && rcBtn.left < rcChevron.right) ||
		//	(rcBtn.right > rcChevron.left && rcBtn.right < rcChevron.right)) )
		//{
		//	continue;
		//}


		if(dwIDCmd == FILLED_BUTTON_ID)
		{
			continue;
		}

		//ATLTRACE("The button %d image is %d\n", i, uImg);
		if(dwIDCmd == CHEVRON_BUTTON_ID)
		{
			//ATLTRACE("Button %d command id is CHEVRON_BUTTON_ID\n", i);
			uImg = -1;//CHEVRON_IMAGE_INDEX;
			*pszText = 0;
		}
		else
		{
			*pszText = _T('\0');
			GetButtonText( dwIDCmd, pszText );
		}

		//if(dwIDCmd == 1038)
		//{
		//	*pszText = _T('\0');
		//	GetButtonText( dwIDCmd, pszText );
		//	ATLTRACE("1038 button test si %s\n", pszText);
		//}
		//pszText = TmpBtnInfo.pszText;
		//if(0 == i)
		//{ 
		//	TBPen.CreatePen( PS_SOLID, 1, dwBtnBkClr);
		//	TBBrush.CreateSolidBrush(dwBtnFrClr);

		//	OldPen = DrawDC.SelectPen(TBPen);
		//	OldBr = DrawDC.SelectBrush(TBBrush);
		//}
		//else
		//{
			//TBPen.Detach();
			//TBBrush.Detach();
			//if(NULL != TBPen.m_hPen)
			//{
			//	TBPen.DeleteObject();
			//}

			//if(NULL != TBBrush.m_hBrush)
			//{
			//	TBBrush.DeleteObject();
			//}
			//TBPen.Attach(::CreatePen( PS_SOLID, 1, dwBtnFrClr));
			//TBBrush.Attach(::CreateSolidBrush(dwBtnBkClr));

			//DrawDC.SelectPen(TBPen);
			//DrawDC.SelectBrush(TBBrush);
			//::DeleteObject(DrawDC.SelectPen(TBPen));
			//::DeleteObject(DrawDC.SelectPen(TBBrush));
		//}

		//if(dwIDCmd == CHEVRON_BUTTON_ID)
		//{
		//	//ATLTRACE("Set the CHEVRON_BUTTON_ID rect %d\n", i);
		//	CRect rcToolbar;
		//	GetWindowRect(&rcToolbar);
		//	//ScreenToClient(&rcToolbar);
		//	rcBtn.right = rcToolbar.Width();
		//	rcBtn.left = rcBtn.right - CHEVRON_BUTTON_WIDTH;
		//}

		//g_XLog.Print(0, "Quote GradientDraw\n");
		//g_XLog.Print(0, "GradientDraw background color is %d the PRESSED_BTN_BK_CLR is %d HOVED_BTN_BK_CLR is %d NORMAL_BTN_BK_CLR is %d command %d index %d\n",
		//	dwBtnBkClr, PRESSED_BTN_BK_CLR, HOVED_BTN_BK_CLR, NORMAL_BTN_BK_CLR, dwIDCmd, i);
		
		//if(dwIDCmd == CHEVRON_BUTTON_ID)
		//{
		//	SelectClipRgn(DrawDC.m_hDC, NULL);
			//CRect rcCliped;
			//INT32 nClipedComplexity;
			//nClipedComplexity = GetClipBox(DrawDC.m_hDC, &rcCliped);
			//ATLTRACE("The cliped region is rc left %d, right %d, top %d, bottom %d, complexity %d\n", rcCliped.left, rcCliped.top, rcCliped.top, rcCliped.bottom, nClipedComplexity);
			//ATLTRACE("NULLREGION %d, SIMPLEREGION %d, COMPLEXREGION %d\n",NULLREGION, SIMPLEREGION, COMPLEXREGION);

			//ATLTRACE("GradientDraw background color is %d the PRESSED_BTN_BK_CLR is %d HOVED_BTN_BK_CLR is %d NORMAL_BTN_BK_CLR is %d command %d index %d\n",
				//dwBtnBkClr, PRESSED_BTN_BK_CLR, HOVED_BTN_BK_CLR, NORMAL_BTN_BK_CLR, dwIDCmd, i );
			//ATLTRACE("GradientDraw index %d rect left %d right %d top %d bottom %d\n", i, rcBtn.left, rcBtn.right, rcBtn.top, rcBtn.bottom);
			//if(i == nHited)
			//{
			//	dwBtnBkClr = PRESSED_BTN_BK_CLR;
			//}
			//else
			//{
			//	dwBtnBkClr = NORMAL_BTN_BK_CLR;
		//}

		if( TBSTATE_ENABLED == ( uState & TBSTATE_ENABLED ) )
		{
			if( rcBtn.right > rcBtn.left && rcBtn.bottom > rcBtn.top )
			{
				if( nHited >= 0 && i == nHited )
				{	
					if( m_bLButtonDown )
					{
						if(rcBtn.right - SPLIT_PART_WIDTH <= ptCur.x  
							&& rcBtn.right >= ptCur.x
							&& uStyle & BTNS_DROPDOWN )
						{
							DrawPressedSplitBtn( DrawDC, rcBtn, szRound);
						}
						else
						{
							DrawPressedBtn( DrawDC, rcBtn, uStyle & BTNS_DROPDOWN, szRound);
						}
					}
					else
					{
						if( uState & TBSTATE_CHECKED)
						{
							DrawHotLightCheckedBtn( DrawDC, rcBtn, uStyle & BTNS_DROPDOWN, szRound);
						}
						else
						{
							DrawHotLightBtn( DrawDC, rcBtn, uStyle & BTNS_DROPDOWN, szRound);
						}
					}
				}
				else if( uState & TBSTATE_CHECKED )
				{
					DrawCheckedBtn( DrawDC, rcBtn, uStyle & BTNS_DROPDOWN, szRound);
				}

			}			//GradientDraw(DrawDC, rcBtn, CSize(2, 2), dwBtnBkClr, 0);
		}

		
		if(uStyle & BTNS_WHOLEDROPDOWN || uStyle & BTNS_DROPDOWN)
		{
			DrawArrow(DrawDC, rcBtn);
		}

		//DrawDC.RoundRect(&rcBtn, CPoint(2, 2));
		//ATLTRACE("The button %d string is %s\n", i, pszText);

		ImageList_GetImageInfo(hImgList, uImg, &TmpImgInfo);
		rcImg.left = TmpImgInfo.rcImage.left;
		rcImg.right = TmpImgInfo.rcImage.right;
		rcImg.top = TmpImgInfo.rcImage.top;
		rcImg.bottom = TmpImgInfo.rcImage.bottom;

#ifdef CENTER_BUTTON_IMAGE
		nHOffset = ((rcBtn.right - rcBtn.left) - (rcImg.right - rcImg.left)) / 2;
		nVOffset = ((rcBtn.bottom - rcBtn.top) - (rcImg.bottom - rcImg.top)) / 2;
#endif

		//ATLTRACE( "The button %d id is %d\n", i, dwIDCmd );
		if(dwIDCmd == CHEVRON_BUTTON_ID)
		{
			nHOffset = (rcBtn.right - rcBtn.left) - (rcImg.right - rcImg.left);
		}
		else
		{
			nHOffset = 4;
		}
		nVOffset = ((rcBtn.bottom - rcBtn.top) - (rcImg.bottom - rcImg.top)) / 2;

		if(dwIDCmd == CHEVRON_BUTTON_ID)
		{
			DrawChevronSign(DrawDC, rcBtn.left + 3, rcBtn.top + 4);
		}
		else if( -1 != uImg && !(uStyle & BTNS_SEP))
		{
			if( i == nHited)
				ImageList_DrawEx(hHotImgList, uImg, DrawDC.m_hDC,  rcBtn.left + nHOffset, rcBtn.top + nVOffset, (rcImg.right - rcImg.left), (rcImg.bottom - rcImg.top), dwBtnBkClr, dwBtnFrClr, ILD_TRANSPARENT);
			else
				ImageList_DrawEx(hImgList, uImg, DrawDC.m_hDC,  rcBtn.left + nHOffset, rcBtn.top + nVOffset, (rcImg.right - rcImg.left), (rcImg.bottom - rcImg.top), dwBtnBkClr, dwBtnFrClr, ILD_TRANSPARENT);
		}

#define X_TOOLBAR_ENABLED_TEXT_CLR RGB( 0, 0, 0 )
#define X_TOOLBAR_DISABLED_TEXT_CLR RGB( 160, 160, 160 )
		//ATLTRACE("The button %d text is %s\n", i, pszText);
		if( CHEVRON_BUTTON_ID != dwIDCmd )
		{
			DWORD dwOldTextClr;
			if( 0 == ( uState & TBSTATE_ENABLED ) )
			{
				dwOldTextClr = DrawDC.SetTextColor( X_TOOLBAR_DISABLED_TEXT_CLR );
				DrawDC.TextOut( rcBtn.left + (rcImg.right - rcImg.left) + 1 + nHOffset, rcBtn.top + 3,pszText, //sizeof( szText ),
#ifdef _UNICODE
					//wcslen(szText)
#else
					//strlen(pszText)
#endif
					-1
					);
				DrawDC.SetTextColor( dwOldTextClr );
			}
			else
			{
				DrawDC.TextOut( rcBtn.left + (rcImg.right - rcImg.left) + 1 + nHOffset, rcBtn.top + 3,pszText, //sizeof( szText ),
#ifdef _UNICODE
					//wcslen(szText)
#else
					//strlen(pszText)
#endif
					-1
					);
			}
		}
	}

	RECT rcDragButton;
	LONG lLineX;
	if(m_bLButtonDown && m_nDragingButton != 0)
	{
		if(nHited >= DRAG_IMAGE_BEGIN_INDEX && nHited <= DRAG_IMAGE_END_INDEX)
		{
			lLineX = -1;
			GetItemRect(nHited, &rcDragButton);
			if((ptCur.x >= rcDragButton.left - 2 &&
				ptCur.x <= rcDragButton.left + 2 ))
			{
				lLineX = rcDragButton.left;
			}
			if((ptCur.x >= rcDragButton.right - 2 &&
				ptCur.x <= rcDragButton.right + 2 ))
			{
				lLineX = rcDragButton.right;
			}
			if(lLineX >= 0)
			{
				DrawMoveDepartLine(DrawDC, lLineX, rcDragButton.top, 4, rcDragButton.bottom - rcDragButton.top);
			}
		}
	}
	if(NULL != OldPen.m_hPen) DrawDC.SelectPen( OldPen );
	if(NULL != OldBr.m_hBrush) DrawDC.SelectBrush( OldBr );
	DrawDC.SelectFont( OldFont );
	DrawDC.SetBkMode(nOldMode);
	//CloseHandle(hImgList);
	//ReleaseDC(DrawDC);

OnPaint_Exit:
	//ImageList_Destroy( hImgList );
	bHandled = TRUE;
	return nRet;
}

//LRESULT XFreeToolBar::OnHookPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	//::MessageBox(NULL,_T("Entern the XFreeToolBar::OnHookPaint Function"), _T("Tip"), 0);
//	bHandled = TRUE;
//	//return 0;
//	static BOOL bFirst = TRUE;
//	if(bFirst)
//	{
//		//__asm int 3;
//		//CalcXFreeBarWidth();
//		//DefWindowProc(uMsg, wParam, lParam);
//		bFirst = FALSE;
//	}
//
//	//CallWindowProc(m_pfnSuperWindowProc, m_hWnd, WM_PAINT, wParam, lParam);
//	//CDC DrawDC = GetDC();
//	CPaintDC DrawDC(m_hWnd);
//	//DrawDC.Rectangle(600, 2, 800, 5);
//	//bHandled = TRUE;
//	//return 0;
//
//	//static UINT i = 0;
//	//DWORD dwBtnBkClr;
//	//i ++;
//	//CRect rcToolbar;
//	//GetWindowRect(&rcToolbar);
//	////ScreenToClient(&rcToolbar);
//	//rcToolbar.right = rcToolbar.Width();
//	//rcToolbar.left = rcToolbar.right - CHEVRON_BUTTON_WIDTH;
//	//rcToolbar.bottom = rcToolbar.Height();
//	//rcToolbar.top = rcToolbar.bottom - 22;
//
//	//if((i % 10 ) < 5)
//	//{
//	//	dwBtnBkClr = PRESSED_BTN_BK_CLR;
//	//}
//	//else
//	//{
//	//	dwBtnBkClr = NORMAL_BTN_BK_CLR;
//	//}
//
//	//GradientDraw(DrawDC, rcToolbar, CSize(2, 2), dwBtnBkClr, 0);
//
//	//bHandled = TRUE;
//	//return 0;
//
//	CPen TBPen;
//	CPenHandle OldPen;
//	CBrush TBBrush;
//	CBrushHandle OldBr;
//	CFontHandle OldFont;
//	INT32 nOldMode;
//	TBBUTTON TBInfo;
//	INT32 nBtnCount;
//	TCHAR pszText[MAX_PATH];
//	//TCHAR *pszText;
//	//CString strText;
//	RECT rcBtn;
//	INT32 nHited;
//	CImageList TempImgList;
//	HIMAGELIST hImgList;
//	hImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETIMAGELIST, 0, 0L);
//	if(NULL == hImgList)
//	{
//		//::MessageBox(NULL, _T("Error"), _T("Get the image list of the toolbar error"), 0);
//		return -3;
//	}
//
//	//TempImgList.FromHandle(hImgList);
//	
//	//TBPen.CreatePen( PS_SOLID, 1, RGB(10,101,10));
//	//TBBrush.CreateSolidBrush(RGB(300,180, 195));
//	//DrawDC = GetDC();
//
//	//OldPen = DrawDC.SelectPen(TBPen);
//	//pOldBr = DrawDC.SelectPen(TBBrush);
//	//OldFont = DrawDC.SelectPen(GetFont());
//
//	IMAGEINFO TmpImgInfo;
//	INT32 nVOffset;
//	INT32 nHOffset;
//	RECT rcImg;
//	CPoint ptCur;
//	if(FALSE == ::GetCursorPos(&ptCur))
//	{
//		//::MessageBox(NULL, _T("Error"), _T("Get current cursor pos error"), 0);
//		return -14;
//	}
//
//	ScreenToClient(&ptCur);
//
//	UINT32 uID;
//	INT32 uImg;
//	UINT32 uStyle;
//	UINT32 uState;
//	DWORD dwBtnBkClr;
//	DWORD dwBtnFrClr;
//	DWORD dwIDCmd;
//
//	//ATLTRACE("SendMessage TB_HITTEST on XFreeToolBar::OnHookPaint\n");
//	//DrawDC = GetDC();
//	nHited = ::SendMessage(m_hWnd, TB_HITTEST, 0, (LPARAM)&ptCur);
//	//ATLTRACE("SendMessage TB_HITTEST on XFreeToolBar::OnHookPaint return %d\n", nHited);
//	//ATLTRACE("Cursor Pos x:%d, y:%d, Hited button is %d\n",ptCur.x, ptCur.y, nHited);
//	nBtnCount = ::SendMessage(m_hWnd, TB_BUTTONCOUNT, 0, 0L);
//	TBBUTTONINFO TmpBtnInfo;
//	OldFont = DrawDC.SelectFont(GetFont());
//	nOldMode = DrawDC.SetBkMode(TRANSPARENT);
//	//CRect rcToolbarTest;
//	//GetWindowRect(&rcToolbarTest);
//	//ScreenToClient(&rcToolbarTest);
//	//DrawDC.Rectangle(&rcToolbarTest);
//	CRect rcChevron;
//	GetWindowRect(&rcChevron);
//	//ScreenToClient(&rcChevron);
//	rcChevron.right = rcChevron.Width();
//	rcChevron.left = rcChevron.right - CHEVRON_BUTTON_WIDTH;
//	rcChevron.bottom = rcChevron.Height();
//	rcChevron.top = 0;
//
//	for( INT32 i = 0; i < nBtnCount; i ++ )
//	{
//		if(GetButton(i, &TBInfo) == FALSE)
//		{
//			continue;
//		}
//		dwIDCmd = TBInfo.idCommand;
//		//ATLTRACE("Command id is %d, Chevron id is %d\n", dwIDCmd, CHEVRON_BUTTON_ID);
//
//		TBInfoGetter(this, dwIDCmd, &TmpBtnInfo, TRUE).style().image().state();//.text();
//		uState = TmpBtnInfo.fsState;
//		if(uState & TBSTATE_HIDDEN)
//		{
//			continue;
//		}
//
//		uStyle = TmpBtnInfo.fsStyle;
//		uImg = TmpBtnInfo.iImage;
//
//
//		//GetButtonInfo(i, uID, uStyle, uImg);
//		GetItemRect( i,&rcBtn);
//		//ATLTRACE("Button rectangle left :%d, top :%d, right :%d, bottom :%d\n", rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
//		
//		//if(i == 0 || i  == 1)
//		//{
//		//	CRect rcToolbar;
//		//	GetWindowRect(&rcToolbar);
//		//	ATLTRACE("The toolbar width %d calc width %d\n", rcToolbar.Width(), m_nCurWidth);
//		//	ATLTRACE("The Button %d left: %d, top: %d, right: %d, bottom: %d\n", i, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
//		//}
//		if(uStyle & BTNS_SEP)
//		{
//			//ATLTRACE("Draw Separater\n");
//			DrawSeparator(DrawDC, rcBtn);
//			continue;
//		}
//
//		DEFLATE_RECT(rcBtn, 1, 1);
//
//		if((rcBtn.right - rcBtn.left) > 0 &&
//			((rcBtn.left > rcChevron.left && rcBtn.left < rcChevron.right) ||
//			(rcBtn.right > rcChevron.left && rcBtn.right < rcChevron.right)) )
//		{
//			continue;
//		}
//
//
//		if(dwIDCmd == FILLED_BUTTON_ID)
//		{
//			continue;
//		}
//
//		//ATLTRACE("The button %d image is %d\n", i, uImg);
//		if(dwIDCmd == CHEVRON_BUTTON_ID)
//		{
//			//ATLTRACE("Button %d command id is CHEVRON_BUTTON_ID\n", i);
//			uImg = CHEVRON_IMAGE_INDEX;
//		}
//		GetButtonText( MapBtnIDFromIndex( i ), pszText);
//		//pszText = TmpBtnInfo.pszText;
//
//		if(i == nHited)
//		{
//			//ATLTRACE("Find the hited button %d command %d %d\n", i, dwIDCmd, CHEVRON_BUTTON_ID );
//			if( TRUE == m_bLButtonDown)
//			{
//				dwBtnBkClr = PRESSED_BTN_BK_CLR;
//				dwBtnFrClr = NORMAL_BTN_FR_CLR;
//			}
//			else
//			{
//				dwBtnBkClr = HOVED_BTN_BK_CLR;
//				dwBtnFrClr = NORMAL_BTN_FR_CLR;
//			}			
//		}
//		else
//		{
//			dwBtnBkClr = NORMAL_BTN_BK_CLR;
//			dwBtnFrClr = NORMAL_BTN_FR_CLR;
//		}
//
//		//if(0 == i)
//		//{
//		//	TBPen.CreatePen( PS_SOLID, 1, dwBtnBkClr);
//		//	TBBrush.CreateSolidBrush(dwBtnFrClr);
//
//		//	OldPen = DrawDC.SelectPen(TBPen);
//		//	OldBr = DrawDC.SelectBrush(TBBrush);
//		//}
//		//else
//		//{
//			//TBPen.Detach();
//			//TBBrush.Detach();
//			//if(NULL != TBPen.m_hPen)
//			//{
//			//	TBPen.DeleteObject();
//			//}
//
//			//if(NULL != TBBrush.m_hBrush)
//			//{
//			//	TBBrush.DeleteObject();
//			//}
//			TBPen.Attach(::CreatePen( PS_SOLID, 1, dwBtnFrClr));
//			TBBrush.Attach(::CreateSolidBrush(dwBtnBkClr));
//
//			DrawDC.SelectPen(TBPen);
//			DrawDC.SelectBrush(TBBrush);
//			//::DeleteObject(DrawDC.SelectPen(TBPen));
//			//::DeleteObject(DrawDC.SelectPen(TBBrush));
//		//}
//
//		//if(dwIDCmd == CHEVRON_BUTTON_ID)
//		//{
//		//	//ATLTRACE("Set the CHEVRON_BUTTON_ID rect %d\n", i);
//		//	CRect rcToolbar;
//		//	GetWindowRect(&rcToolbar);
//		//	//ScreenToClient(&rcToolbar);
//		//	rcBtn.right = rcToolbar.Width();
//		//	rcBtn.left = rcBtn.right - CHEVRON_BUTTON_WIDTH;
//		//}
//
//		//g_XLog.Print(0, "Quote GradientDraw\n");
//		g_XLog.Print(0, "GradientDraw background color is %d the PRESSED_BTN_BK_CLR is %d HOVED_BTN_BK_CLR is %d NORMAL_BTN_BK_CLR is %d command %d index %d\n",
//			dwBtnBkClr, PRESSED_BTN_BK_CLR, HOVED_BTN_BK_CLR, NORMAL_BTN_BK_CLR, dwIDCmd, i);
//		
//		//if(dwIDCmd == CHEVRON_BUTTON_ID)
//		//{
//		//	SelectClipRgn(DrawDC.m_hDC, NULL);
//			//CRect rcCliped;
//			//INT32 nClipedComplexity;
//			//nClipedComplexity = GetClipBox(DrawDC.m_hDC, &rcCliped);
//			//ATLTRACE("The cliped region is rc left %d, right %d, top %d, bottom %d, complexity %d\n", rcCliped.left, rcCliped.top, rcCliped.top, rcCliped.bottom, nClipedComplexity);
//			//ATLTRACE("NULLREGION %d, SIMPLEREGION %d, COMPLEXREGION %d\n",NULLREGION, SIMPLEREGION, COMPLEXREGION);
//
//			//ATLTRACE("GradientDraw background color is %d the PRESSED_BTN_BK_CLR is %d HOVED_BTN_BK_CLR is %d NORMAL_BTN_BK_CLR is %d command %d index %d\n",
//				//dwBtnBkClr, PRESSED_BTN_BK_CLR, HOVED_BTN_BK_CLR, NORMAL_BTN_BK_CLR, dwIDCmd, i );
//			//ATLTRACE("GradientDraw index %d rect left %d right %d top %d bottom %d\n", i, rcBtn.left, rcBtn.right, rcBtn.top, rcBtn.bottom);
//			//if(i == nHited)
//			//{
//			//	dwBtnBkClr = PRESSED_BTN_BK_CLR;
//			//}
//			//else
//			//{
//			//	dwBtnBkClr = NORMAL_BTN_BK_CLR;
//			//}
//
//		if( rcBtn.right > rcBtn.left && rcBtn.bottom > rcBtn.top )
//			GradientDraw(DrawDC, rcBtn, CSize(2, 2), dwBtnBkClr, 0);
//		//}
//		
//		
//		if(uStyle & BTNS_WHOLEDROPDOWN)
//		{
//			DrawArrow(DrawDC, rcBtn);
//		}
//		if(uStyle & BTNS_DROPDOWN)
//		{
//			DrawSplitPart(DrawDC, rcBtn);
//		}
//
//		//DrawDC.RoundRect(&rcBtn, CPoint(2, 2));
//		//ATLTRACE("The button %d string is %s\n", i, pszText);
//
//		ImageList_GetImageInfo(hImgList, uImg, &TmpImgInfo);
//		rcImg.left = TmpImgInfo.rcImage.left;
//		rcImg.right = TmpImgInfo.rcImage.right;
//		rcImg.top = TmpImgInfo.rcImage.top;
//		rcImg.bottom = TmpImgInfo.rcImage.bottom;
//
//#ifdef CENTER_BUTTON_IMAGE
//		nHOffset = ((rcBtn.right - rcBtn.left) - (rcImg.right - rcImg.left)) / 2;
//		nVOffset = ((rcBtn.bottom - rcBtn.top) - (rcImg.bottom - rcImg.top)) / 2;
//#endif
//		if(dwIDCmd == CHEVRON_BUTTON_ID)
//		{
//			nHOffset = (rcBtn.right - rcBtn.left) - (rcImg.right - rcImg.left);
//		}
//		else
//		{
//			nHOffset = 2;
//		}
//		nVOffset = ((rcBtn.bottom - rcBtn.top) - (rcImg.bottom - rcImg.top)) / 2;
//
//		if( -1 != uImg && !(uStyle & BTNS_SEP))
//		{
//			ImageList_DrawEx(hImgList, uImg, DrawDC.m_hDC,  rcBtn.left + nHOffset, rcBtn.top + nVOffset, (rcImg.right - rcImg.left), (rcImg.bottom - rcImg.top), dwBtnBkClr, dwBtnFrClr, ILD_TRANSPARENT);
//		}
//
//		ATLTRACE("The button %d text is %s\n", i, pszText);
//		if( CHEVRON_BUTTON_ID != dwIDCmd )
//			DrawDC.TextOut( rcBtn.left + (rcImg.right - rcImg.left) + 1, rcBtn.top + 3,pszText, //sizeof( szText ),
//#ifdef _UNICODE
//			//wcslen(szText)
//#else
//			//strlen(pszText)
//#endif
//			-1
//		);
//	}
//
//	if(NULL != OldPen.m_hPen) DrawDC.SelectPen( OldPen );
//	if(NULL != OldBr.m_hBrush) DrawDC.SelectBrush( OldBr );
//	DrawDC.SelectFont( OldFont );
//	DrawDC.SetBkMode(nOldMode);
//	//CloseHandle(hImgList);
//	//ReleaseDC(DrawDC);
//	bHandled = TRUE;
//	return 0;
//}

LRESULT XFreeToolBar::OnHookLButtonDown(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	
	if(m_bSplitFocused)
	{
		m_bLBtnDown = true;
		SetCapture();
	}
	m_bLButtonDown = TRUE;

	DWORD dwPoint;
	dwPoint = ::GetMessagePos();
	POINT ptCur = { GET_X_LPARAM(dwPoint), GET_Y_LPARAM(dwPoint) };
	POINT ptDrag;

	ptCur.x = GET_X_LPARAM(dwPoint);
	ptCur.y = GET_Y_LPARAM(dwPoint);
	ptDrag = ptCur;

	ScreenToClient(&ptCur);
	UINT32 nHit = HitTest(&ptCur);
	if(g_pXConfig && nHit>=0 && nHit<g_pXConfig->m_BtnConfs.size())
	{
		bHandled = g_pXConfig->m_BtnConfs[nHit]->StopMouseEvents();
	}

	RECT rcItem;
	INT32 nRet;
	HIMAGELIST hImgList;
	TBBUTTONINFO TBInfo;

	if(nHit >= DRAG_IMAGE_BEGIN_INDEX && nHit <= DRAG_IMAGE_END_INDEX)
	{
		nRet = GetItemRect(nHit, &rcItem);
		if(nRet == 0)
		{
			ATLTRACE("Get Toolbar Item Rect Error!\n");
		}
		else
		{
			hImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETIMAGELIST, 0, 0L);
			TBInfoGetter(this, MapBtnIDFromIndex(nHit), &TBInfo, TRUE).image();
			m_nDragingImage = TBInfo.iImage;
			ClientToScreen(&rcItem);
			m_nDragingButton = nHit;
			//nRet = StartDragging(NULL, ptDrag, rcItem, m_nDragingImage, hImgList);
		}
	}
	return S_OK;
}

INT32 XFreeToolBar::MapBtnIDFromIndex( INT32 nIndex )
{
	TBBUTTON TBBtn;
	LRESULT lRet;
	lRet = ::SendMessage( m_hWnd, TB_GETBUTTON, (WPARAM)nIndex, (LPARAM)&TBBtn );
	if(lRet != FALSE)
	{
		return TBBtn.idCommand;
	}
	else
	{
		ATLASSERT(FALSE);
		return -1;
	}
}

INT32 XFreeToolBar::MapBtnIndexFromID( WORD wBtnID )
{
	LRESULT lRet;
	lRet = ::SendMessage( m_hWnd, TB_COMMANDTOINDEX, wBtnID, 0 );
	return ( INT32 )lRet;
}

INT32 XFreeToolBar::ReplaceButtonText( INT32 nButtonSrc, INT32 nButtonDst)
{
	INT32 nRet;
	TBBUTTONINFO TBInfoFirst;
	TBBUTTONINFO TBInfoSecond;

	TBBUTTONINFO *pTBInfoGet;
	TBBUTTONINFO *pTBInfoSet;

	CHAR pszFirstText[_MAX_PATH];
	CHAR pszSecondText[_MAX_PATH];
	TBBUTTONINFO *pTBInfoTemp;
	CHAR *pszTemp;

	CHAR *pszSet;
	CHAR *pszGet;

	if(nButtonDst == nButtonSrc)
	{
		return 0;
	}

	pTBInfoSet = &TBInfoFirst;
	pTBInfoGet = &TBInfoSecond;
	pszGet = pszFirstText;
	pszSet = pszSecondText;

	TBInfoGetter(this, MapBtnIDFromIndex( nButtonSrc ), pTBInfoSet, TRUE, pszSet, _MAX_PATH).text();
	TBInfoGetter(this, MapBtnIDFromIndex( nButtonDst ), pTBInfoGet, TRUE, pszGet, _MAX_PATH).text();
	//TBInfoSetter(this, MapBtnIDFromIndex( nButtonDst ), TRUE).text(pTBInfoSet->pszText);

	pTBInfoTemp = pTBInfoSet;
	pTBInfoSet = pTBInfoGet;
	pTBInfoGet = pTBInfoTemp;

	pszTemp = pszSet;
	pszSet = pszGet;
	pszGet = pszTemp;

	if(nButtonDst > nButtonSrc)
	{
		for(INT32 i = nButtonDst - 1; i >= nButtonSrc; i--)
		{
			TBInfoGetter(this, MapBtnIDFromIndex( i ), pTBInfoGet, TRUE, pszGet, _MAX_PATH).text();
			TBInfoSetter(this, MapBtnIDFromIndex( i ), TRUE).text(pTBInfoSet->pszText);

			pTBInfoTemp = pTBInfoSet;
			pTBInfoSet = pTBInfoGet;
			pTBInfoGet = pTBInfoTemp;

			pszTemp = pszSet;
			pszSet = pszGet;
			pszGet = pszTemp;
		}
	}
	else
	{
		for(INT32 i = nButtonDst + 1; i <= nButtonSrc; i++)
		{
			TBInfoGetter(this, MapBtnIDFromIndex( i ), pTBInfoGet, TRUE, pszGet, _MAX_PATH).text();
			//TBInfoSetter(this, MapBtnIDFromIndex( i ), TRUE).text(pTBInfoSet->pszText);

			pTBInfoTemp = pTBInfoSet;
			pTBInfoSet = pTBInfoGet;
			pTBInfoGet = pTBInfoTemp;

			pszTemp = pszSet;
			pszSet = pszGet;
			pszGet = pszTemp;
		}
	}
	return 0;
}

LRESULT XFreeToolBar::OnHookLButtonUp(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	DWORD dwPoint;
	POINT ptCur;
	POINT ptDrag;
	RECT rcItem;
	INT32 nRet;
	UINT32 nHit;
	HIMAGELIST hImgList;

	bHandled = FALSE;

	m_bLButtonDown = FALSE;
	m_bLBtnDown = FALSE;
	m_bSplitFocused = FALSE;

	dwPoint = ::GetMessagePos();
	ptCur.x = GET_X_LPARAM(dwPoint);
	ptCur.y = GET_Y_LPARAM(dwPoint);
	ptDrag = ptCur;
	ScreenToClient(&ptCur);

	nHit = HitTest(&ptCur);
	nRet = GetItemRect(nHit, &rcItem);

	if(g_pXConfig && nHit>=0 && nHit<g_pXConfig->m_BtnConfs.size())
	{
		bHandled = g_pXConfig->m_BtnConfs[nHit]->StopMouseEvents();
	}

	if(nHit >= DRAG_IMAGE_BEGIN_INDEX 
		&& nHit <= DRAG_IMAGE_END_INDEX 
		&& m_nDragingButton >= DRAG_IMAGE_BEGIN_INDEX 
		&& m_nDragingButton <= DRAG_IMAGE_END_INDEX 
		&& nHit != m_nDragingButton 
		)
	{
		ATLTRACE("Before Move Source Index: %d, Source Command ID :%d,Destination Index: %d, Destination Command ID: %d\n",
			m_nDragingButton, MapBtnIDFromIndex(m_nDragingButton), nHit, MapBtnIDFromIndex(nHit));
		//::SendMessage(m_hWnd, TB_CUSTOMIZE, m_nDragingButton, );
		
		LRESULT lRes = ::SendMessage(m_hWnd, TB_MOVEBUTTON, m_nDragingButton, nHit);
		ATLTRACE("End Move Source Index: %d, Source Command ID :%d,Destination Index: %d, Destination Command ID: %d\n",
			m_nDragingButton, MapBtnIDFromIndex(m_nDragingButton), nHit, MapBtnIDFromIndex(nHit));
		//ReplaceButtonText( m_nDragingButton, nHit);
		//UpdateWindow();
		//Invalidate(0);
	}

	hImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETIMAGELIST, 0, 0L);
	//ClientToScreen(&rcItem);
	//StopDragging(NULL, hImgList, rcItem, m_nDragingImage, ptDrag);
	//TBInfoSetter(this, MapBtnIDFromIndex(nHit), TRUE).image(m_nDragingImage);
	//TBInfoSetter(this, MapBtnIDFromIndex(m_nDragingButton), TRUE).image(TBInfoDst.iImage);
	ReleaseCapture();
	Invalidate(0);
	return S_OK;
}

#ifndef NO_OPTIONS_PAGE
// on sel XChange event in option page
void XFreeToolBar::OnSelChange( DISPID id, VARIANT* pVarResult )
{
   HRESULT hr;
   IHTMLDocument2Ptr pHtmlDoc = GetHtmlDocument();
   if( pHtmlDoc != NULL )
   {
     IHTMLWindow2Ptr  pWindow;
     IHTMLEventObjPtr pEvent;
     hr = pHtmlDoc->get_parentWindow(&pWindow);
     ATLASSERT( SUCCEEDED( hr ) );
     hr = pWindow->get_event(&pEvent);

     IHTMLElementPtr p;
     hr = pEvent->get_srcElement(&p);

     CComBSTR  id;
     p->get_id(&id);
                        
     for( INT32 i = 0; i < g_pXConfig->m_Settings.m_Settings.size(); i++ )
       g_pXConfig->m_Settings.m_Settings[i]->OnSelChange(p,(const TCHAR*)_bstr_t(id));
  }
}

#define XERROR_BTN_INDEX_OVERFLOW -231
INT32 XFreeToolBar::GetXBtnInfo( INT32 nIndex, XBTNINFO **ppInfo )
{
	ATLASSERT( nIndex >= 0 );
	TBBUTTON BtnData;
	INT32 nBtnCount;

	*ppInfo = NULL;
	nBtnCount = GetButtonCount();
	if( nIndex >= nBtnCount )
	{
		return XERROR_BTN_INDEX_OVERFLOW;
	}

	if( 0 == GetButton( nIndex, &BtnData ) )
	{
		return XERROR_GET_BTN_DATA;
	}

	if( NULL == BtnData.dwData )
	{
		return XERROR_XBUTTON_DATA_EXT_IS_NULL;
	}

	*ppInfo = ( XBTNINFO* )BtnData.dwData;
	return XSUCCESS;
}

INT32 XFreeToolBar::GetBtnRealWidth( INT32 nIndex )
{
	ATLASSERT( nIndex >= 0 );
	INT32 nButtonWidth = 0;
	INT32 nBtnCount;
	INT32 nRet;

	XBTNINFO *pXBtnInfo;

	nRet = GetXBtnInfo( nIndex, &pXBtnInfo );
	if( 0 > nRet )
	{
		return nRet;
	}

	return pXBtnInfo->dwRealWidth;
}

XToolbarItem* XFreeToolBar::GetBtnConf( INT32 nIndex )
{
	ATLASSERT( nIndex >= 0 );
	INT32 nButtonWidth = 0;
	INT32 nBtnCount;
	INT32 nRet;

	XBTNINFO *pXBtnInfo;

	nRet = GetXBtnInfo( nIndex, &pXBtnInfo );
	if( 0 > nRet )
	{
		return NULL;
	}

	ATLASSERT( NULL != pXBtnInfo );
	return ( XToolbarItem* )pXBtnInfo->lpExtension;
}

VOID XFreeToolBar::SetBtnWidthInfo( INT32 nIndex, INT32 nButtonWidth)
{
	INT32 nNormalButtonCount;
	nNormalButtonCount = (INT32)g_pXConfig->m_BtnConfs.size();

	if( nIndex >= CHEVRON_BUTTON_BEGIN_INDEX)
	{
		ATLASSERT(FALSE);
	}
	else if( nIndex < nNormalButtonCount)
		 m_pXBtnInfos[nIndex].dwRealWidth = nButtonWidth;
	else
		m_pXBtnInfos[ HILIGHT_BUTTON_INDEX(nIndex - nNormalButtonCount) ].dwRealWidth = nButtonWidth;
	return;
}

INT32 XFreeToolBar::IsRealHideBtn( INT32 nIndex )
{
	INT32 nRet;
	XBTNINFO *pXBtnInfo;

	nRet = GetXBtnInfo( nIndex, &pXBtnInfo );
	if( nRet < 0 )
	{
		return nRet;
	}
	ATLASSERT( NULL != pXBtnInfo );

	return pXBtnInfo->bRealHide;
}

INT32 XFreeToolBar::DumpAllButtonInfo()
{
	INT32 nBtnCount;
	nBtnCount = GetButtonCount();

	TBBUTTONINFO BtnInfo;
	TBBUTTON TBBtn;

	INT32 nAllBtnsWidth = 0;
	TCHAR szBtnTitle[ MAX_PATH ];
	INT32 i;

	for( i = 0; i < nBtnCount; i++)
	{
		*szBtnTitle = '\0';
		memset(&BtnInfo, 0, sizeof(BtnInfo));
		BtnInfo.cbSize = sizeof(BtnInfo);
		BtnInfo.dwMask = TBIF_STATE | TBIF_SIZE | TBIF_TEXT;
		BtnInfo.pszText = szBtnTitle;
		BtnInfo.cchText = MAX_PATH;
		INT32 ret = GetButtonInfo( WM_USER + i, &BtnInfo );
		GetButton( i, &TBBtn);
		//if(FALSE != ret)
		XLOG( ( 0, __FUNCTION__ "The button %d width is %d title is %s title len is %d\n", i, BtnInfo.cx, BtnInfo.pszText, strlen( BtnInfo.pszText ) ) );
		XLOG( ( 0, __FUNCTION__ "The button %d string is %d command is %d bitmap is %d state is %d style is %d\n", i, TBBtn.iString, TBBtn.idCommand, TBBtn.iBitmap, TBBtn.fsState, TBBtn.fsStyle ) );

		nAllBtnsWidth += BtnInfo.cx;
	}

	return 0;
}

INT32 XFreeToolBar::CalcFixedLeftSideBtnWidth()
{
	INT32 nRet;
	INT32 nBtnCount;
	XBTNINFO *pXBtnInfo; 

	nBtnCount = GetButtonCount();

	nRet = 0;
	for( INT32 i = 0; i < nBtnCount; i++ )
	{
		if( 0 > GetXBtnInfo( i, &pXBtnInfo ) )
		{
			nRet = -1;
			break;
		}
		else if( pXBtnInfo->dwStyle == FIXED_LEFT_SIDE_BTN )
		{
			nRet += pXBtnInfo->dwRealWidth;
		}
	}
	return nRet;
}

INT32 XFreeToolBar::CalcFixedRightSideBtnWidth()
{
	INT32 nRet;
	INT32 nBtnCount;
	XBTNINFO *pXBtnInfo; 

	nBtnCount = GetButtonCount();

	nRet = 0;
	for( INT32 i = 0; i < nBtnCount; i++ )
	{
		if( 0 > GetXBtnInfo( i, &pXBtnInfo ) )
		{
			nRet = -1;
			break;
		}
		else if( pXBtnInfo->dwStyle == FIXED_RIGHT_SIDE_BTN )
		{
			nRet += pXBtnInfo->dwRealWidth;
		}
	}
	return nRet;
}

INT32 XFreeToolBar::IsSplitter( INT32 nIndex )
{
	if( g_pXConfig->m_BtnConfs[ nIndex ]->GetToolbarItemType() == XTYPE_SPLITTER )
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

INT32 XFreeToolBar::IsFixedButton( INT32 nIndex )
{
	ATLASSERT( 0 <= nIndex );
	INT32 nRet;
	XBTNINFO *pXBtnInfo;
	
	nRet = GetXBtnInfo( nIndex, &pXBtnInfo );
	if( nRet < 0 )
	{
		return nRet;
	}

	ATLASSERT( NULL != pXBtnInfo );
	return pXBtnInfo->dwStyle & FIXED_RIGHT_SIDE_BTN;
}

INT32 XFreeToolBar::DepartToolBar()
{
	//return 0;
	INT32 nRet;
	INT32 nBtnCount;
	CRect rcToolBar;
	INT32 nLeftBtnWidth;
	INT32 nRightBtnWidth;
	TBBUTTON TmpBtn;
	TBBUTTON *pRemainBtn;
	INT32 nIndex;

	nRet = XSUCCESS;

	ATLASSERT( TRUE == m_bToolbarInited );

	if( FALSE == GetClientRect( &rcToolBar ) )
	{
		nRet = -26;
		goto Exit_DepartToolbar;
	}

	m_nChevBtnIndex = -1;
	nLeftBtnWidth = 0;

	nRightBtnWidth = CalcFixedRightSideBtnWidth();
	ATLASSERT( nRightBtnWidth >= 0 );

	nBtnCount = GetButtonCount();

	for( nIndex = 0; nIndex < nBtnCount; nIndex++ )
	{
		if( FALSE == IsRealHideBtn( nIndex ) )
		{
			if( nLeftBtnWidth + GetBtnRealWidth( nIndex ) > rcToolBar.Width() - CHEVRON_BUTTON_WIDTH - TOOLBAR_RESERVE_SPACE - nRightBtnWidth )
			{
				if( m_nChevBtnIndex == -1 )
				{
					m_nChevBtnIndex = nIndex;
					nLeftBtnWidth += GetBtnRealWidth( nIndex );
				}

				RealHideBtn( nIndex, TRUE, -1);
			}
			else
			{
				nLeftBtnWidth += GetBtnRealWidth( nIndex );
				RealHideBtn( nIndex, FALSE, -1);
			}
		}
	}

	if( m_nChevBtnIndex != -1 )
		nLeftBtnWidth -= GetBtnRealWidth( m_nChevBtnIndex );
/* 
#define TBSTATE_CHECKED         0x01
#define TBSTATE_PRESSED         0x02
#define TBSTATE_ENABLED         0x04
#define TBSTATE_HIDDEN          0x08
#define TBSTATE_INDETERMINATE   0x10
#define TBSTATE_WRAP            0x20
#if (_WIN32_IE >= 0x0300)
#define TBSTATE_ELLIPSES        0x40
#endif
#if (_WIN32_IE >= 0x0400)
#define TBSTATE_MARKED          0x80
*/

	INT32 nChevBtnIndex;
	INT32 nFillBtnIndex;
	nChevBtnIndex = MapBtnIndexFromID( CHEVRON_BUTTON_ID );
	nFillBtnIndex = MapBtnIndexFromID( FILLED_BUTTON_ID );
	if(m_nChevBtnIndex != -1)
	{
		RealHideBtn( nFillBtnIndex, FALSE, -1);
		RealHideBtn( nChevBtnIndex, FALSE, -1);

		TBInfoSetter( this, FILLED_BUTTON_ID, TRUE ).size( rcToolBar.Width() - nLeftBtnWidth - CHEVRON_BUTTON_WIDTH - nRightBtnWidth - TOOLBAR_RESERVE_SPACE);
		TBInfoSetter( this, CHEVRON_BUTTON_ID, TRUE ).size( CHEVRON_BUTTON_WIDTH );
	}
	else
	{
		RealHideBtn( nFillBtnIndex, FALSE, -1);
		TBInfoSetter( this, FILLED_BUTTON_ID, TRUE ).size( rcToolBar.Width() - nLeftBtnWidth - nRightBtnWidth - TOOLBAR_RESERVE_SPACE );
		RealHideBtn( nChevBtnIndex, TRUE, -1);
	}

	ATLASSERT(  m_nChevBtnIndex < nBtnCount );

	DumpAllButtonInfo();

Exit_DepartToolbar:
	return nRet;
}

VOID XFreeToolBar::OnSize(UINT wParam, CSize size)
{
#ifdef _SERVAGE
	if(g_pXConfig && g_pXConfig->m_BtnConfs.size() && size.cx)
	{
		((\Page*)( g_pXConfig->m_BtnConfs[0] ) )->m_dwWidth = size.cx;
		OnOptionChange();
	}
#endif

	SetMsgHandled(FALSE);
}

LRESULT XFreeToolBar::OnCommand(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(hWndCtl) // fix for combo box - don't know how to make it better
	{
		bHandled = false;
		return S_OK;
	}

	if( IMenuItem* item = g_pXConfig->GetMenuItemOnButton( wID ) )
	{
		if( Command* c = g_pXConfig->FindCommandConf( item->m_strCmd ) )
			c->apply(this);
	}
	return S_OK;
}

INT32 XFreeToolBar::OnHtmlMouseBtnDown( LPVOID pParam )
{
	UpdateAutoWords();
	return 0;
}
#endif

//typedef struct tagNMREBARCHEVRON
//{
//    NMHDR hdr;
//    UINT uBand;
//    UINT wID;
//    LPARAM lParam;
//    RECT rc;
//    LPARAM lParamNM;
//} NMREBARCHEVRON, *LPNMREBARCHEVRON;

LRESULT XFreeToolBar::OnButton( WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	INT32 nBtnIndex;
	
	if( FALSE == m_bToolbarInited )
	{
		return XSUCCESS;
	}

	if( ( hWndCtl != m_hWnd && hWndCtl != NULL ) )
	{
		bHandled = FALSE;
		return XERROR_NOT_XBTN_EVENT;
	}
	else
	{
		nBtnIndex = MapBtnIndexFromID( wID );
		if(m_nDragingButton != 0 && m_nDragingButton != (INT32)nBtnIndex)
		{
			m_nDragingButton = 0;
			bHandled = FALSE;
			return XSUCCESS;
		}
	}

	//DWORD dwPt;
	//dwPt = ::GetMessagePos();
	//POINT ptCur;
	//ptCur.x  = GET_X_LPARAM( dwPt );
	//ptCur.y = GET_Y_LPARAM( dwPt );

	//CreateXFreeWnd( _AtlBaseModule.GetModuleInstance(), ptCur.x, ptCur.y, 100, 300, m_hWnd, 0, 0 );
	if( wID == CHEVRON_BUTTON_ID )
	{
		NMREBARCHEVRON NMChev;
		NMChev.wID = m_pToolBarHost->GetBandID();
		NMChev.rc.left = 0;
		NMChev.rc.right = 32;
		NMChev.rc.top = 0;
		NMChev.rc.bottom = 200;
		BOOL bHandled = TRUE;
		ATLTRACE( "Chevron button clieked\n" );
		OnChevronPushed( 0, ( LPNMHDR )&NMChev, bHandled );
		return XSUCCESS;
	}

	XToolbarItem *pBtnConf;
	if( XSUCCESS == GetBtnConfigInfo( nBtnIndex, &pBtnConf ) )
	//if( nIndex < g_pXConfig->m_BtnConfs.size() ) //Transact the all command of buttons 
	{
		m_nLastPresedButton = nBtnIndex;
		if( Command* command = pBtnConf->m_Command)
		{
			if( IsButtonChecked( wID ) != FALSE)
				command->applyPresed( this );
			else
				command->apply( this );
		}
		pBtnConf->OnXFreeBarCommand( this );
	}
	else
	{
#ifndef NO_FIND_BUTTTONS
		//If have the find button then the button title which will is the finded keyword, do two step to prepare searching:1.get the button title. 2.remove the title quote signs.
		TCHAR szValue[1024];
		GetButtonText(wID, szValue);
		XString strButtonTitle = szValue;

		INT32 nIndex;
		while( ( nIndex = strButtonTitle.find( _T('\'') ) )  != XString::npos )
			strButtonTitle.erase( nIndex, 1 );

		replace( strButtonTitle, _T( "\"" ), XString( _T( "" ) ) );
		m_pHtmlSearch->SearchTextInHTML( strButtonTitle.c_str() );
		m_pHtmlFunc->MoveFocusToHTML();
#endif
	}

	if( TRUE == g_ToolWillReload )
	{
		BuildToolBar();
	}
	g_ToolWillReload = FALSE;
	return XSUCCESS;
}
//End Message Handler

LRESULT CALLBACK HookIEFrame(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;
	BOOL bHandled = TRUE;
	switch(uMsg)
	{
	case WM_SIZE:
		lResult = CallWindowProc(g_fSuperIEFrameProc, hWnd, uMsg, wParam, lParam);
		//g_pXFreeBar->OnHookIEFrameSize(hWnd, uMsg, wParam, lParam, bHandled);
		break;
	default:
		if(NULL != g_fSuperIEFrameProc)
		{
			lResult = CallWindowProc(g_fSuperIEFrameProc, hWnd, uMsg, wParam, lParam);
		}
		break;
	}
	return lResult;
}


BOOL XFreeToolBar::SetBandRebar()
{
	HWND hWnd(NULL);
	m_pToolBarHost->m_pWebBrowser->get_HWND((long*)&hWnd);

	if(hWnd == NULL)
		return FALSE;

	m_ctrlRebar.m_hWnd = FindRebar(hWnd);
	if(m_ctrlRebar.m_hWnd == NULL)
		return FALSE;

	//g_fSuperIEFrameProc = (WNDPROC)::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)HookIEFrame);
	//m_WebBrowserContainer.SubclassWindow(hWnd);
	m_RebarContainer.SubclassWindow(m_ctrlRebar);

	return TRUE;
}

BOOL XFreeToolBar::UnsubclassRebar()
{
	//LONG_PTR pfnWndProc = ::GetWindowLongPtr(m_RebarContainer.m_hWnd, GWLP_WNDPROC);

	ATLASSERT( m_RebarContainer.m_hWnd != NULL /*&& m_WebBrowserContainer.m_hWnd != NULL */);
	//if(m_RebarContainer.m_pfnSuperWindowProc != ::DefWindowProc && ::GetWindowLongPtr(m_RebarContainer.m_hWnd, GWLP_WNDPROC) == pfnWndProc)
		::SetWindowLongPtr(m_RebarContainer.m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_RebarContainer.m_pfnSuperWindowProc);
	// clear out window handle
	m_RebarContainer.m_hWnd = NULL;

	//pfnWndProc = ::GetWindowLongPtr(m_WebBrowserContainer.m_hWnd, GWLP_WNDPROC);

	//if(m_WebBrowserContainer.m_pfnSuperWindowProc != ::DefWindowProc && ::GetWindowLongPtr(m_WebBrowserContainer.m_hWnd, GWLP_WNDPROC) == pfnWndProc)
		//::SetWindowLongPtr(m_WebBrowserContainer.m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_WebBrowserContainer.m_pfnSuperWindowProc);
	// clear out window handle
	//m_WebBrowserContainer.m_hWnd = NULL;
	return 0;
}

void XFreeToolBar::HideBtnIfNeeded(INT32 nIndexOrID,const XString& strCmd, BOOL bIsIndex )
{
	INT32 nBtnID;

	if( TRUE == bIsIndex )
	{
		nBtnID = MapBtnIDFromIndex( nIndexOrID );
	}
	else
	{
		nBtnID = nIndexOrID;
	}

	if( 0 > nBtnID )
	{
		return;
	}

	if(!strCmd.empty())
		HideButton( nBtnID, !isGetValue( strCmd ));
}

INT32 XFreeToolBar::HideHilightBtn( INT32 nIndex, BOOL bHide, INT32 nRealHide)
{
	return RealHideBtn( nIndex, bHide, nRealHide );
}

INT32 XFreeToolBar::SetHilightBtnText(INT32 nIndex, TCHAR *lpszText )
{
	ATLTRACE("Set the key word button text is %s\n", lpszText );
	INT32 nBtnID;
	nBtnID = MapBtnIDFromIndex( nIndex );
	ATLASSERT( 0 <= nBtnID );

	TBInfoSetter( this, nBtnID, TRUE ).text( lpszText );
	return 0;
}

// add / remove auto generated "find" words
void XFreeToolBar::UpdateAutoWords()
{
	//return;
	XString strAllKeyWords;
	INT32 nKeyWordCount;

	nKeyWordCount = m_VectorAllFindKeyWord.size();
	//Enumerate all edits on the toolbar
	for( IBandEdits Iter( g_pXConfig->m_vecCombos.size(), g_pXConfig->m_vecCombos.begin() ); Iter; Iter++ )
		strAllKeyWords += Iter->GetText() + " ";

	// Get the text in the edit box
	//std::vector<XString> vecKeyWords;
	replaceSpace( strAllKeyWords, m_VectorAllFindKeyWord );

	EnableHighlight(m_VectorAllFindKeyWord.size());

#ifdef FIND_MENU
#endif
#ifndef NO_FIND_BUTTTONS
	if(!g_pXConfig->m_Settings.m_highlight.m_colors.size())
		return;

	//DeleteChevronBtn();

	INT32 nNormalButtonCount = g_pXConfig->m_BtnConfs.size(); //Calc the number of additional buttons

	//INT32 nAllButtonCount = GetButtonCount();

#define CHEVRON_FUNC_BUTTON_COUNT 2
	//DeleteXBtns( nAllButtonCount - 1 - CHEVRON_FUNC_BUTTON_COUNT );
	//nAllButtonCount -= 1;

	INT32 nNewAdded =  m_VectorAllFindKeyWord.size() - nKeyWordCount;//(nAllButtonCount - nNormalButtonCount);

	for(UINT32 uIndex = nNormalButtonCount + nKeyWordCount - 1; uIndex  >= nNormalButtonCount + m_VectorAllFindKeyWord.size(); uIndex --)
	{
		ATLTRACE(_T("Hide Highlight Button %d\n"), uIndex);
		RealHideBtn( uIndex, TRUE, TRUE);
	}

	//INT32 nBtnCount = m_uBtnCount;
	if(nNewAdded > 0)
	{
		for( INT32 i = 0; i < nNewAdded; i++ )
		{
			//INT32 com = INIT_BTN_ID( nAllButtonCount + i );
			//m_pXBtnInfos[ nBtnCount + i ].pBtnData->iBitmap = (nAllButtonCount - nNormalButtonCount + i) % g_pXConfig->m_Settings.m_highlight.m_colors.size();
			//m_pXBtnInfos[ nBtnCount + i ].pBtnData->idCommand = com;
			//m_pXBtnInfos[ nBtnCount + i ].pBtnData->fsState = TBSTATE_ENABLED;
			//m_pXBtnInfos[ nBtnCount + i ].pBtnData->fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
			//m_pXBtnInfos[ nBtnCount + i ].pBtnData->dwData = 0;
			//m_pXBtnInfos[ nBtnCount + i ].pBtnData->iString = 0;
			ATLTRACE( _T("Enable Highlight Button %d\n"), nNormalButtonCount + i );
			RealHideBtn( nNormalButtonCount + i ,FALSE, FALSE );
		}
		//AddXBtns(nNewAdded, &m_pXBtnInfos[ nBtnCount ]);
	} //Add buttons with the input text, all number of buttons is nNormalButtonCount of XToolbarItem + nNormalButtonCount of key words.
	//AddChevronBtn();

	//INT32 nCount = GetButtonCount();


	//ATLTRACE("Add the chevron button: %d\n", nNewAdded + nAllButtonCount );

	BOOL bHide = !isGetValue( _T( "ShowFindButtons" ) ) || m_VectorAllFindKeyWord.size() == 0;

	for( UINT32 i = 0; i < m_VectorAllFindKeyWord.size(); i ++ )
	{
		SetHilightBtnText( nNormalButtonCount + i , const_cast<TCHAR*>( m_VectorAllFindKeyWord[i].c_str() ));
		RealHideBtn( nNormalButtonCount + i, bHide, bHide); //Second parameter is the idCommand member of TBBUTTON
	}

	//HideButton( g_pXConfig->MapBtnConfIndexFromCmd(_T("ShowFindButtons")), bHide ); //Find the command of the XToolbarItem that is "ShowFindButtons".

	CalcXFreeBarWidth();
	SetBtnsWidth( nNormalButtonCount, nNormalButtonCount + m_VectorAllFindKeyWord.size() );
	DepartToolBar();
#endif
}

// process search (after Enter press or "Go" button)
void XFreeToolBar::RunSearch()
{
#ifdef SELECT_SEARCH_FROM_MENU
	if(\Commands::Command* c  = g_pXConfig->FindCommandConf(gAppendCommand+"Home"))
		c->apply(this);
	return;
#endif

	INT32 idxCur = 0;
	INT32 idx = XString(getValue(_T("SearchUsing"),_T("0")))[0] - '0';

#ifdef SELECT_SEARCH_FROM_COMBO
	for(XML::vectorDel<Command*>::iterator c=g_pXConfig->m_commands.m_commands.begin();c!=g_pXConfig->m_commands.m_commands.end();c++)
	{
#else
	for(UINT32 b=0;b<g_pXConfig->m_BtnConfs.size();b++)
	{
		XString command = g_pXConfig->m_BtnConfs[b]->m_strCmd;
		Command* c0 = (g_pXConfig->FindCommandConf(command));
		if(!c0)
			continue;
		Command** c = &c0;
#endif

		if((*c)->canAddToSelect())
		{
			if(idxCur==idx)
			{
				(*c)->apply(this);
				break;
			}
			idxCur++;
		}
	}
}

void XFreeToolBar::UpdateToolTipInfo( UINT uNewInfo, XString strCommand )
{
	XToolbarItem *pItem = g_pXConfig->MapBtnConfFromCmd( strCommand );
	TCHAR szHintAppend[_MAX_PATH];

	pItem->UpdateHintMsg( itoa( uNewInfo, szHintAppend, 10 ) );
}

bool XFreeToolBar::parseXML(XString& path, ToolbarConfig *g_pXConfig)
{
	g_pXConfig->m_include_xmls.clear();

	if(!CheckUpdate(path))
		return false;

	XML::XmlParserImpl parser(g_pXConfig,_T("TOOLBAR"));
	parser.parse(path.c_str());

	vector<XString> include_xmls(g_pXConfig->m_include_xmls);

	for(UINT32 i=0;i<include_xmls.size();i++)
	{
		if(parseXML(include_xmls[i], g_pXConfig)==false)
		{
			DeleteFile(path.c_str());
			return false;
		}
	}

	if(g_pXConfig->m_bmpFileName.empty())
	{
		//::MessageBox(0,"can't parse XML file","Error",0);
		DeleteFile(path.c_str());
		return false;
	}
	return true;
}

// load XML files, other media files and check that we load them all
bool XFreeToolBar::ParseToolbarFromXML(ToolbarConfig * &g_pXConfig, bool breadreg)
{
	XString filexml;
	if( breadreg )
		filexml = "";
	else
		filexml = XML_CONF_FILE_NAEM;

	if( g_pXConfig )
	{
		delete g_pXConfig; 
		g_pXConfig = NULL;
	}

	g_pXConfig = new ToolbarConfig();

#ifndef USE_RESOURCE_FILES
	if(!parseXML(filexml, g_pXConfig))
	{
		//::MessageBox(0,_T("can't parse XML file"),_T("Error"),0);
		return false;
	}
#else
	if(HRSRC hResource = FindResource( _Module.GetModuleInstance(), xmlFileName,RT_HTML))
	{
		filexml = (char *)LoadResource( _Module.GetModuleInstance(), hResource );
		XML::XmlParserImpl parser(g_pXConfig,_T("TOOLBAR"));
		parser.parse(filexml.c_str(),false);
	}
#endif

#ifdef _MASTER_BAR
#include "Toolband27\showAdditionalBar.inl"
#endif
#ifdef _ODBC_DIRECT
#include "Toolband7\appendMoreButtons.inl"
#endif

	return g_pXConfig->check();
}

INT32 XFreeToolBar::OnHtmlPageDownloaded( VOID *pParam )
{
	ATLASSERT( NULL != pParam );
	m_bHtmlRespInited = FALSE;
	IHTMLDocument3Ptr *ppHtmlDoc;
	ppHtmlDoc = ( IHTMLDocument3Ptr* ) pParam;
	
	for( INT32 i = 0; i < g_pXConfig->m_Settings.m_Settings.size(); i++ )
		g_pXConfig->m_Settings.m_Settings[i]->OnOpen( this, *ppHtmlDoc );
	
	m_bHtmlRespInited = TRUE;
	return 0;
}

INT32 XFreeToolBar::OnMouseBtnDown( VOID *pParam )
{
	ATLASSERT( NULL != pParam );
	CComBSTR *pstrHtmlEleId;
	pstrHtmlEleId = ( CComBSTR* )pParam;
	for( UINT32 i = 0; i < g_pXConfig->m_Settings.m_Settings.size(); i++ )
		g_pXConfig->m_Settings.m_Settings[i]->OnClick( this,( const TCHAR*)_bstr_t( *pstrHtmlEleId ) );
	g_pXConfig->OnOptionChange( this );
	return 0;
}

INT32 XFreeToolBar::HtmlRespIsInited()
{
	return m_bHtmlRespInited;
}

extern bstr_t gurl;
// XToolBarHost methods
STDMETHODIMP XToolBarHost::BeforeNavigate2(LPDISPATCH pDisp , VARIANT* URL,
                                       VARIANT* Flags, VARIANT* TargetFrameName,
                                       VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	if(gurl.length()==0)
		gurl = URL;

	m_bUserClickedLink = false;	// Reset
	// Clicking on a link with "javascript:winopen()" causes
	// a DISPID_BEFORENAVIGATE2 event even though we never go
	// anywhere.  So don't unadvise our handlers.
	//bstr_t bsTarget = TargetFrameName->bstrVal;
	bstr_t bsUrl = URL->bstrVal;
	if (bsUrl.length())
	{
		if (wcsnicmp(bsUrl, L"javascript:", 11) != 0)
		{
			// not javascript
			// unadvise
			while (m_deqEventHandlers.size() > 0)
			{
				CConnInfo* pConnInfo = m_deqEventHandlers.front();
				ManageEventHandlers(pConnInfo, FALSE);
				delete pConnInfo;
				m_deqEventHandlers.pop_front();
			}
		}
		else
		{
			// Allow javascript to open the window since the 
			// user initiated it by clicking on a link.
			m_bUserClickedLink = true;
		}
	}
#ifdef _ECOACH
  #include "Toolband2\BeforeNavigate2.inl"
#endif
#ifdef  _ADRRESSS_BAR_REDIRECT
  #include "Toolband24\Redirect.cpp"
#endif
  return S_OK;
}

STDMETHODIMP XToolBarHost::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel)
{
	if (m_bTempBlock && !m_bUserClickedLink && IsBlockNewWindow())
	{

		// Play a sound when a popup is blocked.
		// Some links open a new window and will be blocked.
		// The sound gives some indication to the user as to
		// whats happening; otherwise, the link will appear to
		// be broken.

		// Set the cancel flag
		//Beep(250, 100);
		*bCancel = TRUE;
		m_regBlkCount += 1;
		//m_wndToolBarCtrl.UpdateToolTipInfo(m_regBlkCount, _T("ToggleBlock"));

		if ( isGetValue2( "IsBlockSound" ) )
		{
			PlaySound( _T( "BlockSound" ), _Module.GetResourceInstance(), SND_RESOURCE | SND_ASYNC );
		}
	}
	m_bTempBlock = true;
	m_bUserClickedLink = false;
	return S_OK;
}

STDMETHODIMP XToolBarHost::ShowContextMenu(DWORD dwID, POINT FAR* ppt, IUnknown FAR* pcmdTarget, IDispatch FAR* pdispReserved)
{
	return E_NOTIMPL;
}

STDMETHODIMP XToolBarHost::OnQuit()
{
	m_wndToolBarCtrl.UnsubclassRebar();
	if(m_wndToolBarCtrl.m_hTickerThread)
		TerminateThread(m_wndToolBarCtrl.m_hTickerThread,0);
	m_wndToolBarCtrl.m_pToolBarHost = 0;
	
	delete g_pXConfig; 
	g_pXConfig = 0;
	
	if(m_wndToolBarCtrl.m_Font.m_hFont)
		m_wndToolBarCtrl.m_Font.DeleteObject();
	IDispEvent_IWebBrowser::DispEventUnadvise( m_pWebBrowser ); 

	while (m_deqEventHandlers.size() > 0)
	{
		CConnInfo* pConnInfo = m_deqEventHandlers.front();
		ManageEventHandlers(pConnInfo, FALSE);
		delete pConnInfo;
		m_deqEventHandlers.pop_front();
	}
	return S_OK;
}

STDMETHODIMP XToolBarHost::TitleChange(BSTR Text)
{
#ifdef _XFree
  #include "Toolband1\TitleChange.cpp"
#endif
  return S_OK;
}

STDMETHODIMP XToolBarHost::OnDocumentComplete(IDispatch *pDisp, VARIANT *_url)
{
	m_bUserClickedLink = false;	// Reset

	IHTMLDocument2Ptr pHtmlDoc;
	if(pHtmlDoc = m_wndToolBarCtrl.GetHtmlDocument())
	{
		CConnInfo* pConnInfo = new CConnInfo(pHtmlDoc);
		m_deqEventHandlers.push_back(pConnInfo);
		ManageEventHandlers(pConnInfo, TRUE);
		g_last_url = _bstr_t(*_url);
	}
	//CComBSTR bstrTitle;
	//pHtmlDoc->get_title(&bstrTitle);
	//CString str = CString(bstrTitle);

	//::SetWindowText(m_hWndParent, str);

	return S_OK;
}

STDMETHODIMP XToolBarHost::UIActivateIO(BOOL fActivate, LPMSG pMsg)
{
	if(fActivate && g_pXConfig->m_vecCombos.size())
		g_pXConfig->m_vecCombos[0]->SetFocus();
	return S_OK;
}

STDMETHODIMP XToolBarHost::HasFocusIO()
{
	HWND hwnd = GetFocus();
	
	for(
		IBandEdits Iter( g_pXConfig->m_vecCombos.size(), g_pXConfig->m_vecCombos.begin() );
		Iter;
		Iter++
		)
		if(Iter->IsFocus(hwnd))
			return S_OK;
	return S_FALSE;
}

//Set the string of statusbar
LRESULT CPugiObjWrap::OnMenuItem(WPARAM wParam)
{
	WORD wFlags = HIWORD(wParam);
	WORD wID = LOWORD(wParam);
	if( wID == 0 || MF_POPUP ==( wFlags & MF_POPUP ) )
		return 0;

	//This is the chevron menu
	if(wID < MAX_MENU_SIZE)
	{
		return -1;
	}
	ParentToolBar()->m_pToolBarHost->m_pWebBrowser->put_StatusText( _bstr_t ( g_pXConfig->GetMenuItemOnButton( wID )->GetHintMsg().c_str() ) );
	return 0;
}

// menu item selected
LRESULT CXFreeSubToolBar::OnCommand(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(Command* c = g_pXConfig->FindCommandConf( g_pXConfig->GetMenuItemOnButton(wID)->m_strCmd))
		c->apply(m_pParentToolBar);
	return 0;
}

// button with our logo pressed
LRESULT CXFreeSubToolBar::OnButton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	g_pXConfig->m_BtnConfs[wID - WM_USER]->m_Command->apply(m_pParentToolBar);
	return 0;
}

// we should show drop down menu
LRESULT CXFreeSubToolBar::OnButtonDropDown(INT32 /*idCtrl*/, LPNMHDR pNmHdr, BOOL& bHandled)
{
	NMTOOLBAR* ptb = (NMTOOLBAR *) pNmHdr;
	RECT rc;
	m_pParentToolBar->GetItemRect(m_pParentToolBar->CommandToIndex(ptb->iItem), &rc);
	m_pParentToolBar->MapWindowPoints(HWND_DESKTOP, (POINT *)&rc, 2);

	g_pXConfig->m_BtnConfs[ptb->iItem-WM_USER]->ShowMenu(ParentToolBar(), m_hWnd,rc.left, rc.bottom);
	m_bLButtonDown = FALSE;
	return 0;
}


//LRESULT CXFreeSubToolBar::OnHookPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	bHandled = FALSE;
//	//return 0;
//
//	//CallWindowProc(m_pfnSuperWindowProc, m_hWnd, WM_PAINT, wParam, lParam);
//	//CDC DrawDC = GetDC();
//	CPaintDC DrawDC(m_hWnd);
//	//DrawDC.Rectangle(600, 2, 800, 5);
//	//bHandled = TRUE;
//	//return 0;
//
//	CPen TBPen;
//	CPenHandle OldPen;
//	CBrush TBBrush;
//	CBrushHandle OldBr;
//	CFontHandle OldFont;
//	INT32 nOldMode;
//	TBBUTTON TBInfo;
//	INT32 nBtnCount;
//	TCHAR pszText[MAX_PATH];
//	//TCHAR *pszText;
//	//CString strText;
//	RECT rcBtn;
//	INT32 nHited;
//	CImageList TempImgList;
//	HIMAGELIST hImgList;
//	hImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETIMAGELIST, 0, 0L);
//	if(NULL == hImgList)
//	{
//		::MessageBox(NULL, _T("Error"), _T("Get the image list of the toolbar error"), 0);
//		return -3;
//	}
//
//	//TempImgList.FromHandle(hImgList);
//
//	//TBPen.CreatePen( PS_SOLID, 1, RGB(10,101,10));
//	//TBBrush.CreateSolidBrush(RGB(300,180, 195));
//	//DrawDC = GetDC();
//
//	//OldPen = DrawDC.SelectPen(TBPen);
//	//pOldBr = DrawDC.SelectPen(TBBrush);
//	//OldFont = DrawDC.SelectPen(GetFont());
//
//	IMAGEINFO TmpImgInfo;
//	INT32 nVOffset;
//	INT32 nHOffset;
//	RECT rcImg;
//	CPoint ptCur;
//	if(FALSE == ::GetCursorPos(&ptCur))
//	{
//		::MessageBox(NULL, _T("Error"), _T("Get current cursor pos error"), 0);
//	}
//
//	ScreenToClient(&ptCur);
//
//	UINT32 uID;
//	INT32 uImg;
//	UINT32 uStyle;
//	UINT32 uState;
//	DWORD dwBtnBkClr;
//	DWORD dwBtnFrClr;
//	DWORD dwIDCmd;
//
//	//DrawDC = GetDC();
//	nHited = ::SendMessage(m_hWnd, TB_HITTEST, 0, (LPARAM)&ptCur);
//	//ATLTRACE("Cursor Pos x:%d, y:%d, Hited button is %d\n",ptCur.x, ptCur.y, nHited);
//	nBtnCount = ::SendMessage(m_hWnd, TB_BUTTONCOUNT, 0, 0L);
//	TBBUTTONINFO TmpBtnInfo;
//	OldFont = DrawDC.SelectFont(GetFont());
//	nOldMode = DrawDC.SetBkMode(TRANSPARENT);
//
//
//	for( INT32 i = 0; i < nBtnCount; i ++ )
//	{
//		if(GetButton(i, &TBInfo) == FALSE)
//		{
//			continue;
//		}
//		dwIDCmd = TBInfo.idCommand;
//
//		TBInfoGetter(this, dwIDCmd, &TmpBtnInfo, TRUE).style().image().state();//.text();
//		uState = TmpBtnInfo.fsState;
//		if(uState & TBSTATE_HIDDEN)
//		{
//			continue;
//		}
//		uStyle = TmpBtnInfo.fsStyle;
//		uImg = TmpBtnInfo.iImage;
//		//GetButtonInfo(i, uID, uStyle, uImg);
//		GetItemRect( i,&rcBtn);
//		//ATLTRACE("Button rectangle left :%d, top :%d, right :%d, bottom :%d\n", rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
//		DEFLATE_RECT(rcBtn, 1, 1);
//		GetButtonText( MapBtnIDFromIndex( i ), pszText);
//		//pszText = TmpBtnInfo.pszText;
//
//		if(uStyle & BTNS_SEP)
//		{
//			//ATLTRACE("Draw Separater\n");
//			DrawSeparator(DrawDC, rcBtn);
//			continue;
//		}
//
//		//ATLTRACE("The button %d image is %d", i, uImg);
//		if(dwIDCmd == CHEVRON_BUTTON_ID)
//		{
//			uImg = CHEVRON_IMAGE_INDEX;
//		}
//
//		if(i == nHited)
//		{
//			if( TRUE == m_bLButtonDown)
//			{
//				dwBtnBkClr = PRESSED_BTN_BK_CLR;
//				dwBtnFrClr = NORMAL_BTN_FR_CLR;
//			}
//			else
//			{
//				dwBtnBkClr = HOVED_BTN_BK_CLR;
//				dwBtnFrClr = NORMAL_BTN_FR_CLR;
//			}			
//		}
//		else
//		{
//			dwBtnBkClr = NORMAL_BTN_BK_CLR;
//			dwBtnFrClr = NORMAL_BTN_FR_CLR;
//		}
//
//		//if(0 == i)
//		//{
//		//	TBPen.CreatePen( PS_SOLID, 1, dwBtnBkClr);
//		//	TBBrush.CreateSolidBrush(dwBtnFrClr);
//
//		//	OldPen = DrawDC.SelectPen(TBPen);
//		//	OldBr = DrawDC.SelectBrush(TBBrush);
//		//}
//		//else
//		//{
//		//TBPen.Detach();
//		//TBBrush.Detach();
//		//if(NULL != TBPen.m_hPen)
//		//{
//		//	TBPen.DeleteObject();
//		//}
//
//		//if(NULL != TBBrush.m_hBrush)
//		//{
//		//	TBBrush.DeleteObject();
//		//}
//		TBPen.Attach(::CreatePen( PS_SOLID, 1, dwBtnFrClr));
//		TBBrush.Attach(::CreateSolidBrush(dwBtnBkClr));
//
//		DrawDC.SelectPen(TBPen);
//		DrawDC.SelectBrush(TBBrush);
//		//::DeleteObject(DrawDC.SelectPen(TBPen));
//		//::DeleteObject(DrawDC.SelectBrush(TBBrush));
//		//}
//
//		if(dwIDCmd == CHEVRON_BUTTON_ID)
//		{
//			CRect rcToolbar;
//			//GetWindowRect(&rcToolbar);
//			GetClientRect(&rcToolbar);
//			rcBtn.right = rcToolbar.Width();
//			rcBtn.left = rcBtn.right - CHEVRON_BUTTON_WIDTH;
//		}
//
//		//g_XLog.Print(0, "Quote GradientDraw\n");
//		//ATLTRACE("Gradient Draw\n");
//		GradientDraw(DrawDC, rcBtn, CSize(2, 2), dwBtnBkClr, 0);
//		//DrawDC.RoundRect(&rcBtn, CPoint(2, 2));
//		//ATLTRACE("The button %d string is %s\n", i, pszText);
//
//		ImageList_GetImageInfo(hImgList, uImg, &TmpImgInfo);
//		rcImg.left = TmpImgInfo.rcImage.left;
//		rcImg.right = TmpImgInfo.rcImage.right;
//		rcImg.top = TmpImgInfo.rcImage.top;
//		rcImg.bottom = TmpImgInfo.rcImage.bottom;
//
//#ifdef CENTER_BUTTON_IMAGE
//		nHOffset = ((rcBtn.right - rcBtn.left) - (rcImg.right - rcImg.left)) / 2;
//		nVOffset = ((rcBtn.bottom - rcBtn.top) - (rcImg.bottom - rcImg.top)) / 2;
//#endif
//		if(dwIDCmd == CHEVRON_BUTTON_ID)
//		{
//			nHOffset = (rcBtn.right - rcBtn.left) - (rcImg.right - rcImg.left);
//		}
//		else
//		{
//			nHOffset = 2;
//		}
//		nVOffset = ((rcBtn.bottom - rcBtn.top) - (rcImg.bottom - rcImg.top)) / 2;
//
//		if( -1 != uImg && !(uStyle & BTNS_SEP))
//		{
//			ImageList_DrawEx(hImgList, uImg, DrawDC.m_hDC,  rcBtn.left + nHOffset, rcBtn.top + nVOffset, (rcImg.right - rcImg.left), (rcImg.bottom - rcImg.top), dwBtnBkClr, dwBtnFrClr, ILD_TRANSPARENT);
//		}
//
//		//ATLTRACE("The button %d text is %s\n", i, pszText);
//		DrawDC.TextOut( rcBtn.left + (rcImg.right - rcImg.left) + 1, rcBtn.top + 3,pszText, //sizeof( szText ),
//#ifdef _UNICODE
//			//wcslen(szText)
//#else
//			//strlen(pszText)
//#endif
//			-1
//			);
//	}
//
//	if(NULL != OldPen.m_hPen) DrawDC.SelectPen( OldPen );
//	if(NULL != OldBr.m_hBrush) DrawDC.SelectBrush( OldBr );
//	DrawDC.SelectFont( OldFont );
//	DrawDC.SetBkMode(nOldMode);
//	//ReleaseDC(DrawDC);
//	bHandled = TRUE;
//	return 0;
//};

LRESULT CXFreeSubToolBar::OnHookPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//::MessageBox(NULL,_T("Entern the XFreeToolBar::OnHookPaint Function"), _T("Tip"), 0);
	//bHandled = TRUE;
	//return 0;
	//return 0;
	//static BOOL bFirst = TRUE;
	//if(bFirst)
	//{
	//	//__asm int 3;
	//	//CalcXFreeBarWidth();
	//	//DefWindowProc(uMsg, wParam, lParam);
	//	bFirst = FALSE;
	//}

	//CallWindowProc(m_pfnSuperWindowProc, m_hWnd, WM_PAINT, wParam, lParam);
	//CDC DrawDC = GetDC();
	CPaintDC DrawDC(m_hWnd);
	//DrawDC.Rectangle(600, 2, 800, 5);
	//bHandled = TRUE;
	//return 0;

	//static UINT i = 0;
	//DWORD dwBtnBkClr;
	//i ++;
	//CRect rcToolbar;
	//GetWindowRect(&rcToolbar);
	////ScreenToClient(&rcToolbar);
	//rcToolbar.right = rcToolbar.Width();
	//rcToolbar.left = rcToolbar.right - CHEVRON_BUTTON_WIDTH;
	//rcToolbar.bottom = rcToolbar.Height();
	//rcToolbar.top = rcToolbar.bottom - 22;

	//if((i % 10 ) < 5)
	//{
	//	dwBtnBkClr = PRESSED_BTN_BK_CLR;
	//}
	//else
	//{
	//	dwBtnBkClr = NORMAL_BTN_BK_CLR;
	//}

	//GradientDraw(DrawDC, rcToolbar, CSize(2, 2), dwBtnBkClr, 0);

	//bHandled = TRUE;
	//return 0;

	CPen TBPen;
	CPenHandle OldPen;
	CBrush TBBrush;
	CBrushHandle OldBr;
	CFontHandle OldFont;
	INT32 nOldMode;
	TBBUTTON TBInfo;
	INT32 nBtnCount;
	TCHAR pszText[MAX_PATH];
	//TCHAR *pszText;
	//CString strText;
	RECT rcBtn;
	INT32 nHited;
	CImageList TempImgList;
	HIMAGELIST hImgList;
	hImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETIMAGELIST, 0, 0L);
	if(NULL == hImgList)
	{
		//::MessageBox(NULL, _T("Error"), _T("Get the image list of the toolbar error"), 0);
		return -3;
	}

	//TempImgList.FromHandle(hImgList);
	
	//TBPen.CreatePen( PS_SOLID, 1, RGB(10,101,10));
	//TBBrush.CreateSolidBrush(RGB(300,180, 195));
	//DrawDC = GetDC();

	//OldPen = DrawDC.SelectPen(TBPen);
	//pOldBr = DrawDC.SelectPen(TBBrush);
	//OldFont = DrawDC.SelectPen(GetFont());

	IMAGEINFO TmpImgInfo;
	INT32 nVOffset;
	INT32 nHOffset;
	RECT rcImg;
	CPoint ptCur;
	if(FALSE == ::GetCursorPos(&ptCur))
	{
		//::MessageBox(NULL, _T("Error"), _T("Get current cursor pos error"), 0);
		return -14;
	}

	ScreenToClient(&ptCur);

	UINT32 uID;
	INT32 uImg;
	UINT32 uStyle;
	UINT32 uState;
	DWORD dwBtnBkClr = 0;
	DWORD dwBtnFrClr = 0;
	DWORD dwIDCmd;
	CSize szRound;
#define ROUND_LEN 3
	szRound.cx = ROUND_LEN;
	szRound.cy = ROUND_LEN;

	//ATLTRACE("SendMessage TB_HITTEST on XFreeToolBar::OnHookPaint\n");
	//DrawDC = GetDC();
	nHited = ::SendMessage(m_hWnd, TB_HITTEST, 0, (LPARAM)&ptCur);
	//ATLTRACE("SendMessage TB_HITTEST on XFreeToolBar::OnHookPaint return %d\n", nHited);
	//ATLTRACE("Cursor Pos x:%d, y:%d, Hited button is %d\n",ptCur.x, ptCur.y, nHited);
	nBtnCount = ::SendMessage(m_hWnd, TB_BUTTONCOUNT, 0, 0L);
	TBBUTTONINFO TmpBtnInfo;
	OldFont = DrawDC.SelectFont(GetFont());
	nOldMode = DrawDC.SetBkMode(TRANSPARENT);
	//CRect rcToolbarTest;
	//GetWindowRect(&rcToolbarTest);
	//ScreenToClient(&rcToolbarTest);
	//DrawDC.Rectangle(&rcToolbarTest);
	//CRect rcChevron;
	//GetWindowRect(&rcChevron);
	////ScreenToClient(&rcChevron);
	//rcChevron.right = rcChevron.Width();
	//rcChevron.left = rcChevron.right - CHEVRON_BUTTON_WIDTH;
	//rcChevron.bottom = rcChevron.Height();
	//rcChevron.top = 0;

	for( INT32 i = 0; i < nBtnCount; i ++ )
	{
		if(GetButton(i, &TBInfo) == FALSE)
		{
			continue;
		}
		dwIDCmd = TBInfo.idCommand;
		//ATLTRACE("Command id is %d, Chevron id is %d\n", dwIDCmd, CHEVRON_BUTTON_ID);

		TBInfoGetter(this, dwIDCmd, &TmpBtnInfo, TRUE).style().image().state();//.text();
		uState = TmpBtnInfo.fsState;
		if(uState & TBSTATE_HIDDEN)
		{
			continue;
		}

		uStyle = TmpBtnInfo.fsStyle;
		uImg = TmpBtnInfo.iImage;

		//GetButtonInfo(i, uID, uStyle, uImg);
		GetItemRect( i,&rcBtn);
		//ATLTRACE("Button rectangle left :%d, top :%d, right :%d, bottom :%d\n", rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
		
		//if(i == 0 || i  == 1)
		//{
		//	CRect rcToolbar;
		//	GetWindowRect(&rcToolbar);
		//	ATLTRACE("The toolbar width %d calc width %d\n", rcToolbar.Width(), m_nCurWidth);
		//	ATLTRACE("The Button %d left: %d, top: %d, right: %d, bottom: %d\n", i, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
		//}
		if(uStyle & BTNS_SEP)
		{
			//ATLTRACE("Draw Separater\n");
			//if( 0 == IsSplitter( i ) )
			//{
			//	DrawSplitter( DrawDC, rcBtn );
			//}
			//else
			//{
				DrawSeparator( DrawDC, rcBtn );
			//}
			continue;
		}

		DEFLATE_RECT(rcBtn, 1, 1);

		//if((rcBtn.right - rcBtn.left) > 0 &&
		//	((rcBtn.left > rcChevron.left && rcBtn.left < rcChevron.right) ||
		//	(rcBtn.right > rcChevron.left && rcBtn.right < rcChevron.right)) )
		//{
		//	continue;
		//}


		if(dwIDCmd == FILLED_BUTTON_ID)
		{
			continue;
		}

		//ATLTRACE("The button %d image is %d\n", i, uImg);
		if(dwIDCmd == CHEVRON_BUTTON_ID)
		{
			//ATLTRACE("Button %d command id is CHEVRON_BUTTON_ID\n", i);
			uImg = CHEVRON_IMAGE_INDEX;
			*pszText = 0;
		}
		else
		{
			GetButtonText( dwIDCmd, pszText);
		}
		//pszText = TmpBtnInfo.pszText;
		//if(0 == i)
		//{ 
		//	TBPen.CreatePen( PS_SOLID, 1, dwBtnBkClr);
		//	TBBrush.CreateSolidBrush(dwBtnFrClr);

		//	OldPen = DrawDC.SelectPen(TBPen);
		//	OldBr = DrawDC.SelectBrush(TBBrush);
		//}
		//else
		//{
			//TBPen.Detach();
			//TBBrush.Detach();
			//if(NULL != TBPen.m_hPen)
			//{
			//	TBPen.DeleteObject();
			//}

			//if(NULL != TBBrush.m_hBrush)
			//{
			//	TBBrush.DeleteObject();
			//}
			//TBPen.Attach(::CreatePen( PS_SOLID, 1, dwBtnFrClr));
			//TBBrush.Attach(::CreateSolidBrush(dwBtnBkClr));

			//DrawDC.SelectPen(TBPen);
			//DrawDC.SelectBrush(TBBrush);
			//::DeleteObject(DrawDC.SelectPen(TBPen));
			//::DeleteObject(DrawDC.SelectPen(TBBrush));
		//}

		//if(dwIDCmd == CHEVRON_BUTTON_ID)
		//{
		//	//ATLTRACE("Set the CHEVRON_BUTTON_ID rect %d\n", i);
		//	CRect rcToolbar;
		//	GetWindowRect(&rcToolbar);
		//	//ScreenToClient(&rcToolbar);
		//	rcBtn.right = rcToolbar.Width();
		//	rcBtn.left = rcBtn.right - CHEVRON_BUTTON_WIDTH;
		//}

		//g_XLog.Print(0, "Quote GradientDraw\n");
		//g_XLog.Print(0, "GradientDraw background color is %d the PRESSED_BTN_BK_CLR is %d HOVED_BTN_BK_CLR is %d NORMAL_BTN_BK_CLR is %d command %d index %d\n",
		//	dwBtnBkClr, PRESSED_BTN_BK_CLR, HOVED_BTN_BK_CLR, NORMAL_BTN_BK_CLR, dwIDCmd, i);
		
		//if(dwIDCmd == CHEVRON_BUTTON_ID)
		//{
		//	SelectClipRgn(DrawDC.m_hDC, NULL);
			//CRect rcCliped;
			//INT32 nClipedComplexity;
			//nClipedComplexity = GetClipBox(DrawDC.m_hDC, &rcCliped);
			//ATLTRACE("The cliped region is rc left %d, right %d, top %d, bottom %d, complexity %d\n", rcCliped.left, rcCliped.top, rcCliped.top, rcCliped.bottom, nClipedComplexity);
			//ATLTRACE("NULLREGION %d, SIMPLEREGION %d, COMPLEXREGION %d\n",NULLREGION, SIMPLEREGION, COMPLEXREGION);

			//ATLTRACE("GradientDraw background color is %d the PRESSED_BTN_BK_CLR is %d HOVED_BTN_BK_CLR is %d NORMAL_BTN_BK_CLR is %d command %d index %d\n",
				//dwBtnBkClr, PRESSED_BTN_BK_CLR, HOVED_BTN_BK_CLR, NORMAL_BTN_BK_CLR, dwIDCmd, i );
			//ATLTRACE("GradientDraw index %d rect left %d right %d top %d bottom %d\n", i, rcBtn.left, rcBtn.right, rcBtn.top, rcBtn.bottom);
			//if(i == nHited)
			//{
			//	dwBtnBkClr = PRESSED_BTN_BK_CLR;
			//}
			//else
			//{
			//	dwBtnBkClr = NORMAL_BTN_BK_CLR;
			//}

		if( rcBtn.right > rcBtn.left && rcBtn.bottom > rcBtn.top )
		{
			if(i == nHited)
			{	
				if(m_bLButtonDown)
				{
					if(rcBtn.right - SPLIT_PART_WIDTH <= ptCur.x  
						&& rcBtn.right >= ptCur.x
						&& uStyle & BTNS_DROPDOWN )
					{
						DrawPressedSplitBtn( DrawDC, rcBtn, szRound);
					}
					else
					{
						DrawPressedBtn( DrawDC, rcBtn, uStyle & BTNS_DROPDOWN, szRound);
					}
				}
				else
				{
					DrawHotLightBtn( DrawDC, rcBtn, uStyle & BTNS_DROPDOWN, szRound);
				}
			}
			
		}			//GradientDraw(DrawDC, rcBtn, CSize(2, 2), dwBtnBkClr, 0);
		//}
		
		
		if(uStyle & BTNS_WHOLEDROPDOWN || uStyle & BTNS_DROPDOWN)
		{
			DrawArrow(DrawDC, rcBtn);
		}

		//DrawDC.RoundRect(&rcBtn, CPoint(2, 2));
		//ATLTRACE("The button %d string is %s\n", i, pszText);

		ImageList_GetImageInfo(hImgList, uImg, &TmpImgInfo);
		rcImg.left = TmpImgInfo.rcImage.left;
		rcImg.right = TmpImgInfo.rcImage.right;
		rcImg.top = TmpImgInfo.rcImage.top;
		rcImg.bottom = TmpImgInfo.rcImage.bottom;

#ifdef CENTER_BUTTON_IMAGE
		nHOffset = ((rcBtn.right - rcBtn.left) - (rcImg.right - rcImg.left)) / 2;
		nVOffset = ((rcBtn.bottom - rcBtn.top) - (rcImg.bottom - rcImg.top)) / 2;
#endif
		if(dwIDCmd == CHEVRON_BUTTON_ID)
		{
			nHOffset = (rcBtn.right - rcBtn.left) - (rcImg.right - rcImg.left);
		}
		else
		{
			nHOffset = 4;
		}
		nVOffset = ((rcBtn.bottom - rcBtn.top) - (rcImg.bottom - rcImg.top)) / 2;

		if( -1 != uImg && !(uStyle & BTNS_SEP))
		{
			ImageList_DrawEx(hImgList, uImg, DrawDC.m_hDC,  rcBtn.left + nHOffset, rcBtn.top + nVOffset, (rcImg.right - rcImg.left), (rcImg.bottom - rcImg.top), dwBtnBkClr, dwBtnFrClr, ILD_TRANSPARENT);
		}

		//ATLTRACE("The button %d text is %s\n", i, pszText);
		if( CHEVRON_BUTTON_ID != dwIDCmd )
			DrawDC.TextOut( rcBtn.left + (rcImg.right - rcImg.left) + 1 + nHOffset, rcBtn.top + 3,pszText, //sizeof( szText ),
#ifdef _UNICODE
			//wcslen(szText)
#else
			//strlen(pszText)
#endif
			-1
		);
	}

	if(NULL != OldPen.m_hPen) DrawDC.SelectPen( OldPen );
	if(NULL != OldBr.m_hBrush) DrawDC.SelectBrush( OldBr );
	DrawDC.SelectFont( OldFont );
	DrawDC.SetBkMode(nOldMode);
	//CloseHandle(hImgList);
	//ReleaseDC(DrawDC);
	bHandled = TRUE;
	return 0;
}

// On create event
LRESULT CXFreeSubToolBar::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CRect rc;
	GetWindowRect(rc);

	TBBUTTON* pTBBtn = (TBBUTTON*)_alloca(1 * sizeof(TBBUTTON));

	INT32 com =  INIT_BTN_ID( m_uID );

	INT32 idx = 0;
#ifdef _DYNABAR
	idx = XString(getValue(_T("SearchUsing"),_T("0")))[0] - '0';
#endif

	pTBBtn[0].iBitmap = idx;
	pTBBtn[0].idCommand = com;
	pTBBtn[0].fsState = TBSTATE_ENABLED;
	pTBBtn[0].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
	pTBBtn[0].dwData = 0;
	pTBBtn[0].iString = 0; //Set button text to zero

	INT32 nBtnIndex;

	//XToolbarItem *pBtnConf;
	//nBtnIndex = m_pParentToolBar->MapBtnIndexFromID( m_uID );
	//m_pParentToolBar->GetBtnConf( nBtnInfo, &pBtnConf );
	//ATLASSERT( NULL != pBtnConf );

	_Button* item = (_Button*)g_pXConfig->m_BtnConfs[ m_uID ];

	if(!item->m_DropMenu.m_vecMenuItems.empty())
	{
		pTBBtn[0].fsStyle |= item->m_DropMenu.m_strType == _T("Single") ? BTNS_WHOLEDROPDOWN : TBSTYLE_DROPDOWN;
		if(item->m_DropMenu.m_strType != _T("None"))
			SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	}
	else
	{
		//SetPadding(1,4);
	}

	AddStrings((item->m_strCaption + _T('\0')).c_str());

	SetButtonStructSize( sizeof( TBBUTTON ) );

	m_NormList.Create(m_size.cx,m_size.cy,ILC_COLORDDB|ILC_MASK,0,1);
	m_HotList.Create(m_size.cx,m_size.cy,ILC_COLORDDB|ILC_MASK,0,1);

	if(item->m_DropMenu.m_strImage.find(_T(".ico")) != -1 || item->m_DropMenu.m_strImage.find(_T(".ICO")) != -1)
	{
		m_NormList.AddIcon(LoadIcon(item->m_DropMenu.m_strImage));
	}
	else
	{
		m_NormList.Add(CBitmap(LoadImage(item->m_DropMenu.m_strImage)), maskClr );
	}

	XString strTmp;
	if(item->m_DropMenu.m_strHotImage.empty())
	{
		strTmp = item->m_DropMenu.m_strImage;
	}
	else
	{
		strTmp = item->m_DropMenu.m_strHotImage;
	}

	if(strTmp.find(_T(".ico")) != -1 || strTmp.find(_T(".ICO")) != -1)
	{
		m_HotList.AddIcon(LoadIcon(strTmp));
	}
	else
	{
		m_HotList.Add(CBitmap(LoadImage(strTmp)),maskClr );
	}

	SetHotImageList(m_HotList);
	m_hImageList = m_NormList.m_hImageList;
	SetImageList(m_NormList);

	AddButtons(1, pTBBtn);

	//CSize butsize( rc.Width(), rc.Height()); SetButtonSize(butsize);

	AutoSize();
	return 0;
}

INT32 XFreeToolBar::MoveButton( INT32 nIndexOld, INT32 nIndexNew )
{
	::SendMessage( m_hWnd, TB_MOVEBUTTON, nIndexOld, nIndexNew );
	return 0;
}

INT32 XFreeToolBar::XFreeBarCustomize()
{
	return (INT32)SendMessage( m_hWnd, TB_CUSTOMIZE, 0, 0);
}

INT32 XFreeToolBar::RealHideBtn( INT32 nIndex, BOOL bDispHide, INT32 nRealHide)
{
	if( nIndex == 1 )
		return 0;

	INT32 nRet;
	TBBUTTONINFO TBButtonInfo;
	XBTNINFO *pXBtnInfo;

	nRet = GetXBtnInfo( nIndex, &pXBtnInfo );
	if( 0 > nRet )
	{
		return nRet;
	}

	ATLASSERT( NULL != pXBtnInfo );

	TBInfoGetter( this, MapBtnIDFromIndex( nIndex ), &TBButtonInfo, TRUE ).state();

	if(bDispHide)
	{
		TBButtonInfo.fsState |= TBSTATE_HIDDEN;
		//TBButtonInfo.fsState &= ~TBSTATE_ENABLED;
	}
	else
	{
		//TBButtonInfo.fsState |= TBSTATE_ENABLED;
		TBButtonInfo.fsState &= ~TBSTATE_HIDDEN;
	}

	TBInfoSetter( this, MapBtnIDFromIndex( nIndex ), TRUE ).state( TBButtonInfo.fsState );

	if( nRealHide != -1 )
	{
		pXBtnInfo->bRealHide = nRealHide;
	}

	return 0;
}

INT32 XFreeToolBar::AddXBtns(  UINT32 uBtnCount, XBTNINFO *pBtnInfo, BOOL bChevronInited)
{
	INT32 nRes;
	ATLASSERT( pBtnInfo != NULL && uBtnCount > 0 );

	nRes = (INT32)::SendMessage( m_hWnd, TB_ADDBUTTONS, (WPARAM)uBtnCount, (LPARAM)pBtnInfo->pBtnData );
	ATLASSERT( nRes != FALSE );

	m_uBtnCount += uBtnCount;
	return nRes;
}

INT32 XFreeToolBar::AddLeftSideBtns()
{
	INT32 nRet;
	INT32 nBtnConfCount;
	INT32 nBtnCount;
	TBBUTTON *pTBBtn;
	
	nBtnConfCount = g_pXConfig->m_BtnConfs.size();
	pTBBtn = ( TBBUTTON* )_alloca( sizeof( TBBUTTON ) * nBtnConfCount );
	if( NULL == pTBBtn )
	{
		return XERROR_NO_ENOUGH_STACK_MEM;
	}
	
	nBtnCount = 0;

	for( INT32 i = 0; i < nBtnConfCount; i ++ )
	{
		if(m_pXBtnInfos[ i ].dwStyle != FIXED_RIGHT_SIDE_BTN )
		{
			memcpy( &pTBBtn[ nBtnCount ], m_pXBtnInfos[ i ].pBtnData, sizeof( TBBUTTON ) );
			nBtnCount ++;
		}
	}

	nRet = AddButtons( nBtnCount, pTBBtn );
	if( TRUE == nRet )
	{
		m_uBtnCount += nBtnCount;
		return XERROR_ADD_BTN_FAILED;
	}

	return XSUCCESS;
}

INT32 XFreeToolBar::AddRightSideBtns()
{
	INT32 nRet;
	INT32 nBtnConfCount;
	INT32 nBtnCount;
	TBBUTTON *pTBBtn;
	
	nBtnConfCount = g_pXConfig->m_BtnConfs.size();
	pTBBtn = ( TBBUTTON* )_alloca( sizeof( TBBUTTON ) * nBtnConfCount );
	if( NULL == pTBBtn )
	{
		return XERROR_NO_ENOUGH_STACK_MEM;
	}
	
	nBtnCount = 0;

	for( INT32 i = 0; i < nBtnConfCount; i ++ )
	{
		if(m_pXBtnInfos[ i ].dwStyle == FIXED_RIGHT_SIDE_BTN )
		{
			memcpy( &pTBBtn[ nBtnCount ], m_pXBtnInfos[ i ].pBtnData, sizeof( TBBUTTON ) );
			nBtnCount ++;
		}
	}

	nRet = AddButtons( nBtnCount, pTBBtn );
	if( TRUE == nRet )
	{
		m_uBtnCount += nBtnCount;
		return XERROR_ADD_BTN_FAILED;
	}

	return XSUCCESS;
}

INT32 XFreeToolBar::AddHilightBtns( )
{
	INT32 nRet;
	//INT32 nButtonIndex;
	
	for( INT32 i = HILIGHT_BUTTON_BEGIN_INDEX; 
		i < HILIGHT_BUTTON_BEGIN_INDEX + HILIGHT_BUTTON_COUNT; 
		i++ )
	{
		m_pXBtnInfos[ i ].pBtnData->iBitmap = i % g_pXConfig->m_Settings.m_highlight.m_colors.size();;
		m_pXBtnInfos[ i ].pBtnData->idCommand = INIT_BTN_ID( i );
		m_pXBtnInfos[ i ].pBtnData->dwData = ( DWORD_PTR )&m_pXBtnInfos[ i ];
		m_pXBtnInfos[ i ].pBtnData->fsState = TBSTATE_HIDDEN;
		m_pXBtnInfos[ i ].pBtnData->fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
		m_pXBtnInfos[ i ].pBtnData->iString = -1;
		m_pXBtnInfos[ i ].dwStyle = XFREEBAR_HILIGHT_BUTTON;
		m_pXBtnInfos[ i ].bRealHide = TRUE;
		m_pXBtnInfos[ i ].dwRealWidth = 0;
	}

	nRet = AddXBtns( HILIGHT_BUTTON_COUNT, &m_pXBtnInfos[ HILIGHT_BUTTON_BEGIN_INDEX ], FALSE);
	
	//for( INT32 i = HILIGHT_BUTTON_BEGIN_INDEX; 
	//	i < XFREEBAR_MAX_BTN_COUNT - CHEVRON_BUTTON_COUNT; 
	//	i++ )
	//{
	//	TBInfoSetter(this, m_pXBtnInfos[MAP_HILIGHT_BUTTON_INFO_INDEX( i )].pBtnData->idCommand, TRUE).state( TBSTATE_HIDDEN );
	//}
	return nRet;
}

INT32 XFreeToolBar::DeleteXBtns( UINT32 uBtnIndex )
{
	ATLASSERT( uBtnIndex < GetButtonCount() );

	INT32 nRes;
	nRes = (INT32)::SendMessage( m_hWnd, TB_DELETEBUTTON, (WPARAM)uBtnIndex, (LPARAM)0);
	if( TRUE == nRes )
	{
		return XSUCCESS;
	}
	return XERROR_DELETE_BTN_FAILED;
}

INT32 XFreeToolBar::InitChevronBtn()
{
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 2 ].pBtnData->iBitmap = -1;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 2 ].pBtnData->idCommand = FILLED_BUTTON_ID;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 2 ].pBtnData->dwData = ( DWORD_PTR )&m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 2 ];
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 2 ].pBtnData->fsState = 0;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 2 ].pBtnData->fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;// | BTNS_SHOWTEXT;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 2 ].pBtnData->iString = -1;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 2 ].dwStyle = XFREEBAR_FILLSPACE_BTN;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 2 ].bRealHide = TRUE;

	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 1 ].pBtnData->iBitmap = -1;//CHEVRON_IMAGE_INDEX;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 1 ].pBtnData->idCommand = CHEVRON_BUTTON_ID;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 1 ].pBtnData->dwData = ( DWORD_PTR )&m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 1 ];
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 1 ].pBtnData->fsState = TBSTATE_ENABLED;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 1 ].pBtnData->fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;// | BTNS_SHOWTEXT;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 1 ].pBtnData->iString = -1;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 1 ].dwStyle = XFREEBAR_CHEVRON_BTN;
	m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - 1 ].bRealHide = TRUE;
	m_uBtnCount = 0;
	return 0;
}

#ifdef _DEBUG_CHEVRON
#define CHEVRON_BUTTON_TEXT "Chevron"
#define SPACE_FILLED_BUTTON_TEXT "Filled"
#endif

INT32 XFreeToolBar::AddChevronBtn()
{
	INT32 nRes;
	//DeleteChevronBtn();
	nRes = AddXBtns( CHEVRON_BUTTON_COUNT, &m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - CHEVRON_BUTTON_COUNT ], FALSE);
	//SetButtonText(18, SPACE_FILLED_BUTTON_TEXT);
	//SetButtonText(19, CHEVRON_BUTTON_TEXT);
	//TBInfoSetter(this, CHEVRON_BUTTON_ID, TRUE).image(-1).size(CHEVRON_BUTTON_WIDTH).state( TBSTATE_ENABLED ).text(CHEVRON_BUTTON_TEXT);
	//TBInfoSetter(this, FILLED_BUTTON_ID, TRUE).image(-1).text(SPACE_FILLED_BUTTON_TEXT);
	return nRes;
}

INT32 XFreeToolBar::DeleteChevronBtn()
{
	ATLASSERT(FALSE);
	UINT32 uFillBtnIndex;
	UINT32 uChevBtnIndex;

	uFillBtnIndex = (UINT32)MapBtnIndexFromID( FILLED_BUTTON_ID );
	uChevBtnIndex = (UINT32)MapBtnIndexFromID( CHEVRON_BUTTON_ID );

	ATLTRACE("The filled button index is %d\n", uFillBtnIndex);
	ATLTRACE("The chevron button index is %d\n", uChevBtnIndex);

	if(uFillBtnIndex != -1)
	{
		if(0 < SendMessage( m_hWnd, TB_DELETEBUTTON, uFillBtnIndex, 0 ))
			m_uBtnCount -= 1;
	}
	if(uChevBtnIndex != -1)
	{
		if(0 < SendMessage( m_hWnd, TB_DELETEBUTTON, uChevBtnIndex, 0 ))
			m_uBtnCount -= 1;
	}
	return 0;
}