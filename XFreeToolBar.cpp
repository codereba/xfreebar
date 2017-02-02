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
#include "XChevResponsor.h"

using namespace XML;

#define BUTTON_HEIGHT TB_HEIGHT
#define ICON_HEIGHT 16
#define FONT_HEIGHT 13
#define PADDING_WIDTH 2
#define DRAW_TEXT_OFFSET (INT32)( ( BUTTON_HEIGHT - FONT_HEIGHT ) / 2 )
#define PADDING_HEIGHT ( INT32 )( ( BUTTON_HEIGHT - ICON_HEIGHT ) )

#define CHEVRON_BUTTON_WIDTH 15
#define CHEVRON_BUTTON_ID ( INT32 )( WM_USER + MAX_BUTTON_COUNT )
#define FILLED_BUTTON_ID ( INT32 )( WM_USER + MAX_BUTTON_COUNT - 1)

//UINT32 XBTNINFO::uBtnCount = 0;

extern XString gAppendCommand;
extern XString gAppendCommandDisplay;

_bstr_t gurl;
XString g_last_url;
//static WNDPROC g_fSuperIEFrameProc = NULL;

// format of backgound for button icons
#if defined(_LOOKJET) || defined(_SEARCHHIT) || defined(_ADRRESSS_BAR_REDIRECT)
const INT32 maskClr = CLR_NONE;
#elif defined(_ODBC_DIRECT) || defined(_MASTER_BAR)
const INT32 maskClr = 0xff00ff;
#else
const INT32 maskClr = CLR_DEFAULT;
#endif

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

//XFreeToolBar *g_pXFreeBar = NULL;
// "main" crop
XFreeToolBar::XFreeToolBar() :
									m_bSplitFocused( FALSE ),
									m_bLBtnDown( FALSE ),
									m_pToolBarHost( NULL ),
									m_pXConfig( NULL ),
									m_nLastPresedButton( -1 ),
									m_hImageDll( NULL ),
									m_hTickerThread( 0 ),
									m_nCurWidth( 0 ),
									m_nMinWidth( 0 ),
									m_pChevDrop( NULL ),
									m_pChevResponsor( NULL ),
									m_nChevBtnIndex( -1 ),
									m_bToolbarInited( FALSE ),
									m_bLButtonDown( FALSE ),
									m_uBtnCount( 0 ),
									m_nDragingImage( 0 ),
									m_nDragingButton( -1 ),
									m_bHtmlRespInited( FALSE ), 
									m_RebarContainer( NULL, this, 1 )

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

	if( m_pChevResponsor != NULL )
	{
		delete m_pChevResponsor;
	}

	
	delete m_pXConfig; 
	m_pXConfig = 0;
	m_bToolbarInited = FALSE;

	DestroyXBtnInfos();
}

// here we store temp data that we generate during showing menus
//Show/Hide and Enable/Disable the hilight button, the input param can be the keyword number.
void XFreeToolBar::EnableHighlight(BOOL bEnable)
{
	INT32 nHilightIndex = m_pXConfig->MapBtnConfIndexFromCmd( _T("Highlight") );
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
		GetItemRect(i, &rc);
		m_nCurWidth += rc.right - rc.left;
		if(i < m_pXConfig->m_BtnConfs.size() && m_pXConfig->m_BtnConfs[i]->isShownAlways())
			m_nMinWidth = rc.right - rc.left;
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
		nRet = GetButton( i, &TBBtn );
		ATLASSERT( FALSE != nRet );
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
DWORD XFreeToolBarReflector::OnItemPrePaint(INT32 , LPNMCUSTOMDRAW pNmHdr)
{
	XToolBarConfig *pConfig;

	XToolBarConfig *pXConfig;
	pXConfig = m_pToolBarHost->GetXToolBar()->GetToolBarConfig();
	ATLASSERT( NULL != pXConfig );

	LPNMTBCUSTOMDRAW lpTBCustomDraw = (LPNMTBCUSTOMDRAW)pNmHdr;
	bool bDisabled = ((lpTBCustomDraw->nmcd.uItemState & CDIS_DISABLED) == CDIS_DISABLED);
	if(!bDisabled)
	{
		if(pXConfig) // on quit bug fix
			if(lpTBCustomDraw->nmcd.dwItemSpec-WM_USER<pXConfig->m_BtnConfs.size()) // generated words
				return pXConfig->m_BtnConfs[lpTBCustomDraw->nmcd.dwItemSpec-WM_USER]->OnItemPrePaint(&m_pToolBarHost->m_wndToolBarCtrl,lpTBCustomDraw);
	}
	return CDRF_DODEFAULT;
}
#endif

LRESULT XFreeToolBarReflector::OnNotifyRangeHandler( INT32, LPNMHDR lParam, BOOL &bHandled  )
{
	bHandled = FALSE;

	//bHandled = TRUE;
	return 0;
}

// process tooltip events
LRESULT XFreeToolBarReflector::OnToolTip( INT32, LPNMHDR lParam, BOOL &bHandled  )
{
	LPTOOLTIPTEXT lpTooltip;
	bHandled = FALSE;
	lpTooltip = ( LPTOOLTIPTEXT ) lParam;
	lpTooltip->hinst = _Module.GetModuleInstance();

	m_pToolBarHost->GetXToolBar()->GetToolTipInfo( lpTooltip->hdr.idFrom, &lpTooltip->lpszText );
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
		if( ::MessageBox(0,_T("A new version of the " + sProfileName + " IE ToolBarConfig is available.\nDo you want to update your current " + sProfileName + " IE ToolBarConfig ?"),XTOOLBAR_ALERT_TITLE,MB_YESNO)==IDYES)
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
#endif //CHECK_EACH_START
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
	
	*ppBtnConf = ( XToolbarItem* )pXBtnInfo->lpExtension;

	if( NULL == *ppBtnConf )
	{
		return XERROR_TOOLBAR_CONF_IS_NULL;
	}
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
	for( ULONG i = 0; i < m_pXConfig->m_Settings.m_fonts.size(); i++ )
	{
		if( FontsName.find( m_pXConfig->m_Settings.m_fonts[i]->m_strName ) != FontsName.end() )
		{
			m_Font.CreateFont( m_pXConfig->m_Settings.m_fonts[i]->m_FontNum, 0, 0, 0, FW_NORMAL, 0, 0, 0, nCharSet,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS, m_pXConfig->m_Settings.m_fonts[i]->m_strName.c_str());
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
	INT32 nBtnConfNum = m_pXConfig->m_BtnConfs.size();
	for(INT32 i = 0; i < nBtnConfNum; i++)
	{
		m_pXConfig->m_BtnConfs[i]->SetBtnCmdId( INIT_BTN_ID( i ) );
		m_pXConfig->m_BtnConfs[i]->SetBtnConfIndex( i );
		m_pXConfig->m_BtnConfs[i]->m_Command = m_pXConfig->FindCommandConf( m_pXConfig->m_BtnConfs[ i ]->m_strCmd );

		if( m_pXConfig->m_BtnConfs[i]->m_dwStyle == FIXED_RIGHT_SIDE_BTN )
		{
			m_pXBtnInfos[i].dwStyle = FIXED_RIGHT_SIDE_BTN;
			m_pXBtnInfos[i].bRealHide = TRUE;
		}
		else
		{
			m_pXBtnInfos[i].dwStyle = 0;
			m_pXBtnInfos[i].bRealHide = FALSE;
		}

		m_pXBtnInfos[i].lpExtension = ( LPVOID )m_pXConfig->m_BtnConfs[i];
		m_pXBtnInfos[i].pBtnData->iBitmap = GetImageId( m_pXConfig->m_BtnConfs[i]->GetConfImageIndex() );
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

	return 0;
}

#define TUNE_TOOLBAR_WIDTH_TIMER_ESCAPE 50
#define TUNE_TOOLBAR_WIDTH_TIMER 1080
static INT32 nStartTimerWidth = 0;
static INT32 nTimer = 0;

#define X_ERROR_PARSE_XML_CONF -31
// not good function name 
// really: yes build toolbar!!
INT32 XFreeToolBar::BuildToolBar()
{
	CWaitCursor wait;

	XToolBarUpdate();

	if( !ParseToolbarFromXML( m_pXConfig ) )
		return X_ERROR_PARSE_XML_CONF;

	if( XToolbarItem* item = m_pXConfig->MapBtnConfFromCmd( "search" ) )
		item->m_strCaption = gAppendCommandDisplay;

	setValue( _T( "showcorrupted" ), _T( "1" ) );

	setValue( _T( "gUpdate" ), _T( "0" ) );

	// is it time to update dll itself
#ifdef UPDATE_DLL
	XString cur_ver = getValue(_T("updateVer"),_T(""));
	if(!m_pXConfig->m_Settings.m_updateVer.empty() && !cur_ver.empty() && cur_ver!=m_pXConfig->m_Settings.m_updateVer) //update dll from server
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

	setValue( _T( "updateVer" ), m_pXConfig->m_Settings.m_updateVer );

	SetFonts();

	m_bToolbarInited = FALSE;

	//Delete all buttons.
	INT32 nBtnCount = GetButtonCount();
	for( INT32 i = nBtnCount - 1; i >= 0; i-- )
		DeleteXBtns( i );

	// build toolbar data
	SetButtonStructSize( sizeof( TBBUTTON ) );

	InitChevronBtn();

	SetExtendedStyle( TBSTYLE_EX_DRAWDDARROWS );

	InitNormBtn();
	// apply images and hot images lists to toolbar
	if(!m_pXConfig->m_bmpFileName.empty())
	{	
		XString filebmp = m_pXConfig->m_bmpFileName;

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

		XString filebmp2 = m_pXConfig->m_bmpHotFileName;
		if(!filebmp2.empty())
			CheckUpdate( filebmp2 );

		XString strDisbImgFile = m_pXConfig->m_bmpDisbFileName;
		if(!strDisbImgFile.empty())
			CheckUpdate( strDisbImgFile );

		m_NormList.Destroy();
		m_HotList.Destroy();
		m_DisbList.Destroy();

		m_NormList.CreateFromImage(filebmp.c_str(),bSize.cx,0,maskClr,IMAGE_BITMAP,LR_CREATEDIBSECTION|BMP_LOADFLAG);
		m_HotList.CreateFromImage(filebmp2.empty() ? filebmp.c_str():filebmp2.c_str(),bSize.cx,0,maskClr,IMAGE_BITMAP,LR_CREATEDIBSECTION|BMP_LOADFLAG);
		m_DisbList.CreateFromImage( strDisbImgFile.c_str(), bSize.cx, 0, maskClr, IMAGE_BITMAP, LR_CREATEDIBSECTION | BMP_LOADFLAG );
#else
#include "applyBimapFilename.inl"
#endif

		SetHotImageList(m_HotList);
		m_hImageList = m_NormList.m_hImageList;
		SetImageList(m_NormList);
		SetDisabledImageList( m_DisbList );
	}

	//Initialze the default toolbar settings.
	m_pXConfig->m_Settings.Init();

	AddLeftSideBtns();
	AddHilightBtns();
	AddChevronBtn();
	AddRightSideBtns();
	
	SetPadding( PADDING_WIDTH, PADDING_HEIGHT );
	AutoSize();

	INT32 nBtnConfNum = m_pXConfig->m_BtnConfs.size();
	for( INT32 i = 0; i < nBtnConfNum; i++ )
	{
		m_pXConfig->m_BtnConfs[i]->XChange( this );
	}

	m_pXConfig->OnOptionChange( this );

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

	//calculate current width of toolbar for correct chevrons behaiviour
	CalcXFreeBarWidth();
	SetBtnsWidth( 0, GetButtonCount() );

#ifdef _DEBUG //check that we don't forget about any commands
	for( INT32 wID = MENU_COMMAND_ID_BEGIN; wID < MENU_COMMAND_ID_END; wID++ )
	{
		IMenuItem *item = m_pXConfig->GetMenuItemOnButton( wID );
		if( item && !item->m_strCmd.empty() && m_pXConfig->FindCommandConf( item->m_strCmd ) == 0 )
			::MessageBox( 0,( _T( "Can't find command " ) + item->m_strCmd ).c_str(), XTOOLBAR_ALERT_TITLE, 0 );
	}
#endif

	DefWindowProc( WM_PAINT, 0, 0 );
	m_bToolbarInited = TRUE;

	RECT rcToolBar;
	GetClientRect( &rcToolBar );
	nStartTimerWidth = rcToolBar.right - rcToolBar.left;

	if( nTimer == 0 )
	{
		nTimer = ::SetTimer( m_hWnd, TUNE_TOOLBAR_WIDTH_TIMER, TUNE_TOOLBAR_WIDTH_TIMER_ESCAPE, NULL );
	}
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

#include "XAboutDlg.h"
LRESULT XFreeToolBar::OnAbout()
{
	XAboutDlg AboutDlg;
	AboutDlg.DoModal();

	return 0;
}

VOID ConvertToRegularFileName( LPTSTR lpszFileName )
{
	INT32 nLength;
	nLength = _tcslen( lpszFileName );

	for( INT32 i = 0; i < nLength; i ++ )
	{
		if( lpszFileName[ i ] == _T( ':' ) ||
			lpszFileName[ i ] == _T( '.' ))
		{
			lpszFileName[ i ] = _T( '_' );
		}
	}
	return;
}

INT32 GetServerNameFromURL( LPCTSTR lpszURL, LPTSTR lpszServerName, INT32 nMaxNameLen )
{
	INT32 nBeginSlash;
	INT32 nEndSlash;

	INT32 nURLLen;
	INT32 nFindedTime;
	nURLLen = strlen( lpszURL );

	nFindedTime = 0;

	nBeginSlash = -1;
	nEndSlash = -1;

	for( INT32 i = 0; i < nURLLen; i++ )
	{
		if( lpszURL[ i ] == _T('/') )
		{
			if( nFindedTime == 1 )
			{
				nBeginSlash = i;
			}
			else if (nFindedTime == 2 )
			{
				nEndSlash = i;
				break;
			}

			nFindedTime ++;
		}
	}

	if( nEndSlash != -1 )
	{
		if( nMaxNameLen > nEndSlash )
		{
			memcpy( lpszServerName, lpszURL + nBeginSlash + 1, nEndSlash - nBeginSlash - 1 );
			lpszServerName[nEndSlash - nBeginSlash - 1 ] = '\0';
		}
		return 0;
	}

	return -1;
}

INT32 CheckRegularIcon( HICON hIcon )
{
	ATLASSERT( NULL != hIcon );
	ICONINFO IconInfo;
	BITMAP Bmp;
	if( FALSE ==::GetIconInfo( hIcon, &IconInfo ) )
	{
		return -1;
	}

	if( FALSE == GetObject( IconInfo.hbmColor, sizeof( BITMAP ), &Bmp ) )
	{
		return -1;
	}

	if( Bmp.bmHeight > ICON_MAX_LEN || Bmp.bmWidth > ICON_MAX_LEN )
	{
		return -1;
	}
	return 0;
}

INT32 CheckRegularIcon( LPCTSTR lpszIconFile )
{
	HICON hIcon;
	hIcon = AtlLoadIconImage(lpszIconFile,LR_LOADFROMFILE);
	if( NULL != hIcon )
	{
		return CheckRegularIcon( hIcon );
	}

	return -1;
}

LRESULT XFreeToolBar::OnConfig()
{
	INT32 nRet = 0;
	//_bstr_t strTitle = _T("XFree Bar Custom");
	TCHAR szCurLinkUrl[MAX_URL_LENGTH];
	TCHAR szServerName[MAX_PATH] = { 0 };
	TCHAR szFavIconURL[MAX_PATH] = { 0 };
	TCHAR szFavIconFile[MAX_PATH] = { 0 };


	HWND hWnd( NULL );
	HICON hTitleIcon;
	CXToolBarOptionDlg OptionDlg;

	nRet = GetWebBrowserCurUrl( m_pToolBarHost->m_pWebBrowser, szCurLinkUrl,MAX_URL_LENGTH );
	if( 0 > nRet )
	{
		goto show_config_dlg;
	}

	nRet = GetServerNameFromURL( szCurLinkUrl, szServerName, MAX_PATH );
	if( 0 > nRet )
	{
		goto show_config_dlg;
	}

	_tcscat( szFavIconURL, "http://" );
	_tcscat( szFavIconURL, szServerName );
	_tcscat( szFavIconURL, "/" );
	_tcscat( szFavIconURL, "favicon.ico" );

	ConvertToRegularFileName( szServerName );
	GetXmlConfDir( szFavIconFile, MAX_PATH );
	_tcscat( szFavIconFile, szServerName );
	_tcscat( szFavIconFile, _T( ".ico" ) );
	

	if( 0 == DownloadWebSiteFavIcon( szFavIconURL, szFavIconFile ) )
	{
		if( 0 == CheckRegularIcon( szFavIconFile ) )
		{
			OptionDlg.SetCurSiteFavIcon( szFavIconFile );
		}
	}
	else
	{
		m_pToolBarHost->m_pWebBrowser->get_HWND(( LONG* ) &hWnd );
		if( NULL != hWnd )
		{
			hTitleIcon = (HICON)::SendMessage( ::GetParent( hWnd ), WM_QUERYDRAGICON, TRUE, 0 );
		}

		if( NULL != hTitleIcon )
		{
			if( 0 == CheckRegularIcon( hTitleIcon ) )
			{
				OptionDlg.SetTitleIcon( hTitleIcon );
			}
		}
	}

show_config_dlg:
	if( OptionDlg.DoModal() == IDOK )
	{
		g_ToolWillReload = TRUE;
	}

	return nRet;
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

INT32 XFreeToolBar::CursorIsOnChevronMenu()
{
	if( m_pChevDrop == NULL )
	{
		return 0;
	}

	if( m_pChevDrop->m_hWnd == NULL )
	{
		return 0;
	}

	POINT ptCursor;
	RECT rcChevDrop;
	m_pChevDrop->GetWindowRect( &rcChevDrop );
	::GetCursorPos( &ptCursor );

	if( ptCursor.x >= rcChevDrop.left && ptCursor.x <= rcChevDrop.right 
		&& ptCursor.y >= rcChevDrop.top && ptCursor.y <= rcChevDrop.bottom )
	{
		return 1;
	}

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
		return 0;
	}

	XToolbarItem* pBtnConf;
	if( CursorIsOnChevronMenu() )
	{
		nBtnID = pNMTB->iItem;
		nBtnIndex = MapBtnIndexFromID( nBtnID );
	}
	else
	{
		DWORD dwPoint = ::GetMessagePos();
		POINT ptCur;
		POINT ptDrag;
		//INT32 nSepHited;
		ptCur.x = GET_X_LPARAM(dwPoint);
		ptCur.y = GET_Y_LPARAM(dwPoint);
		ptDrag = ptCur;

		ScreenToClient(&ptCur);

		nBtnIndex = HitTest( &ptCur );
		if( nBtnIndex < 0 )
		{
			bHandled = FALSE;
			return 0;
		}
	}

	pBtnConf = GetBtnConf( nBtnIndex );

	if( pBtnConf == NULL )
	{
		bHandled = FALSE;
		return 0;
	}

	m_nDragingButton = -1;
	pBtnConf->ShowMenu( this, m_hWnd, rcBtn.left, rcBtn.bottom );
	WhenLButtonUp();

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

	for(UINT32 i = 0; i < nBtnCount; i++)
	{
		tbi.cbSize = sizeof TBBUTTONINFO;
		tbi.dwMask = TBIF_STATE | TBIF_SIZE | TBIF_COMMAND;
		INT32 ret = pToolbar->GetButtonInfo( WM_USER + i, &tbi);
		RECT rc;
		pToolbar->GetItemRect(i, &rc);

		if(rc.left <= (ToolbarRc.right - CHEVRON_WIDTH) && rc.right >= (ToolbarRc.right - CHEVRON_WIDTH) )
		{
			return i;
		}
	}
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
	NMREBARCHEVRON* pChevron = (NMREBARCHEVRON*)pNmHdr;

	POINT ChevronPt;
	CalcChevronPos(m_hWnd, &pChevron->rc, &ChevronPt);
		RECT ToolbarRc;
	GetClientRect(&ToolbarRc);
	ScreenToClient(&ToolbarRc);

	if(m_pChevDrop == NULL)
	{
		m_pChevDrop = new CChevDrop();
		m_pChevResponsor = new XChevResponsor( this, m_pChevDrop );
		m_pChevDrop->SetChevResponsor( m_pChevResponsor );

		if(m_pChevDrop != NULL)
			m_pChevDrop->CreatePopup(m_pToolBarHost->GetInvisibleWnd());
	}

	INT32 nAllBtnCount = GetButtonCount();
	INT32 nOtherBtnCount = nAllBtnCount - m_nChevBtnIndex;
	BOOL *pButtonsHide = new BOOL[ nOtherBtnCount ];
	XBTNINFO *pXBtnInfo;
	for( INT32 i = m_nChevBtnIndex; i < nAllBtnCount; i++ )
	{

		pButtonsHide[ i - m_nChevBtnIndex ] = IsRealHideBtn( i );

	}

	if(m_pChevDrop != NULL)
	{
		m_pChevDrop->ShowPopup(m_pToolBarHost->GetInvisibleWnd(), this->m_hWnd, 120, 25, m_nChevBtnIndex, CHEVRON_BUTTON_COUNT, pButtonsHide, ChevronPt);
		delete[] pButtonsHide;
	}

	return 0;
}

// pre process keyboard input 
STDMETHODIMP XFreeToolBar::TranslateAcceleratorIO(LPMSG pMsg)
{
	for(IBandEdits Iter( m_pXConfig->m_vecCombos.size(), m_pXConfig->m_vecCombos.begin() );Iter;Iter++)
		if (Iter->IsFocus(GetFocus()))
			return Iter->TranslateAcceleratorIO(pMsg);
	return S_FALSE;
}

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
		for(IBandEdits Iter( m_pXConfig->m_vecCombos.size(), m_pXConfig->m_vecCombos.begin() );Iter;Iter++)
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
			if(m_pXConfig->m_vecCombos.size())
				m_pXConfig->m_vecCombos[0]->SetFocus();
			bHandled = TRUE;
		}
	}

	return S_OK;
}

// catch mouse over event
LRESULT XFreeToolBar::OnHookMouseMove(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	if( FALSE == m_bToolbarInited )
	{
		return 0;
	}

	RECT rcToolbar;
	DWORD dwPoint = ::GetMessagePos();
	POINT ptCur;
	POINT ptDrag;

	ptCur.x = GET_X_LPARAM(dwPoint);
	ptCur.y = GET_Y_LPARAM(dwPoint);
	ptDrag = ptCur;

	GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	ScreenToClient(&ptCur);

	INT32 nHit = HitTest(&ptCur);

	rcToolbar.left = rcToolbar.right - CHEVRON_BUTTON_WIDTH;
	if(ptCur.x >= rcToolbar.left &&
		ptCur.x <= rcToolbar.right &&
		ptCur.y >= rcToolbar.top &&
		ptCur.y <= rcToolbar.bottom)
	{
		SendMessage( m_hWnd, WM_PAINT, 0, 0 );
	}

	static INT32 nSplitterIndex;

	bHandled = FALSE;

	if( m_bLBtnDown )
	{
		m_pXConfig->m_splitters[ nSplitterIndex ]->StopMouseEvents(this);
		return S_OK;
	}

	if( m_pXConfig && nHit >= 0 && nHit < m_pXConfig->m_BtnConfs.size() )
	{
		m_bSplitFocused = false;
		return S_OK;
	}

	for(ULONG i = 0; i < m_pXConfig->m_splitters.size(); i++)
	{
		ATLTRACE( "The hited button id is %d the item id is %d\n", nHit, m_pXConfig->m_splitters[ i ]->GetBtnCmdId() );
		if( MapBtnIndexFromID( m_pXConfig->m_splitters[ i ]->GetBtnCmdId() ) ==  -( nHit + 1 ) )
		{
			m_bSplitFocused = true;
			nSplitterIndex = i;
			return S_OK;
		}
	}

	m_bSplitFocused = false;
	return S_OK;
}

LRESULT XFreeToolBar::OnHookSetCursor(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	if(m_bSplitFocused || m_bLBtnDown)
	{
		SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		bHandled = TRUE;
	}
	else if(m_nDragingButton != -1)
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
	if( wParam == TUNE_TOOLBAR_WIDTH_TIMER )
	{
		INT32 nToolBarWidth;
		RECT rcToolBar;

		::GetClientRect( m_hWnd, &rcToolBar );

		if( nStartTimerWidth != rcToolBar.right - rcToolBar.left )
		{
			nStartTimerWidth = rcToolBar.right - rcToolBar.left;
			return 0;
		}

		nToolBarWidth = rcToolBar.right - rcToolBar.left; 

		ATLTRACE( "Calc bar width is %d\n", nToolBarWidth );

		TuneToolBarSizeInBorder( nToolBarWidth );
		::KillTimer(m_hWnd, TUNE_TOOLBAR_WIDTH_TIMER );
		nTimer = 0;
	}
	
	bHandled = TRUE;
	return 0;
}

LRESULT XFreeToolBar::OnHookPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	INT32 nRet;
	CPaintDC DrawDC( m_hWnd );
	CPen TBPen;
	CPenHandle OldPen;
	CBrush TBBrush;
	CBrushHandle OldBr;
	CFontHandle OldFont;
	INT32 nOldMode;
	TBBUTTON TBInfo;
	INT32 nBtnCount;
	TCHAR pszText[MAX_PATH];

	RECT rcBtn;
	INT32 nHited;
	HIMAGELIST hImgList;
	HIMAGELIST hHotImgList;
	HIMAGELIST hDisbImgList;
	CBrush BkBr;

	if( FALSE == m_bToolbarInited )
	{
		nRet = XERROR_TOOLBAR_IS_NOT_INITED;
		goto OnPaint_Exit;
	}

	nRet = 0;

	hImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETIMAGELIST, 0, 0L);
	hHotImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETHOTIMAGELIST, 0, 0L);
	hDisbImgList = ( HIMAGELIST )::SendMessage( m_hWnd, TB_GETDISABLEDIMAGELIST, 0, 0L );

	if(NULL == hImgList)
	{
#define GET_IMAGELIST_ERR_ON_PAINT -3

		nRet = GET_IMAGELIST_ERR_ON_PAINT;
		goto OnPaint_Exit;
	}

	IMAGEINFO TmpImgInfo;
	INT32 nVOffset;
	INT32 nHOffset;
	RECT rcImg;
	CPoint ptCur;
	if( FALSE == ::GetCursorPos( &ptCur ) )
	{
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


	nHited = ::SendMessage( m_hWnd, TB_HITTEST, 0, (LPARAM)&ptCur );

	nBtnCount = ::SendMessage( m_hWnd, TB_BUTTONCOUNT, 0, 0L );
	TBBUTTONINFO TmpBtnInfo;
	OldFont = DrawDC.SelectFont( GetFont() );
	nOldMode = DrawDC.SetBkMode( TRANSPARENT );

	for( INT32 i = 0; i < nBtnCount; i ++ )
	{
		if(GetButton(i, &TBInfo) == FALSE)
		{
			continue;
		}
		dwIDCmd = TBInfo.idCommand;

		TBInfoGetter(this, dwIDCmd, &TmpBtnInfo, TRUE).style().image().state(); 
		
		uState = TmpBtnInfo.fsState;
		if(uState & TBSTATE_HIDDEN)
		{
			continue;
		}

		uStyle = TmpBtnInfo.fsStyle;
		uImg = TmpBtnInfo.iImage;

		GetItemRect( i, &rcBtn );
	
		if(uStyle & BTNS_SEP)
		{
			if( rcBtn.left > rcBtn.right )
			{
				INT32 nLeft;
				nLeft = rcBtn.left;
				rcBtn.right = rcBtn.left;
				rcBtn.left = nLeft;
			}

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

		rcBtn.bottom -= 1;
		rcBtn.right -= 1;

		if(dwIDCmd == FILLED_BUTTON_ID)
		{
			continue;
		}

		if(dwIDCmd == CHEVRON_BUTTON_ID)
		{
			uImg = -1;
			*pszText = 0;
		}
		else
		{
			*pszText = _T('\0');
			GetButtonText( dwIDCmd, pszText );
		}

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

			}
		}

		if(uStyle & BTNS_WHOLEDROPDOWN || uStyle & BTNS_DROPDOWN)
		{
			DrawArrow(DrawDC, rcBtn);
		}

		if( uImg >= 0 )
		{
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
		}
		else
		{
			nHOffset = 4;
		}

		if(dwIDCmd == CHEVRON_BUTTON_ID)
		{
			DrawChevronSign(DrawDC, rcBtn.left + 3, rcBtn.top + 4);
		}
		else if( 0 <= uImg && !(uStyle & BTNS_SEP) )
		{
			if( ( uState & TBSTATE_ENABLED ) == 0 && NULL != hDisbImgList )
			{
				ImageList_DrawEx(hDisbImgList, uImg, DrawDC.m_hDC,  rcBtn.left + nHOffset, rcBtn.top + nVOffset, (rcImg.right - rcImg.left), (rcImg.bottom - rcImg.top), dwBtnBkClr, dwBtnFrClr, ILD_TRANSPARENT);
			}
			else if( i == nHited)
				ImageList_DrawEx(hHotImgList, uImg, DrawDC.m_hDC,  rcBtn.left + nHOffset, rcBtn.top + nVOffset, (rcImg.right - rcImg.left), (rcImg.bottom - rcImg.top), dwBtnBkClr, dwBtnFrClr, ILD_TRANSPARENT);
			else
				ImageList_DrawEx(hImgList, uImg, DrawDC.m_hDC,  rcBtn.left + nHOffset, rcBtn.top + nVOffset, (rcImg.right - rcImg.left), (rcImg.bottom - rcImg.top), dwBtnBkClr, dwBtnFrClr, ILD_TRANSPARENT);
		}

#define X_TOOLBAR_ENABLED_TEXT_CLR RGB( 0, 0, 0 )
#define X_TOOLBAR_DISABLED_TEXT_CLR RGB( 160, 160, 160 )

		if( CHEVRON_BUTTON_ID != dwIDCmd )
		{
			DWORD dwOldTextClr;
			if( 0 == ( uState & TBSTATE_ENABLED ) )
			{
				dwOldTextClr = DrawDC.SetTextColor( X_TOOLBAR_DISABLED_TEXT_CLR );
			}

			if( uImg >= 0 )
			{
				DrawDC.TextOut( rcBtn.left + (rcImg.right - rcImg.left) + 1 + nHOffset, rcBtn.top + DRAW_TEXT_OFFSET,pszText, //sizeof( szText ),
					-1
					);
			}
			else
			{
				DrawDC.TextOut( rcBtn.left + nHOffset + 1, rcBtn.top + DRAW_TEXT_OFFSET, pszText, //sizeof( szText ),
					-1
					);
			}

			if( 0 == ( uState & TBSTATE_ENABLED ) )
			{
				DrawDC.SetTextColor( dwOldTextClr );
			}
		}
	}

#define DRAG_TIP_LINE_WIDTH 3
	RECT rcDragButton;
	LONG lLineX;
	if(m_bLButtonDown && m_nDragingButton != -1 )
	{
		if(nHited >= DRAG_IMAGE_BEGIN_INDEX && nHited <= DRAG_IMAGE_END_INDEX)
		{
			lLineX = -1;
			GetItemRect(nHited, &rcDragButton);
			if( nHited > m_nDragingButton )
			{
				lLineX = rcDragButton.left;
			}
			else if ( nHited < m_nDragingButton )
			{
				lLineX = rcDragButton.right - DRAG_TIP_LINE_WIDTH;
			}
			if(lLineX >= 0)
			{
				DrawMoveDepartLine(DrawDC, lLineX, rcDragButton.top, DRAG_TIP_LINE_WIDTH, rcDragButton.bottom - rcDragButton.top);
			}
		}
	}
	if(NULL != OldPen.m_hPen) DrawDC.SelectPen( OldPen );
	if(NULL != OldBr.m_hBrush) DrawDC.SelectBrush( OldBr );
	DrawDC.SelectFont( OldFont );
	DrawDC.SetBkMode(nOldMode);

OnPaint_Exit:
	bHandled = TRUE;
	return nRet;
}

LRESULT XFreeToolBar::OnHookLButtonDown(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;

	if(m_bSplitFocused)
	{
		m_bLBtnDown = true;
		SetCapture();
	}
	WhenLButtonDown();

	DWORD dwPoint;
	dwPoint = ::GetMessagePos();
	POINT ptCur = { GET_X_LPARAM(dwPoint), GET_Y_LPARAM(dwPoint) };
	POINT ptDrag;

	ptCur.x = GET_X_LPARAM(dwPoint);
	ptCur.y = GET_Y_LPARAM(dwPoint);
	ptDrag = ptCur;

	ScreenToClient(&ptCur);
	UINT32 nHit = HitTest(&ptCur);
	if(m_pXConfig && nHit>=0 && nHit<m_pXConfig->m_BtnConfs.size())
	{
		bHandled = m_pXConfig->m_BtnConfs[nHit]->StopMouseEvents();
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
			XLOG( ( 0, _T( "Set the draggin button index is %d\r\n" ), m_nDragingButton ) );
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

INT32 XFreeToolBar::WhenLButtonDown()
{
	m_bLButtonDown = TRUE;
	SetCapture();
	return 0;
}

INT32 XFreeToolBar::WhenLButtonUp()
{
	m_bLButtonDown = FALSE;
	XLOG( ( 0, _T( "Reset the dragging button index\r\n" ), m_nDragingButton ) );
	ReleaseCapture();
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

	m_bLBtnDown = FALSE;
	m_bSplitFocused = FALSE;

	dwPoint = ::GetMessagePos();
	ptCur.x = GET_X_LPARAM(dwPoint);
	ptCur.y = GET_Y_LPARAM(dwPoint);
	ptDrag = ptCur;
	ScreenToClient(&ptCur);

	nHit = HitTest(&ptCur);
	nRet = GetItemRect(nHit, &rcItem);

	if(m_pXConfig && nHit>=0 && nHit<m_pXConfig->m_BtnConfs.size())
	{
		bHandled = m_pXConfig->m_BtnConfs[nHit]->StopMouseEvents();
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
		
		LRESULT lRes = ::SendMessage(m_hWnd, TB_MOVEBUTTON, m_nDragingButton, nHit);
		ATLTRACE("End Move Source Index: %d, Source Command ID :%d,Destination Index: %d, Destination Command ID: %d\n",
			m_nDragingButton, MapBtnIDFromIndex(m_nDragingButton), nHit, MapBtnIDFromIndex(nHit));
	}

	WhenLButtonUp();
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
                        
     for( INT32 i = 0; i < m_pXConfig->m_Settings.m_Settings.size(); i++ )
       m_pXConfig->m_Settings.m_Settings[i]->OnSelChange(p,(const TCHAR*)_bstr_t(id));
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
	nNormalButtonCount = (INT32)m_pXConfig->m_BtnConfs.size();

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
	if( m_pXConfig->m_BtnConfs[ nIndex ]->GetToolbarItemType() == XTYPE_SPLITTER )
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
	INT32 nRet;
	INT32 nBtnCount;
	CRect rcToolBar;
	INT32 nLeftBtnWidth;
	INT32 nRightBtnWidth;
	TBBUTTON *pRemainBtn;
	INT32 nIndex;
	INT32 nClientWidth;
	INT32 nChevBtnIndex;
	INT32 nFillBtnIndex;
	INT32 nFillBtnWidth;

	nRet = XSUCCESS;

	if( FALSE == m_bToolbarInited )
	{
		nRet = -21;
		goto Exit_DepartToolbar;
	}

	if( FALSE == GetClientRect( &rcToolBar ) )
	{
		nRet = -26;
		goto Exit_DepartToolbar;
	}
	nClientWidth = rcToolBar.Width();

	m_nChevBtnIndex = -1;
	nLeftBtnWidth = 0;

	nRightBtnWidth = CalcFixedRightSideBtnWidth();
	ATLASSERT( nRightBtnWidth >= 0 );

	nBtnCount = GetButtonCount();

	nChevBtnIndex = MapBtnIndexFromID( CHEVRON_BUTTON_ID );
	nFillBtnIndex = MapBtnIndexFromID( FILLED_BUTTON_ID );

	RealHideBtn( nFillBtnIndex, TRUE, -1);
	RealHideBtn( nChevBtnIndex, TRUE, -1);

	for( nIndex = 0; nIndex < nBtnCount; nIndex++ )
	{
		if( FALSE == IsRealHideBtn( nIndex ) )
		{
			if( nLeftBtnWidth + GetBtnRealWidth( nIndex ) > nClientWidth - CHEVRON_BUTTON_WIDTH - TOOLBAR_RESERVE_SPACE - nRightBtnWidth )
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

#define  SIZE_FILL_SPACE_BTN_TIME 2

	if(m_nChevBtnIndex != -1)
	{
		nFillBtnWidth = nClientWidth - nLeftBtnWidth - CHEVRON_BUTTON_WIDTH - nRightBtnWidth - TOOLBAR_RESERVE_SPACE;

		RealHideBtn( nChevBtnIndex, FALSE, -1);
		TBInfoSetter( this, CHEVRON_BUTTON_ID, TRUE ).size( CHEVRON_BUTTON_WIDTH );
		
		ATLASSERT( nFillBtnWidth >= 0 );
		if( nFillBtnWidth > 0 )
		{
			RealHideBtn( nFillBtnIndex, FALSE, -1);
			TBInfoSetter( this, FILLED_BUTTON_ID, TRUE ).size( nFillBtnWidth );
		}

		ATLTRACE( "Filled button width is %d\n", nFillBtnWidth );

		
	}
	else
	{
		RealHideBtn( nChevBtnIndex, TRUE, -1);
		RealHideBtn( nFillBtnIndex, FALSE, -1);
		TBInfoSetter( this, FILLED_BUTTON_ID, TRUE ).size( nClientWidth - nLeftBtnWidth - nRightBtnWidth - TOOLBAR_RESERVE_SPACE );
	}

	ATLASSERT(  m_nChevBtnIndex < nBtnCount );

Exit_DepartToolbar:
	return nRet;
}

LRESULT XFreeToolBar::OnHookSize(UINT32 uMsg, UINT wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = TRUE;

	DefWindowProc( uMsg, wParam, lParam );
#ifdef _SERVAGE
	if(m_pXConfig && m_pXConfig->m_BtnConfs.size() && size.cx)
	{
		((Page*)( m_pXConfig->m_BtnConfs[0] ) )->m_dwWidth = size.cx;
		OnOptionChange();
	}
#endif

	if( wParam == SIZE_MINIMIZED || wParam == SIZE_MAXSHOW || wParam == SIZE_MAXHIDE )
	{
		return 0;
	}

	RECT rcToolBar;
	::GetClientRect( m_hWnd, &rcToolBar );

	nStartTimerWidth = rcToolBar.right - rcToolBar.left;

	if( nTimer == 0 )
	{
		nTimer = ::SetTimer( m_hWnd, TUNE_TOOLBAR_WIDTH_TIMER, TUNE_TOOLBAR_WIDTH_TIMER_ESCAPE, NULL );
	}
	return 0;
}

INT32 XFreeToolBar::TuneToolBarSizeInBorder( INT32 nToolBarWidth )
{
	INT32 nLeftBtnWidth;
	INT32 nRightBtnWidth;
	INT32 nMinLeftWidth;
	INT32 nSizedWidth;
	RECT rcCombo;
	INT32 nBtnCmdID;
	INT32 nBtnIndex;
	IXEditCtrl* pEdit;

	if ( FALSE == m_bToolbarInited )
		return 0;

	nSizedWidth = nToolBarWidth;

	nLeftBtnWidth = CalcFixedLeftSideBtnWidth();
	nRightBtnWidth = CalcFixedRightSideBtnWidth();
	ATLASSERT( 0 < nLeftBtnWidth );
#define OTHER_FILLED_SPACE 10
	nMinLeftWidth = nLeftBtnWidth + CHEVRON_BUTTON_WIDTH + TOOLBAR_RESERVE_SPACE + nRightBtnWidth + OTHER_FILLED_SPACE;

	IBandEdits Iter( m_pXConfig->m_vecCombos.size(), m_pXConfig->m_vecCombos.begin() );
	pEdit = (IXEditCtrl* )Iter;
	nBtnCmdID = pEdit->m_pComboHost->GetBtnCmdId();
	nBtnIndex = MapBtnIndexFromID( nBtnCmdID );
	GetItemRect( nBtnIndex, &rcCombo );

	if( nSizedWidth == 0 )
		return 0;

	if(rcCombo.left == 0 )
	{
		DepartToolBar();
		return 0;
	}
 
	rcCombo.bottom = rcCombo.top + COMBOBOX_HEIGHT;

	rcCombo.right = nSizedWidth - nMinLeftWidth + rcCombo.left;

	if( ( rcCombo.right - rcCombo.left ) < COMBOBOX_MIN_WIDTH )
	{
		return 0;
	}

	if( ( rcCombo.right - rcCombo.left ) > DEFAULT_COMBOBOX_WIDTH )
	{
		rcCombo.right = rcCombo.left + DEFAULT_COMBOBOX_WIDTH;
	}

	TBInfoSetter( this, nBtnCmdID, TRUE ).size( rcCombo.right - rcCombo.left );

	pEdit->MoveWindow( &rcCombo );

	SetBtnsWidth( nBtnIndex, nBtnIndex + 1 );

	CalcXFreeBarWidth( );
	DepartToolBar();
	return 0;
}

LRESULT XFreeToolBar::OnCommand(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(hWndCtl) // fix for combo box - don't know how to make it better
	{
		bHandled = false;
		return S_OK;
	}

	if( IMenuItem* item = m_pXConfig->GetMenuItemOnButton( wID ) )
	{
		if( Command* c = m_pXConfig->FindCommandConf( item->m_strCmd ) )
			c->apply(this);
	}
	return S_OK;
}

INT32 XFreeToolBar::OnHtmlMouseBtnDown( LPVOID pParam )
{
	UpdateSearchedWords();
	return 0;
}
#endif

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
		if(m_nDragingButton != -1 && m_nDragingButton != (INT32)nBtnIndex)
		{
			bHandled = FALSE;
			return XSUCCESS;
		}
	}

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
	{
		m_nLastPresedButton = nBtnIndex;
		if( Command* command = pBtnConf->m_Command )
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

BOOL XFreeToolBar::SubclassRebarHost()
{
	HWND hWnd(NULL);
	static WNDPROC OldRebarProc = NULL;
	m_pToolBarHost->m_pWebBrowser->get_HWND((long*)&hWnd);

	if(hWnd == NULL)
		return FALSE;

	m_ctrlRebar.m_hWnd = FindRebar( hWnd );
	if( m_ctrlRebar.m_hWnd == NULL )
		return FALSE;

	WNDPROC CurWndProc;

	CurWndProc = ( WNDPROC )::GetWindowLong( m_ctrlRebar.m_hWnd, GWL_WNDPROC );
	if( OldRebarProc == NULL )
	{
		OldRebarProc = CurWndProc;
	}

	if( OldRebarProc == CurWndProc )
	{
	}

	return TRUE;
}

BOOL XFreeToolBar::UnsubclassRebarHost()
{
	XLOG( ( 0, "XFreeToolBar::UnsubclassRebarHost The windows handle of the rebar is 0x%x\n", m_RebarContainer.m_hWnd ) );

	if( NULL != m_RebarContainer.m_hWnd )
	{
		::SetWindowLongPtr(m_RebarContainer.m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_RebarContainer.m_pfnSuperWindowProc);
		// clear out window handle
		m_RebarContainer.m_hWnd = NULL;
	}
	else
	{
		if( NULL != m_RebarContainer.m_pfnSuperWindowProc )
		{
			ATLTRACE( _T( "container process is %x\r\n" ), m_RebarContainer.m_pfnSuperWindowProc );
			if( ::IsWindow( m_ctrlRebar.m_hWnd ) )
			{	
				ATLTRACE( _T( "Window process is %x, container process is %x\r\n" ), ::GetWindowLongPtr( m_ctrlRebar.m_hWnd, GWLP_WNDPROC), m_RebarContainer.m_pfnSuperWindowProc );
			}
		}
	}

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
INT32 XFreeToolBar::UpdateSearchedWords()
{
	//return;
	XString strAllKeyWords;
	INT32 nKeyWordCount;

	nKeyWordCount = m_VectorAllFindKeyWord.size();
	//Enumerate all edits on the toolbar
	for( IBandEdits Iter( m_pXConfig->m_vecCombos.size(), m_pXConfig->m_vecCombos.begin() ); Iter; Iter++ )
		strAllKeyWords += Iter->GetText() + " ";

	// Get the text in the edit box
	replaceSpace( strAllKeyWords, m_VectorAllFindKeyWord );

	EnableHighlight(m_VectorAllFindKeyWord.size());

#ifdef FIND_MENU
#endif
#ifndef NO_FIND_BUTTTONS
	if(!m_pXConfig->m_Settings.m_highlight.m_colors.size())
		return XSUCCESS;

	INT32 nNormalButtonCount = m_pXConfig->m_BtnConfs.size(); //Calc the number of additional buttons

#define CHEVRON_FUNC_BUTTON_COUNT 2

	INT32 nNewAdded =  m_VectorAllFindKeyWord.size() - nKeyWordCount;//(nAllButtonCount - nNormalButtonCount);

	for(UINT32 uIndex = nNormalButtonCount + nKeyWordCount - 1; uIndex  >= nNormalButtonCount + m_VectorAllFindKeyWord.size(); uIndex --)
	{
		ATLTRACE(_T("Hide Highlight Button %d\n"), uIndex);
		RealHideBtn( uIndex, TRUE, TRUE);
	}

	if(nNewAdded > 0)
	{
		for( INT32 i = 0; i < nNewAdded; i++ )
		{
			ATLTRACE( _T("Enable Highlight Button %d\n"), nNormalButtonCount + i );
			RealHideBtn( nNormalButtonCount + i ,FALSE, FALSE );
		}
	} 

	BOOL bHide = !isGetValue( _T( "ShowFindButtons" ) ) || m_VectorAllFindKeyWord.size() == 0;

	for( UINT32 i = 0; i < m_VectorAllFindKeyWord.size(); i ++ )
	{
		SetHilightBtnText( nNormalButtonCount + i , const_cast<TCHAR*>( m_VectorAllFindKeyWord[i].c_str() ));
		RealHideBtn( nNormalButtonCount + i, bHide, bHide); //Second parameter is the idCommand member of TBBUTTON
	}

	CalcXFreeBarWidth();
	SetBtnsWidth( nNormalButtonCount, nNormalButtonCount + m_VectorAllFindKeyWord.size() );
	DepartToolBar();
	return XSUCCESS;
#endif
}

// process search (after Enter press or "Go" button)
void XFreeToolBar::RunSearch()
{
#ifdef SELECT_SEARCH_FROM_MENU
	if(Commands::Command* c  = m_pXConfig->FindCommandConf(gAppendCommand+"Home"))
		c->apply(this);
	return;
#endif

	INT32 idxCur = 0;
	INT32 idx = XString(getValue(_T("SearchUsing"),_T("0")))[0] - '0';

#ifdef SELECT_SEARCH_FROM_COMBO
	for(XML::vectorDel<Command*>::iterator c=m_pXConfig->m_commands.m_commands.begin();c!=m_pXConfig->m_commands.m_commands.end();c++)
	{
#else
	for(UINT32 b=0;b<m_pXConfig->m_BtnConfs.size();b++)
	{
		XString command = m_pXConfig->m_BtnConfs[b]->m_strCmd;
		Command* c0 = (m_pXConfig->FindCommandConf(command));
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
	XToolbarItem *pItem = m_pXConfig->MapBtnConfFromCmd( strCommand );
	TCHAR szHintAppend[_MAX_PATH];

	pItem->UpdateHintMsg( itoa( uNewInfo, szHintAppend, 10 ) );
}

bool XFreeToolBar::parseXML(XString& path )
{
	m_pXConfig->m_include_xmls.clear();

	if(!CheckUpdate(path))
		return false;

	XML::XmlParserImpl parser(m_pXConfig,_T("TOOLBAR"));
	parser.parse(path.c_str());

	vector<XString> include_xmls(m_pXConfig->m_include_xmls);

	for(UINT32 i=0;i<include_xmls.size();i++)
	{
		if(parseXML(include_xmls[i])==false)
		{
			DeleteFile(path.c_str());
			return false;
		}
	}

	if(m_pXConfig->m_bmpFileName.empty())
	{
		DeleteFile(path.c_str());
		return false;
	}
	return true;
}

// load XML files, other media files and check that we load them all
bool XFreeToolBar::ParseToolbarFromXML( bool breadreg)
{
	XString filexml;
	if( breadreg )
		filexml = "";
	else
		filexml = XML_CONF_FILE_NAME;

	if( m_pXConfig )
	{
		delete m_pXConfig; 
		m_pXConfig = NULL;
	}

	m_pXConfig = new XToolBarConfig();

#ifndef USE_RESOURCE_FILES
	if(!parseXML(filexml))
	{
		return false;
	}
#else
	if(HRSRC hResource = FindResource( _Module.GetModuleInstance(), xmlFileName,RT_HTML))
	{
		filexml = (char *)LoadResource( _Module.GetModuleInstance(), hResource );
		XML::XmlParserImpl parser(m_pXConfig,_T("TOOLBAR"));
		parser.parse(filexml.c_str(),false);
	}
#endif

#ifdef _MASTER_BAR
#include "Toolband27\showAdditionalBar.inl"
#endif
#ifdef _ODBC_DIRECT
#include "Toolband7\appendMoreButtons.inl"
#endif

	return m_pXConfig->check();
}

INT32 XFreeToolBar::OnHtmlPageDownloaded( VOID *pParam )
{
	ATLASSERT( NULL != pParam );
	m_bHtmlRespInited = FALSE;
	IHTMLDocument3Ptr *ppHtmlDoc;
	ppHtmlDoc = ( IHTMLDocument3Ptr* ) pParam;
	
	for( ULONG i = 0; i < m_pXConfig->m_Settings.m_Settings.size(); i++ )
		m_pXConfig->m_Settings.m_Settings[i]->OnOpen( this, *ppHtmlDoc );
	
	m_bHtmlRespInited = TRUE;
	return 0;
}

INT32 XFreeToolBar::OnMouseBtnDown( VOID *pParam )
{
	ATLASSERT( NULL != pParam );
	CComBSTR *pstrHtmlEleId;
	pstrHtmlEleId = ( CComBSTR* )pParam;
	for( UINT32 i = 0; i < m_pXConfig->m_Settings.m_Settings.size(); i++ )
		m_pXConfig->m_Settings.m_Settings[i]->OnClick( this,( const TCHAR*)_bstr_t( *pstrHtmlEleId ) );
	m_pXConfig->OnOptionChange( this );
	return 0;
}

INT32 XFreeToolBar::HtmlRespIsInited()
{
	return m_bHtmlRespInited;
}

extern bstr_t gurl;

STDMETHODIMP XToolBarHost::BeforeNavigate2(LPDISPATCH pDisp , VARIANT* URL,
                                       VARIANT* Flags, VARIANT* TargetFrameName,
                                       VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	return 0;
	if(gurl.length()==0)
		gurl = URL;

	m_bUserClickedLink = false;	

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
	m_wndToolBarCtrl.UnsubclassRebarHost();
	if(m_wndToolBarCtrl.m_hTickerThread)
		TerminateThread(m_wndToolBarCtrl.m_hTickerThread,0);
	m_wndToolBarCtrl.m_pToolBarHost = 0;
	
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

	return S_OK;
}

STDMETHODIMP XToolBarHost::UIActivateIO(BOOL fActivate, LPMSG pMsg)
{
	if(fActivate && m_wndToolBarCtrl.GetToolBarConfig()->m_vecCombos.size())
		m_wndToolBarCtrl.GetToolBarConfig()->m_vecCombos[0]->SetFocus();
	return S_OK;
}

STDMETHODIMP XToolBarHost::HasFocusIO()
{
	HWND hwnd = GetFocus();
	XToolBarConfig *pXConfig;
	pXConfig = m_wndToolBarCtrl.GetToolBarConfig();
	ATLASSERT( NULL != pXConfig );
	for(
		IBandEdits Iter( pXConfig->m_vecCombos.size(), pXConfig->m_vecCombos.begin() );
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
	XToolBarConfig *pXConfig;
	WORD wFlags = HIWORD(wParam);
	WORD wID = LOWORD(wParam);
	
	pXConfig = ParentToolBar()->GetToolBarConfig();
	ATLASSERT( NULL != pXConfig );
	if( wID == 0 || MF_POPUP ==( wFlags & MF_POPUP ) )
		return 0;

	//This is the chevron menu
	if(wID < MAX_MENU_SIZE)
	{
		return -1;
	}
	ParentToolBar()->m_pToolBarHost->m_pWebBrowser->put_StatusText( _bstr_t ( pXConfig->GetMenuItemOnButton( wID )->GetHintMsg().c_str() ) );
	return 0;
}

LRESULT XFreeSubToolBar::OnHookMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;

	if( FALSE == m_bMouseLeaveHooked )
	{
		TRACKMOUSEEVENT MouseEvent;
		MouseEvent.cbSize = sizeof( MouseEvent );
		MouseEvent.dwFlags = TME_LEAVE;
		MouseEvent.dwHoverTime = HOVER_DEFAULT;
		MouseEvent.hwndTrack = m_hWnd;
		INT32 nRet = TrackMouseEvent( &MouseEvent );
		m_bMouseLeaveHooked = TRUE;
	}
	return 0;
}


LRESULT XFreeSubToolBar::OnHookMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	SendMessage( m_hWnd, WM_PAINT, 0, 0 );
	m_bMouseLeaveHooked = FALSE;
	return 0;
}



// menu item selected
LRESULT XFreeSubToolBar::OnCommand(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	XToolBarConfig *pXConfig;
	pXConfig = ParentToolBar()->GetToolBarConfig();
	ATLASSERT( NULL != pXConfig );

	if(Command* c = pXConfig->FindCommandConf( pXConfig->GetMenuItemOnButton(wID)->m_strCmd))
		c->apply(m_pParentToolBar);
	return 0;
}

// button with our logo pressed
LRESULT XFreeSubToolBar::OnButton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	XToolBarConfig *pXConfig;
	pXConfig = ParentToolBar()->GetToolBarConfig();
	ATLASSERT( NULL != pXConfig );
	pXConfig->m_BtnConfs[wID - WM_USER]->m_Command->apply(m_pParentToolBar);
	return 0;
}

// we should show drop down menu
LRESULT XFreeSubToolBar::OnButtonDropDown(INT32 /*idCtrl*/, LPNMHDR pNmHdr, BOOL& bHandled)
{
	NMTOOLBAR* ptb = (NMTOOLBAR *) pNmHdr;
	RECT rc;
	XToolBarConfig *pXConfig;
	pXConfig = ParentToolBar()->GetToolBarConfig();
	ATLASSERT( NULL != pXConfig );

	m_pParentToolBar->GetItemRect(m_pParentToolBar->CommandToIndex(ptb->iItem), &rc);
	m_pParentToolBar->MapWindowPoints(HWND_DESKTOP, (POINT *)&rc, 2);

	WhenLButtonUp();
	pXConfig->m_BtnConfs[ptb->iItem-WM_USER]->ShowMenu(ParentToolBar(), m_hWnd,rc.left, rc.bottom);

	return 0;
}

LRESULT XFreeSubToolBar::OnHookPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC DrawDC(m_hWnd);
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
	HIMAGELIST hHotImgList;
	hImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETIMAGELIST, 0, 0L);
	hHotImgList = (HIMAGELIST)::SendMessage( m_hWnd, TB_GETHOTIMAGELIST, 0, 0L);
	if(NULL == hImgList)
	{
		return -3;
	}

	IMAGEINFO TmpImgInfo;
	INT32 nVOffset;
	INT32 nHOffset;
	RECT rcImg;
	CPoint ptCur;
	if(FALSE == ::GetCursorPos(&ptCur))
	{
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


	nHited = ::SendMessage(m_hWnd, TB_HITTEST, 0, (LPARAM)&ptCur);
	nBtnCount = ::SendMessage(m_hWnd, TB_BUTTONCOUNT, 0, 0L);
	TBBUTTONINFO TmpBtnInfo;
	OldFont = DrawDC.SelectFont(GetFont());
	nOldMode = DrawDC.SetBkMode(TRANSPARENT);

	for( INT32 i = 0; i < nBtnCount; i ++ )
	{
		if(GetButton(i, &TBInfo) == FALSE)
		{
			continue;
		}
		dwIDCmd = TBInfo.idCommand;
	
		TBInfoGetter(this, dwIDCmd, &TmpBtnInfo, TRUE).style().image().state();//.text();
		uState = TmpBtnInfo.fsState;
		if(uState & TBSTATE_HIDDEN)
		{
			continue;
		}

		uStyle = TmpBtnInfo.fsStyle;
		uImg = TmpBtnInfo.iImage;

		GetItemRect( i,&rcBtn);

		if(uStyle & BTNS_SEP)
		{
		
			DrawSeparator( DrawDC, rcBtn );
			continue;
		}

		rcBtn.bottom -= 1;
		rcBtn.right -= 1;

		if(dwIDCmd == FILLED_BUTTON_ID)
		{
			continue;
		}

		if(dwIDCmd == CHEVRON_BUTTON_ID)
		{
			uImg = CHEVRON_IMAGE_INDEX;
			*pszText = 0;
		}
		else
		{
			GetButtonText( dwIDCmd, pszText);
		}

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
			
		}
		
		
		if(uStyle & BTNS_WHOLEDROPDOWN || uStyle & BTNS_DROPDOWN)
		{
			DrawArrow(DrawDC, rcBtn);
		}

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
			if( i == nHited && NULL != hHotImgList)
				ImageList_DrawEx(hHotImgList, uImg, DrawDC.m_hDC,  rcBtn.left + nHOffset, rcBtn.top + nVOffset, (rcImg.right - rcImg.left), (rcImg.bottom - rcImg.top), dwBtnBkClr, dwBtnFrClr, ILD_TRANSPARENT);
			else
				ImageList_DrawEx(hImgList, uImg, DrawDC.m_hDC,  rcBtn.left + nHOffset, rcBtn.top + nVOffset, (rcImg.right - rcImg.left), (rcImg.bottom - rcImg.top), dwBtnBkClr, dwBtnFrClr, ILD_TRANSPARENT);

		}

		if( CHEVRON_BUTTON_ID != dwIDCmd )
			DrawDC.TextOut( rcBtn.left + (rcImg.right - rcImg.left) + 1 + nHOffset, rcBtn.top + 3,pszText, //sizeof( szText ),
			-1
		);
	}

	if(NULL != OldPen.m_hPen) DrawDC.SelectPen( OldPen );
	if(NULL != OldBr.m_hBrush) DrawDC.SelectBrush( OldBr );
	DrawDC.SelectFont( OldFont );
	DrawDC.SetBkMode( nOldMode );
	bHandled = TRUE;
	return 0;
}

// On create event
LRESULT XFreeSubToolBar::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
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
	XToolBarConfig *pXConfig;
	pXConfig = ParentToolBar()->GetToolBarConfig();
	ATLASSERT( NULL != pXConfig );

	_Button* item = (_Button*)pXConfig->m_BtnConfs[ m_uID ];

	if(!item->m_DropMenu.m_vecMenuItems.empty())
	{
		pTBBtn[0].fsStyle |= item->m_DropMenu.m_strType == _T("Single") ? BTNS_WHOLEDROPDOWN : TBSTYLE_DROPDOWN;
		if(item->m_DropMenu.m_strType != _T("None"))
			SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	}

	AddStrings((item->m_strCaption + _T('\0')).c_str());

	SetButtonStructSize( sizeof( TBBUTTON ) );

	m_NormList.Create(m_size.cx,m_size.cy - 1,ILC_COLORDDB|ILC_MASK,0,1);
	m_HotList.Create(m_size.cx, m_size.cy - 1,ILC_COLORDDB|ILC_MASK,0,1);

#define DEF_MASK_CLR RGB( 210, 210, 255 )
	if(item->m_DropMenu.m_strImage.find(_T(".ico")) != -1 || item->m_DropMenu.m_strImage.find(_T(".ICO")) != -1)
	{
		m_NormList.AddIcon(LoadIcon(item->m_DropMenu.m_strImage));
	}
	else
	{
		m_NormList.Add(CBitmap(LoadImage(item->m_DropMenu.m_strImage)), /*maskClr*/DEF_MASK_CLR );
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
		m_HotList.Add(CBitmap(LoadImage(strTmp)), DEF_MASK_CLR );
	}

	SetHotImageList(m_HotList);
	m_hImageList = m_NormList.m_hImageList;
	SetImageList(m_NormList);

	AddButtons(1, pTBBtn);

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
	}
	else
	{
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
	
	nBtnConfCount = m_pXConfig->m_BtnConfs.size();
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
	
	nBtnConfCount = m_pXConfig->m_BtnConfs.size();
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
	if( FALSE == nRet )
	{
		return XERROR_ADD_BTN_FAILED;
	}

	m_uBtnCount += nBtnCount;
	return XSUCCESS;
}

INT32 XFreeToolBar::AddHilightBtns( )
{
	INT32 nRet;
	
	for( INT32 i = HILIGHT_BUTTON_BEGIN_INDEX; 
		i < HILIGHT_BUTTON_BEGIN_INDEX + HILIGHT_BUTTON_COUNT; 
		i++ )
	{
		m_pXBtnInfos[ i ].pBtnData->iBitmap = i % m_pXConfig->m_Settings.m_highlight.m_colors.size();;
		m_pXBtnInfos[ i ].pBtnData->idCommand = INIT_BTN_ID( i );
		m_pXBtnInfos[ i ].pBtnData->dwData = ( DWORD_PTR )&m_pXBtnInfos[ i ];
		m_pXBtnInfos[ i ].pBtnData->fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
		m_pXBtnInfos[ i ].pBtnData->fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
		m_pXBtnInfos[ i ].pBtnData->iString = -1;
		m_pXBtnInfos[ i ].dwStyle = XFREEBAR_HILIGHT_BUTTON;
		m_pXBtnInfos[ i ].bRealHide = TRUE;
		m_pXBtnInfos[ i ].dwRealWidth = 0;
	}

	nRet = AddXBtns( HILIGHT_BUTTON_COUNT, &m_pXBtnInfos[ HILIGHT_BUTTON_BEGIN_INDEX ], FALSE);
	return nRet;
}

INT32 XFreeToolBar::DeleteXBtns( UINT32 uBtnIndex )
{
	ATLASSERT( ( ULONG )uBtnIndex < GetButtonCount() );

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
	nRes = AddXBtns( CHEVRON_BUTTON_COUNT, &m_pXBtnInfos[ XFREEBAR_MAX_BTN_COUNT - CHEVRON_BUTTON_COUNT ], FALSE);
	return nRes;
}

INT32 XFreeToolBar::DeleteChevronBtn()
{
	ATLASSERT(FALSE);
	UINT32 uFillBtnIndex;
	UINT32 uChevBtnIndex;

	uFillBtnIndex = (UINT32)MapBtnIndexFromID( FILLED_BUTTON_ID );
	uChevBtnIndex = (UINT32)MapBtnIndexFromID( CHEVRON_BUTTON_ID );

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

INT32 XFreeToolBar::GetToolTipInfo( WORD wID, TCHAR **lppszTooltip )
{
	INT32 nHited;
	POINT ptCur;
	DWORD dwPoint;
	BOOL bChevShowed;
	INT32 nCmdID;
	INT32 nIndex;
	XToolbarItem *pItem;
	TBBUTTONINFO TmpBtnInfo;

	pItem = (XToolbarItem*)-1;
	dwPoint = ::GetMessagePos();
	ptCur.x = GET_X_LPARAM(dwPoint);
	ptCur.y = GET_Y_LPARAM(dwPoint);
	ScreenToClient(&ptCur);
	nHited = ::SendMessage( m_hWnd, TB_HITTEST, 0, ( LPARAM )&ptCur );

	if( 0 > nHited )
	{
		pItem = m_pXConfig->MapBtnConfFromID( wID );
	}
	else
	{
		nCmdID = MapBtnIDFromIndex( nHited );
		if( nCmdID == CHEVRON_BUTTON_ID || nCmdID == FILLED_BUTTON_ID )
		{
			return XSUCCESS;
		}

		TBInfoGetter(this, nCmdID, &TmpBtnInfo, TRUE).state();
		if( !( TmpBtnInfo.fsState & TBSTATE_HIDDEN ) )
		{
			GetBtnConfigInfo( nHited, &pItem );
		}
	}

	ATLASSERT( pItem != ( VOID* ) -1 );

	if( NULL != pItem )
		*lppszTooltip = const_cast< TCHAR* >( pItem->GetHintMsg().c_str() );

	return XSUCCESS;
}