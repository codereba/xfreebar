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
#include "XHtmlOption.h"
#include "XHtmlEventObject.h"

XHtmlOption::XHtmlOption(void) : m_pHtmlResponsor( NULL )
{
}

XHtmlOption::~XHtmlOption(void)
{
}

void XHtmlOption::OnMouseBtnDown( DISPID DispId, VARIANT* pVarResult )
{
	if( FALSE != m_pHtmlResponsor->HtmlRespIsInited() )
		return;

	IHTMLDocument2Ptr pHtmlDoc = GetHtmlDocument();

	if( pHtmlDoc )
	{
		IHTMLWindow2Ptr pHtmlWin;
		pHtmlDoc->get_parentWindow( &pHtmlWin );
		if( pHtmlWin == NULL )
			return;

		IHTMLEventObjPtr pHtmlEvent;
		pHtmlWin->get_event( &pHtmlEvent );
		if( pHtmlEvent == NULL)
			return;

		IHTMLElementPtr pHtmlEle;
		pHtmlEvent->get_srcElement( &pHtmlEle );
		if( pHtmlEle == NULL )
			return;
		CComBSTR  strHtmlEleId;
		pHtmlEle->get_id( &strHtmlEleId );
		if( strHtmlEleId )
		{
			m_pHtmlResponsor->OnMouseBtnDown( ( VOID* )&strHtmlEleId );
		}
	}
}

STDMETHODIMP XHtmlOption::OnDownloadComplete()
{
	ATLASSERT( NULL != m_pHtmlResponsor );
	IHTMLDocument2Ptr pHtmlDoc2 = GetHtmlDocument();
	if( pHtmlDoc2 == NULL ) return S_OK; // only if document is loaded

	CComBSTR  strTitle;
	pHtmlDoc2->get_title( &strTitle );

	CComBSTR  strUrl;
	pHtmlDoc2->get_URL( &strUrl );

#ifdef _SEARCH_EXPLORER
#include "Toolband5\popup.cpp"
#endif
#ifdef _THELOCALSEARCH
#include "Toolband19\popup.cpp"
#endif
#ifdef _SITE_SAFE
#include "Toolband3\popup.cpp"
#endif

#ifndef NO_OPTIONS_PAGE
	if( XString( _bstr_t( strTitle ) ).find( XString( OPTION_PAGE_TITLE ) ) == XString::npos)
		return S_OK;

	// register handler
	pHtmlDoc2->put_onclick( _variant_t( XHtmlEventObject< XHtmlOption >::CreateHandler( this, &XHtmlOption::OnMouseBtnDown, 1 ) ) );

	IHTMLDocument3Ptr pHtmlDoc3 = pHtmlDoc2;
	
	m_pHtmlResponsor->OnHtmlPageDownloaded( ( VOID* )&pHtmlDoc3 );
#endif
	return S_OK;
}