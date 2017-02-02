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
#include "ConfigHost.h"
#include "XFreeToolBar.h"
#include "XToolBarHost.h"
#include "XHtmlEventObject.h"
#include <tom.h>

// some global variables :-(
XString gAppendCommand;
XString gAppendCommandDisplay;

extern _bstr_t gurl;

static std::map<XString,XString> gLastSearchWords;

static bool g_DO_HTML = false;

INT32 XFreeToolBar::OnOptionCheckBoxOpen( IHTMLDocument3* pHtmlDoc3 ,XString strSetting, XString strID )
{
	if( strSetting == XString( _T( "1" ) ))
	{
		IHTMLElementPtr element;
		pHtmlDoc3->getElementById( _bstr_t( strID.c_str() ), &element );
		if( element )
		{
			if( IHTMLInputElementPtr pelement = element )
				pelement->put_checked(VARIANT_TRUE);
		}
	}
	return 0;
}

static _variant_t varEmpty;
INT32 XFreeToolBar::OnConfigWebBrowserBeforeNavigate( XString strURL )
{
	if( FAILED( m_pToolBarHost->m_pWebBrowser->Navigate2( &_variant_t( _bstr_t( strURL.c_str() ) ), &varEmpty, &varEmpty, &varEmpty, &varEmpty ) ) )
	{
		return -1;
	}
	return 0;
}

INT32 XFreeToolBar::OnMenuSeparatorAppendMenu( CMenuHandle &HandleMenu )
{
#ifdef USE_SIMPLE_MENU
	HandleMenu.AppendMenu( MF_SEPARATOR );
#else
	_XFreeMenuItemData* pItem = new _XFreeMenuItemData();
	pItem->fType = MF_SEPARATOR;
	HandleMenu.AppendMenu( MF_OWNERDRAW, ( UINT_PTR )0, LPCTSTR( pItem ) );
#endif
	return 0;
}

INT32 XFreeToolBar::OnOptionCheckBoxClick( XString strCurID, XString strOrgID, XString strSetting )
{
	if( strCurID == strOrgID)
		setValue( strCurID, strSetting == XString( _T( "1" ) ) ? _T( "0" ) : _T( "1" ) );
	return 0;
}

INT32 XFreeToolBar::OnOptionRadioOpen( IHTMLDocument3* pHtmlDoc3 )
{
	return 0;
}

INT32 XFreeToolBar::OnOptionRadioClick( XString strID )
{
	return 0;
}

INT32 XFreeToolBar::OnOptionSelectOpen( IHTMLDocument3 *pHtmlDoc3, XString strID, XString strSetting)
{
	IHTMLElementPtr pelement = NULL;
	pHtmlDoc3->getElementById( _bstr_t( strID.c_str() ), &pelement );

	if( NULL == pelement )
	{
		return -1;
	}
	IHTMLDocument2Ptr pHtmlDoc2 = pHtmlDoc3;
	IHTMLSelectElementPtr pselelement = pelement;
	if( NULL == pselelement )
	{
		return -1;
	}

	// register handler of the onchange event with the CHostToolbar::OnSelChange method.
	pselelement->put_onchange( _variant_t( XHtmlEventObject< XFreeToolBar >::CreateHandler( this, &XFreeToolBar::OnSelChange, 1 ) ) );

	for( UINT32 i = 0; i < m_pXConfig->m_BtnConfs.size(); i++ )
	{
		XString strCmd = m_pXConfig->m_BtnConfs[ i ]->m_strCmd;

		Command* pCmd = m_pXConfig->FindCommandConf( strCmd );

		if( NULL == pCmd )
		{
			continue;
		}

		if( pCmd->canAddToSelect() )
		{
				IHTMLElementPtr element;
				pHtmlDoc2->createElement( _bstr_t( _T( "OPTION" ) ), &element );
				IHTMLOptionElementPtr pEleOption = element;
				pEleOption->put_text( _bstr_t( m_pXConfig->m_BtnConfs[ i ]->m_strCaption.c_str() ) );
				pselelement->add( element, _variant_t( long( -1 ) ) );
		}
	}

	XString strIndex = strSetting;
	LONG lIndex = strIndex[0] - '0';
	pselelement->put_selectedIndex( lIndex );
	return 0;
}

INT32 XFreeToolBar::OnOptionSelectSelChange( XString strID )
{
	return 0;
}

INT32 XFreeToolBar::OnOptionButtonClick( XString strID )
{
	return 0;
}

#define MESSAGE_COMMAND_CAPTION _T( "XFree Toolbar 1.0" )
#define MESSAGE_COMMAND_TEXT _T( "XFree Toolbar 1.0 designed by Arhtas. EMail:Shi.JiJie@hotmail.com" )
INT32 XFreeToolBar::OnCommandSpecialApply( XString strType, XString strName, XString strDisplay )
{
	if( strType == _T( "Hide toolbar" ) )
	{
		ShowBandObject( m_pToolBarHost->m_pWebBrowser, XTOOLBAR_UUID, FALSE );
	}
	else if( strType == _T( "Message" ) )
	{
		::MessageBox( 0, MESSAGE_COMMAND_TEXT, MESSAGE_COMMAND_CAPTION, MB_OKCANCEL );
	}
	else if( strType == _T( "Search" ) )
	{
		RunSearch();
	}
	else if( strType == _T( "ClearHistory" ) )
	{
		CRegKey keyAppID;
		keyAppID.Attach( GetAppRegistryKey() );
		keyAppID.RecurseDeleteKey( _T("History1") );
		keyAppID.RecurseDeleteKey( _T("History2") );
		for( IBandEdits Iter( m_pXConfig->m_vecCombos.size(), m_pXConfig->m_vecCombos.begin() ); Iter; Iter++)
			Iter->ClearHistory();
	}
	else if( strType == _T( "Uninstall" ) )
	{
		if( ::MessageBox( 0, XTOOLBAR_UNINSTALL_MSG, XTOOLBAR_ALERT_TITLE, MB_YESNO ) == IDYES)
		{
			m_pToolBarHost->m_pWebBrowser->Quit();
			DllUnregisterServer();
		}
	}
	else if( strType == _T( "Update" ) )
	{
		setValue( _T( "gUpdate" ), _T( "1" ) );
		BuildToolBar();
		IWebBrowser2 *pBrowser = m_pToolBarHost->m_pWebBrowser;

		_variant_t varURL = URL_AFTER_UPDATE;
#ifdef _CHECK_UPDATE
		XString strVerFile = _T( "version.txt" );
		XString strPrevCRCCode = getValue( strVerFile );
		if( CheckUpdate ( strVerFile, true ) )
		{
			XString strCRCCode = getValue( strVerFile );
			if( strPrevCRCCode == newstrCRCCodeCRC )
				varURL  = urlAfterUpdate2;
		}
#endif
		pBrowser->Navigate2(&varURL, &varEmpty, &varEmpty, &varEmpty, &varEmpty);
	}
	else if( strType == _T( "ShowDialog") )
	{
#ifdef _ODBC_DIRECT
#include "Toolband7\ShowDialog.cpp"
#endif
#ifdef _SITE_SAFE
#include "Toolband3\ShowDialog.cpp"
#endif
	}
	else if( strType == _T( "Make Default Toolbar" ) )
	{
		setValue( _T( "Toolbar Extension" ), gAppendCommand );
		setValue( _T( "Toolbar Extension Display" ), gAppendCommandDisplay );
	}
#ifdef _MICROSOLUTIONS
#include "Toolband22\popup.cpp"
#endif
	else if( strType == _T( "Switch to" ) )
	{
		gAppendCommand = strName;
		for( IBandEdits Iter( m_pXConfig->m_vecCombos.size(), m_pXConfig->m_vecCombos.begin() ); Iter; Iter++)
		{
			gLastSearchWords[ Iter->GetBtnConfCaption() ] = Iter->GetText();
		}
#ifdef _GERMAN
#include "Toolband21\SwitchTo.cpp"
#endif
		m_pXConfig->OnOptionChange( this );
		if(XToolbarItem* pItem = m_pXConfig->MapBtnConfFromCmd( "search" ) )
		{
			pItem->m_strCaption = strDisplay;
			gAppendCommandDisplay = strDisplay;
		}
		BuildToolBar();
	}
	else if(strType==_T("Option"))
	{
		XToolbarItem *pItem;
		
		OnConfig();
		pItem = m_pXConfig->MapBtnConfFromCmd( "option" );
		ATLASSERT( NULL != pItem );
		InvalidateButtonByID( pItem->GetBtnCmdId() );
	}
	else if( strType == _T( "About" ) )
	{
		OnAbout();
	}
#ifdef _ODBC_DIRECT
#include "Toolband7\startAutoLogin.cpp"
#endif
	return 0;
}

INT32 XFreeToolBar::InvalidateButtonByIndex( INT32 nIndex )
{
	ATLASSERT( 0 <= nIndex );
	RECT rcBtn;
	BOOL bRet;
	GetItemRect( nIndex, &rcBtn );
	bRet = ::InvalidateRect( m_hWnd, &rcBtn, TRUE );
	return bRet;
}

INT32 XFreeToolBar::InvalidateButtonByID( INT32 nID )
{
	ATLASSERT( 0 <= nID );
	INT32 nBtnIndex;
	nBtnIndex = MapBtnIndexFromID( nID );
	if( nBtnIndex < 0 )
	{
		return XERROR_BTN_NOT_EXIST;
	}

	return InvalidateButtonByIndex( nBtnIndex );
}

INT32 XFreeToolBar::OnCommandScriptApply( LPCTSTR lpszBody )
{
	IHTMLDocument2Ptr pHtmlDoc = GetHtmlDocument();
	if(pHtmlDoc)
	{
		CComPtr< IDispatch > pDisp;
		pHtmlDoc->get_Script( &pDisp );
		if( (VOID*)NULL == pDisp )
		{
			return -1;
		}
		CComQIPtr< IHTMLWindow2 > pHTMLWnd = pDisp; //This IHTMLWindow2 interface has the execScript method to execture the script
		VARIANT pvarRet;
		pHTMLWnd->execScript( _bstr_t( lpszBody ), _bstr_t( "JavaScript" ), &pvarRet );
	}
	return 0;
}

INT32 XFreeToolBar::OnCommandPopClockApply( XString strBlockType )
{
	if( strBlockType == "IsBlock" )
	{
		setValue( "IsBlockWindow", "1" );
		setValue( "IsBlockSound", "1" );
		CheckButton( WM_USER + m_pXConfig->MapBtnConfIndexFromCmd( _T( "ToggleBlock" ) ) ,TRUE );
	}
	else if( strBlockType == "IsSound" )
	{
		setValue( "IsBlockSound", isGetValue2( "IsBlockSound" ) ? "0" : "1" );
	}
	else if( strBlockType == "IsClean" )
	{
		m_pToolBarHost->CleanBlockCount();
	}
	return 0;
}

INT32 XFreeToolBar::OnCommandPopClockPressed()
{
	setValue( "IsBlockWindow", "0" );
	CheckButton( WM_USER + m_pXConfig->MapBtnConfIndexFromCmd( _T( "ToggleBlock" ) ), FALSE );
	return 0;
}

INT32 XFreeToolBar::OnSplitterGetRegion( INT32 nID, LPRECT pRect )
{
	return GetItemRect( nID, pRect );
}

INT32 XFreeToolBar::OnCommandWebJumpApply( XString strURL, XString strPost, XString strNewWindow, XString strOption, XString strTarget )
{		
	IHTMLDocument2Ptr pHtmlDoc = GetHtmlDocument();

	for( IBandEdits Iter(  m_pXConfig->m_vecCombos.size(), m_pXConfig->m_vecCombos.begin() ); Iter; Iter++ )
	{
		gLastSearchWords[Iter->GetBtnConfCaption()] = Iter->GetText(); // GetText knows about CBS_DROPDOWNLIST style
		XString strEdit = replaceSpace( ( urlencode( XString( Iter->GetText() ) ) ), vector<XString>() );
#ifdef _BIBLE
		if(Iter->m_pComboHost->m_strName==XString("book"))
			strEdit = IntToStr(Iter->FindStringExact(strEdit.c_str())+1);
#endif
		XString mask = _T("%")+Iter->GetBtnConfCaption();

		if((replace(strURL,mask,strEdit) || replace(strPost,mask,strEdit)) && Iter->m_pComboHost->m_style!=CBS_DROPDOWNLIST)
			Iter->AddinHistoryAndUpdateWords(Iter->GetText());
	}

	CComBSTR  url;
	CComBSTR  domain;
	CComBSTR  title;
	if(pHtmlDoc)
	{
		if(pHtmlDoc->get_URL(&url)==0)
			gurl = urlencode(XString(_bstr_t(url))).c_str();
		pHtmlDoc->get_domain(&domain);
		pHtmlDoc->get_title(&title);
	}

	replace(strURL,_T("%d"),_bstr_t(domain));
	replace(strURL,_T("%title"),_bstr_t(title));

	_variant_t varURL = replaceUrl(strURL);
	_variant_t postData = getPostData((const TCHAR*)replaceUrl(strPost));
	_variant_t varHeaders;
	varHeaders.vt = VT_BSTR;
	varHeaders.bstrVal = NULL;

#ifdef _DYNABAR
	varHeaders = "Referer: DynaBar toolbar";
#endif

	if( strNewWindow == _T("true") ||
		( strNewWindow.empty() && isGetValue2( _T( "OpenNew" ) ) ) )
	{
		IWebBrowser2 *pIE = CreateNewIEInstance();
		pIE->Navigate2(&varURL, &varEmpty, &varEmpty, &postData, &varHeaders);
	}
	else
	{
		if( strNewWindow == _T("modaldialog"))
		{
			HRESULT Ret;
			IHTMLDocument2Ptr pHtmlDoc = GetHtmlDocument();
			if(pHtmlDoc)
			{
				IHTMLWindow2Ptr win = 0;
				pHtmlDoc->get_parentWindow(&win);
				if( win )
					Ret = win->showModalDialog( varURL.bstrVal, &_variant_t( "" ), &_variant_t( strOption.c_str() ), &varEmpty );
			}
		}
		// show HTMl page that can have a lot of "our" page links
		else if( strNewWindow == _T( "toolbarwindow" ) )
		{
#if defined(_ALEXA) || defined (_TYCOON) || defined (_ESTELLEREYNA)
			CHtmlPage* HtmlPage = new CHtmlPage;
			HtmlPage->m_parentBrowser = GetWebBrowser();
			HtmlPage->m_url = varURL;
			HtmlPage->Create(*pToolbar);
			HtmlPage->ShowWindow(SW_SHOW);
			HtmlPage->m_wasShow = true;

			CRect rc,rc2;
			GetItemRect(m_nLastPresedButton, rc);
			ClientToScreen(rc);
			HtmlPage->GetWindowRect(&rc2);
			HtmlPage->MoveWindow(rc.left,rc.bottom,rc2.Width(),rc2.Height());
#endif
		}
		else
		{
			_variant_t target = strTarget.c_str();
			m_pToolBarHost->m_pWebBrowser->Navigate2( &varURL, &varEmpty, &target, &postData, &varHeaders );
		}
	}
	return 0;
}

INT32 XFreeToolBar::OnWebJumpLocalHTMLApply()
{
	return 0;
}


INT32 XFreeToolBar::OnCommandShellApply( INT32 nType, XString strCmd, XString strCantFound)
{
	XString strCmdTmp = strCmd;
	if( nType == CMD_SUPER )
	{
		for( IBandEdits Iter( m_pXConfig->m_vecCombos.size(), m_pXConfig->m_vecCombos.begin() ); Iter; Iter++ )
		{
			gLastSearchWords[ Iter->GetBtnConfCaption() ] = Iter->GetText(); // GetText knows about CBS_DROPDOWNLIST style
			XString strEdit = replaceSpace( XString( Iter->GetText() ), vector< XString >() );
#ifdef _BIBLE
			if( Iter->m_pComboHost->m_strName == XString( "book" ) )
				strEdit = IntToStr( Iter->FindStringExact( strEdit.c_str() ) +1 );
#endif
			XString mask = _T( "%" ) + Iter->GetBtnConfCaption();

			if( replace( strCmd, mask, strEdit ) && Iter->m_pComboHost->m_style != CBS_DROPDOWNLIST )
				Iter->AddinHistoryAndUpdateWords( Iter->GetText() );
		}
	}

	HINSTANCE ret = ::ShellExecute( NULL, NULL, strCmd.c_str(), NULL, NULL, SW_SHOWNORMAL );
	if( INT32( ret ) < 32 )
		::MessageBox( 0, strCantFound.c_str(), XTOOLBAR_ALERT_TITLE, 0 );
	return 0;
}

INT32 XFreeToolBar::OnCommandHilightApply()
{
	ATLASSERT( NULL != m_pHtmlFunc && NULL != m_pHtmlSearch );
	INT32 nColors;
	if( ( nColors = m_pXConfig->m_Settings.m_highlight.m_colors.size() ) <= 0 )
		return -1;

	OnCommandHilightPressed();

	XString strAllKeyWords;
	for( IBandEdits Iter( m_pXConfig->m_vecCombos.size(), m_pXConfig->m_vecCombos.begin() ); Iter; Iter++ )
		strAllKeyWords += Iter->GetText() + _T(" ");

	// Get the text in the edit box
	std::vector< XString > vecKeyWords;
	replaceSpace( strAllKeyWords, vecKeyWords );

	for( ULONG i = 0; i < vecKeyWords.size(); i++ )
	{
		XString strVal = vecKeyWords[ i ];
		replace( strVal, _T("\""), XString( _T( "" ) ) );

		m_pHtmlSearch->SearchAndTipTextInHTML( strVal.c_str(), 0/*tomMatchWord*/, GetCommandHighlightColor( i % nColors ).c_str() );
	}

	m_pHtmlFunc->MoveFocusToHTML();
	CheckButton( WM_USER + m_pXConfig->MapBtnConfIndexFromCmd( _T( "Highlight" ) ), TRUE );
	return 0;
}

INT32 XFreeToolBar::OnCommandHilightPressed()
{
	for( UINT32 i = 0 ; i < m_pXConfig->m_Settings.m_highlight.m_colors.size(); i ++ )
	{
		m_pHtmlSearch->ClearSearchResults();
	}

	CheckButton( WM_USER + m_pXConfig->MapBtnConfIndexFromCmd( _T( "Highlight" ) ), FALSE);
	return 0;
}

INT32 XFreeToolBar::OnToolbarItemPrePaint( LPNMTBCUSTOMDRAW lpTBCustomDraw )
{
	bool hot = ( lpTBCustomDraw->nmcd.uItemState & CDIS_HOT ) == CDIS_HOT;
	bool selected = ( lpTBCustomDraw->nmcd.uItemState & CDIS_SELECTED ) == CDIS_SELECTED;
	if( hot || selected )
	{
		lpTBCustomDraw->clrText = 200 << 16;
		lpTBCustomDraw->clrBtnHighlight = 234;

		CDCHandle dc = lpTBCustomDraw->nmcd.hdc;
		dc.SetTextColor( lpTBCustomDraw->clrText );
		dc.SetBkColor( 255 );
		dc.SetBkMode( lpTBCustomDraw->nStringBkMode );

		HFONT hFont = GetFont();
		HFONT hFontOld = NULL;
		if(hFont != NULL)
			hFontOld = dc.SelectFont( hFont );
	}
	return CDRF_DODEFAULT;
}

INT32 XFreeToolBar::OnToolbarItemChange( INT32 nIndex )
{
	XToolbarItem *pBtnConf;
	INT32 nBtnIndex;

	XString strName = m_pXConfig->m_BtnConfs[ nIndex ]->GetBtnConfCaption();
	strName += _T('\0');

	AddStrings( strName.c_str() );
	ATLTRACE( "Add strings %s\n", strName.c_str() );
	return 0;
}

#define SEPARATOR_WIDTH 6 
INT32 XFreeToolBar::OnItemSeparatorChange( INT32 nIndex )
{
	TBInfoSetter( this, nIndex ).style( TBSTYLE_SEP ).size( SEPARATOR_WIDTH ); //.image(8); 
	return 0;
}

INT32 XFreeToolBar::OnItemSeparatorOptionChange( INT32 nIndex, XString strVisId )
{
	HideBtnIfNeeded( nIndex, strVisId );// to hide last by default
	return 0;
}

INT32 XFreeToolBar::OnDragHandleDragStop( VOID *pParam )
{
	ATLASSERT( pParam != NULL );

	Splitter *pSplitter;
	IXEditCtrl *pEdit;

	pSplitter = ( Splitter* )pParam;
	pEdit = (IXEditCtrl*)pSplitter->GetDraggingEdit();

	POINT curPoint;
	::GetCursorPos( &curPoint );

	ScreenToClient( &curPoint );
	CRect rcEdit;
	GetItemRect( MapBtnIndexFromID( pEdit->m_pComboHost->GetBtnCmdId() ), &rcEdit );

	g_XLog.Print( 0, "The combobox item rect geted:(left %d right %d top %d bottom %d\n", 
		rcEdit.left, 
		rcEdit.right, 
		rcEdit.top, 
		rcEdit.bottom );

	CRect rcBand;
	GetWindowRect( &rcBand );
	ScreenToClient( &rcBand );
	rcEdit.right = curPoint.x;
	rcEdit.bottom = rcEdit.top + COMBOBOX_HEIGHT;

	if( rcEdit.right < rcBand.right - BLANK_WIDTH && rcEdit.Width() > MIN_COMBO_WIDTH  && rcEdit.Width() < 600 )
	{
		INT32 nBtnIndex;
		pEdit->MoveWindow(rcEdit);

		nBtnIndex = MapBtnIndexFromID( pEdit->m_pComboHost->GetBtnCmdId() );
		TBInfoSetter( this, pEdit->m_pComboHost->GetBtnCmdId(), TRUE ).size( rcEdit.Width() ).text( _T( "" ) );

		CalcXFreeBarWidth();
		SetBtnsWidth( nBtnIndex, nBtnIndex + 1 );
		DepartToolBar();
	}

	return 0;
}

INT32 XFreeToolBar::OnMenuCommandAppendMenu( CMenuHandle &HanldeMenu, XString &strCaption, INT32 nImgIndex, XString strCheckKey, XString strCmd, INT32& nMenuIndex, INT32 nMenuItemIndex )
{
	INT32 nRet;
#ifdef USE_SIMPLE_MENU
	handle.AppendMenu(MF_STRING,MENU_COMMAND_ID_BEGIN + MAX_MENU_SIZE*nMenuIndex +nMenuItemIndex,m_strCaption.c_str());
#else
	_XFreeMenuItemData* pMI = new _XFreeMenuItemData();
	pMI->fType = MF_STRING;
	pMI->lpstrText = (LPTSTR)strCaption.c_str();
	pMI->iButton = GetToolBarItemBmp( nImgIndex ); 
	if( pMI->iButton >= 0 )
	{
		pMI->iButton += TOOLBAR_IMAGE_INDEX_BEGIN;
	}

	#define MENU_ITEM_CMD_ID( btn_id, menu_item_id ) ( WORD )( MAX_MENU_SIZE * btn_id + menu_item_id )
	nRet = HanldeMenu.AppendMenu( MF_OWNERDRAW, MENU_COMMAND_ID_BEGIN + MENU_ITEM_CMD_ID( nMenuIndex, nMenuItemIndex ), LPCTSTR( pMI ) );
#endif
	if( strCheckKey != "" )
	{
		HanldeMenu.CheckMenuItem( nMenuItemIndex, MF_BYPOSITION|( isGetValue2( strCheckKey ) ? MF_CHECKED :MF_UNCHECKED ) );
	}

	if( strCmd == gAppendCommand )
		HanldeMenu.CheckMenuRadioItem( MENU_COMMAND_ID_BEGIN + MAX_MENU_SIZE * nMenuIndex +0,MENU_COMMAND_ID_BEGIN + MAX_MENU_SIZE * nMenuIndex +nMenuItemIndex ,MENU_COMMAND_ID_BEGIN + MAX_MENU_SIZE * nMenuIndex + nMenuItemIndex,MF_BYCOMMAND);
	return 0;
}

XString XFreeToolBar::GetCommandHighlightColor( INT32 nIndex )
{
	return ( _T( "color:#000000;background-color:" ) + m_pXConfig->m_Settings.m_highlight.m_colors[ nIndex ] + _T(";") ).c_str();
}

Combo::~Combo()
{
	if( m_pEdit )
		m_pEdit->DestroyWindow();

	delete m_pEdit;
}

INT32 XFreeToolBar::OnItemButtonOptionChange( VOID *pParam )
{
	ATLASSERT( NULL != pParam );
	_Button *pButton = (_Button*)pParam;

	INT32 nBtnIndex;
	nBtnIndex = MapBtnIndexFromID( pButton->GetBtnCmdId() );

	AddCustomImage( pButton->GetConfImageIndex(), pButton->m_strHotImage );
	if( pButton->GetConfImageIndex().empty() && ! pButton->m_DropMenu.m_strImage.empty() )
	{
		RECT rcBtn;
		GetItemRect( nBtnIndex, &rcBtn );
		pButton->m_DropMenu.m_SubToolBar.MoveWindow( &rcBtn );
	}

	if( pButton->m_strCheckKey != "" )
	{
		CheckButton( pButton->GetBtnCmdId(), isGetValue2( pButton->m_strCheckKey ) );
	}

	TBBUTTONINFO TButtonInfo;
	TButtonInfo.cbSize = sizeof TBBUTTONINFO;
	TButtonInfo.dwMask = TBIF_TEXT | TBIF_IMAGE;

#ifdef _HUNTBAR
#include "Toolband12\DescriptiveText.cpp"
#else
	if( !isGetValue( _T("DescriptiveText") ) )
		TButtonInfo.pszText = _T( "" );
	else
		TButtonInfo.pszText = const_cast< TCHAR* >( pButton->GetBtnConfCaption().c_str() );
	if( !isGetValue( _T( "DescriptiveImage" ) ) )
		TButtonInfo.iImage = -2;
	else
		TButtonInfo.iImage = GetImageId( pButton->GetConfImageIndex());
	SetButtonInfo( pButton->GetBtnCmdId(), &TButtonInfo );
	HideBtnIfNeeded( nBtnIndex, pButton->m_strVisId );
#endif
	return 0;
}

INT32 XFreeToolBar::OnItemButtonChange( VOID *pParam )
{
	ATLASSERT( NULL != pParam );

	_Button *pButton;
	pButton = ( _Button* ) pParam;

	RECT rc;
	INT32 nBtnIndex;
	XString strMenuType;
	nBtnIndex = MapBtnIndexFromID( pButton->GetBtnCmdId() );
	ATLASSERT( nBtnIndex >= 0 );

	strMenuType = pButton->m_DropMenu.m_strType;
	GetItemRect( nBtnIndex, &rc );

	if( !strMenuType.empty() && strMenuType != _T( "None" ) )
	{
		TBInfoSetter( this, pButton->GetBtnCmdId(), TRUE).style( BTNS_SHOWTEXT | TBSTYLE_AUTOSIZE | ( pButton->m_DropMenu.m_strType == _T( "Single" ) ? BTNS_WHOLEDROPDOWN : TBSTYLE_DROPDOWN ) ); 
	}

	if( pButton->GetConfImageIndex().empty() && ! pButton->m_DropMenu.m_strImage.empty()) 
	{
		if(pButton->m_DropMenu.m_strImage.find(_T(".ico")) != -1 || pButton->m_DropMenu.m_strImage.find(_T(".ICO")) != -1)
		{
			pButton->m_DropMenu.m_SubToolBar.m_size.cx = 16;//GetIconSize(LoadIcon(m_DropMenu.m_strImage));
			pButton->m_DropMenu.m_SubToolBar.m_size.cy = TB_HEIGHT;
		}
		else
		{
			CBitmap( LoadImage( pButton->m_DropMenu.m_strImage ) ).GetSize( pButton->m_DropMenu.m_SubToolBar.m_size );
		}

#ifdef _DYNABAR
		m_DropMenu.m_SubToolBar.m_size.cx /=2;
#endif
		CDC dc = ::GetDC(NULL);
		dc.SelectFont( m_Font.m_hFont );
		SIZE size;
		GetTextExtentPoint(dc, pButton->m_strCaption.c_str(), pButton->m_strCaption.size(),&size);

		pButton->m_DropMenu.m_SubToolBar.m_uID = pButton->GetBtnConfIndex();
		pButton->m_DropMenu.m_SubToolBar.setroot( this );

		rc.bottom = TB_HEIGHT;

		HWND hWnd;
		ATLTRACE( "Button %d is creating\n", nBtnIndex );
		hWnd = pButton->m_DropMenu.m_SubToolBar.Create(*this, rc, NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
			CCS_TOP | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN |
			TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_LIST ,0); 

		{
			TBInfoSetter( this, pButton->GetBtnCmdId(), TRUE ).style( TBSTYLE_BUTTON ).size( size.cx + pButton->m_DropMenu.m_SubToolBar.m_size.cx + ADD_TO_DROP_MENU ); 
		}

		ATLTRACE( "Button %d is created, created window is %u\n", nBtnIndex, (UINT32)hWnd );
	}
	return 0;
}


INT32 XFreeToolBar::OnItemPageOptionChange( VOID *pParam )
{
	ATLASSERT( NULL != pParam );
	Page *pPage = ( Page* )pParam;

	static bool old = g_DO_HTML;
	if( ( pPage->m_Page.m_browser == NULL ) && g_DO_HTML)
	{
		AtlAxCreateControlEx( replaceUrl( pPage->m_strUrl ), 
			pPage->m_Page.m_hWnd, 
			NULL, 
			( IUnknown** )&(pPage->m_Page.m_browser), 
			NULL, 
			DIID_DWebBrowserEvents2, 
			(IUnknown*)&pPage->m_Page 
			);

		::SendMessage( pPage->m_Page.m_hWnd, WM_SIZE, 0, pPage->m_dwWidth );
	}

	pPage->m_Page.ShowWindow(SW_SHOW);// : SW_HIDE);
	HideBtnIfNeeded( pPage->GetBtnCmdId(), pPage->m_strVisId, FALSE );

	CRect rc;
	GetItemRect( MapBtnIndexFromID( pPage->GetBtnCmdId() ), &rc );

	TBInfoSetter( this, pPage->GetBtnCmdId(), TRUE ).style( TBSTYLE_BUTTON ).state( TBSTATE_INDETERMINATE ).size( pPage->m_dwWidth ); 

	rc.right = rc.left + pPage->m_dwWidth;
	rc.top += 2;
	rc.bottom -= 2;
	rc.bottom += TB_HEIGHT + 8; 
	pPage->m_Page.MoveWindow( rc );
	return 0;
}

INT32 XFreeToolBar::ClearTempMenuItems()
{
	INT32 nMenuSize;
	nMenuSize = _XFreeMenuItemData::MenuItems.size();
	ATLTRACE("DropDown Menu Size Is %d\r\n", nMenuSize );
	_XFreeMenuItemData::MenuItems.clear();
	return 0;
}

INT32 XFreeToolBar::OnItemPageChange( VOID *pParam )
{
	ATLASSERT( NULL != pParam );
	Page *pPage = ( Page* )pParam;

	if( !pPage->m_Page.m_hWnd )
	{
		CRect rc;
		GetItemRect( MapBtnIndexFromID( pPage->GetBtnCmdId() ), &rc );
		
		pPage->m_Page.pConfigHost = this;
		
		pPage->m_Page.Create( *this, rc, NULL, WS_CHILD | WS_TABSTOP | WS_VISIBLE );

		if( g_DO_HTML )
			AtlAxCreateControlEx( replaceUrl( pPage->m_strUrl ), pPage->m_Page.m_hWnd, NULL, (IUnknown**)&(pPage->m_Page.m_browser), NULL, DIID_DWebBrowserEvents2, (IUnknown*)&(pPage->m_Page) );
	}
	return 0;
}

#include "RegOper.h"
INT32 XFreeToolBar::OnItemComboChange( VOID *pParam )
{
	ATLASSERT( NULL != pParam );
	Combo *pCombo = (Combo*)pParam;

	{ 
		TBInfoSetter BtnInfoSetting( this, pCombo->GetBtnCmdId(), TRUE );
		if( pCombo->m_widths.size() > 0 && pCombo->m_widths[0] >= 0 && pCombo->m_widths[0] <= DEFAULT_COMBOBOX_WIDTH )
			BtnInfoSetting.size( pCombo->m_widths[0] );
		BtnInfoSetting.style( TBSTYLE_BUTTON );
	} 

	if( NULL == pCombo->m_pEdit )
	{
		pCombo->m_pEdit = new XComboBoxCtrl;
		pCombo->m_pEdit->m_pConfigHost = this;
		pCombo->m_pEdit->m_pComboHost = pCombo;
		pCombo->m_pEdit->Create( m_hWnd, GetToolTips() );
	}

	pCombo->m_pEdit->ResetContent(); //Clean the text in the edit by send a TB_RESET_CONTENT message

	m_pXConfig->m_vecCombos.push_back( pCombo->m_pEdit ); //Toolbar has the EditCtrl list too.std::Vector<XComboBoxCtrl>

	if( pCombo->m_values.empty() )
	{
		for( RegIterator Iter( pCombo->m_strName ); Iter; Iter++ )
			pCombo->m_pEdit->InsertString( -1, Iter );

		pCombo->m_pEdit->SetWindowText( gLastSearchWords[ pCombo->m_strName ].c_str() );
	}
	else
	{
		for( UINT32 i = 0; i < pCombo->m_values.size(); i++)
		{
			pCombo->m_pEdit->InsertString( i, pCombo->m_values[ i ]->m_strDisplay.empty() ? pCombo->m_values[ i ]->m_value.c_str() : pCombo->m_values[ i ]->m_strDisplay.c_str() );
			pCombo->m_pEdit->SetItemDataPtr( i, &pCombo->m_values[ i ]->m_value);
		}
#ifdef _PROXY_CHANGE_TOOLBAR
#include "Toolband26\initProxyList.inl"
#endif
		INT32 idx = XString( getValue( _T("SearchUsing"), _T("0") ) )[0] - '0';
		pCombo->m_pEdit->SetCurSel( idx );
	}

	UpdateSearchedWords();
	AtlInitCommonControls( ICC_WIN95_CLASSES );

	EnableButton( pCombo->GetBtnCmdId(), FALSE );
	return 0;
}

INT32 XFreeToolBar::OnItemComboOptinoChange( VOID *pParam )
{
	ATLASSERT( NULL != pParam );
	Combo *pCombo = (Combo*)pParam;

	if( !pCombo->m_strVisId.empty() )
	{
		HideButton( pCombo->GetBtnCmdId(), !isGetValue( pCombo->m_strVisId ) );
		pCombo->m_pEdit->ShowWindow( isGetValue( pCombo->m_strVisId ) );
	}

	RECT rcEdit;
	GetItemRect( MapBtnIndexFromID( pCombo->GetBtnCmdId() ), &rcEdit );

	rcEdit.bottom = rcEdit.top + COMBOBOX_HEIGHT;
	XString p = getValue( _T("EditWidth") + pCombo->m_strName);
	UINT32 ip = p[0] - '0';
	if(ip >= pCombo->m_widths.size() )
		ip = 0;

	INT32 new_width = ( pCombo->m_widths[ ip ] ) / (100);
	rcEdit.right = rcEdit.left + new_width;

	pCombo->m_pEdit->MoveWindow( &rcEdit );
	TBInfoSetter( this, pCombo->GetBtnCmdId(), TRUE ).size( new_width ).text( _T("") ); 

	pCombo->m_pEdit->SetFont( m_Font );
	return 0;
}

INT32 XFreeToolBar::OnDragHandleOptionChange( VOID *pParam )
{
	Splitter *pSplitter;
	pSplitter = (Splitter*)pParam;

	IXEditCtrl* pEdit;
	for( IBandEdits Iter( m_pXConfig->m_vecCombos.size(), m_pXConfig->m_vecCombos.begin() ); Iter; Iter++ )
	{
		if( pSplitter->strEditName == Iter->GetBtnConfCaption() )
		{
			pSplitter->SetDraggingEdit( (IXEditCtrl* )Iter );
			break;
		}
		
	}
	
	pEdit = pSplitter->GetDraggingEdit();

	if( pSplitter->m_bFirstChange )
	{
		CRect rcEdit;
		GetItemRect( MapBtnIndexFromID( pEdit->m_pComboHost->GetBtnCmdId() ), &rcEdit );
		rcEdit.bottom = rcEdit.top + COMBOBOX_HEIGHT;

		if(rcEdit.right - rcEdit.left > MAX_COMBOBOX_WIDTH)
			return 0;

		rcEdit.right = rcEdit.left + DEFAULT_COMBOBOX_WIDTH;

		pEdit->MoveWindow(rcEdit);
		TBInfoSetter( this,pEdit->m_pComboHost->GetBtnCmdId(), TRUE ).size(rcEdit.Width()/* - 80*/ ).text(_T("")); 
		CalcXFreeBarWidth();
		pSplitter->m_bFirstChange = false;
		Invalidate(0);
	}

	m_pXConfig->m_splitters.push_back( pSplitter );
	TBInfoSetter( this, pSplitter->GetBtnCmdId(), TRUE ).size(SPLITTER_WIDTH).text(_T(""));

	return 0;
}

INT32 XFreeToolBar::OnPopupMenuAppenMenu( CMenuHandle &HandleMenu, LPCTSTR lpszTitle, CMenuHandle &MenuPopup)
{
	_XFreeMenuItemData* pMI = new _XFreeMenuItemData();
	pMI->fType = MF_STRING;
	pMI->lpstrText = ( LPTSTR ) lpszTitle;
	pMI->iButton = -1;
	HandleMenu.AppendMenu( MF_OWNERDRAW | MF_POPUP, ( UINT32 )( HMENU )MenuPopup, LPCTSTR( pMI ) );
	return 0;
}

INT32 XFreeToolBar::OnComboProcess()
{
	RunSearch();
	return 0;
}

INT32 XFreeToolBar::OnComboCommand()
{
	return 0;
}

INT32 XFreeToolBar::OnComboSetFocus()
{
	if( m_pToolBarHost)
		m_pToolBarHost->FocusChange( TRUE );

	return XSUCCESS;
}

INT32 XFreeToolBar::OnComboKillFocus()
{

	if( m_pToolBarHost)
		m_pToolBarHost->FocusChange( FALSE );

	return XSUCCESS;
}