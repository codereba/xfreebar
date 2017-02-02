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

#pragma once
#include "xhtmlanalyzer.h"

#define SEARCH_FLAG_WHOLE_WORD 2
#define SEARCH_FLAG_ALL 0
class XHtmlSearch :
	public XHtmlAnalyzer
{
public:
	XHtmlSearch(void);
public:
	virtual ~XHtmlSearch(void);

public:
	void ClearSearchResults( _bstr_t strSearchId = "XFree" );

	void ClearSearchResults( IHTMLDocument2 *lpHtmlDocument,
							_bstr_t strSearchId = "XFree" );
	
	VOID SearchTextInHTML( _bstr_t strSearchText, 
							LONG lFlags = 0, 
							BOOL bNNF = FALSE );
	
	VOID SearchAndTipTextInHTML( _bstr_t strSearchText, 
							LONG lFlags, 
							_bstr_t strMatchStyle, 
							_bstr_t strSearchId = "XFree" );
	
	VOID SearchTextInHTML( IHTMLDocument2 *lpHtmlDocument,
							_bstr_t strSearchText, 
							LONG lFlags = 0, 
							BOOL bNNF =FALSE );
	
	VOID SearchAndTipTextInHTML( IHTMLDocument2 *lpHtmlDocument, 
								_bstr_t strSearchText, 
								LONG lFlags, 
								_bstr_t strMatchStyle = _T(""), 
								_bstr_t strSearchId = "XFree" );
};
