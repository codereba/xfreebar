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

#ifndef __XHTMLANALYZER_H__
#define __XHTMLANALYZER_H__

class XHtmlAnalyzer
{
public:
	XHtmlAnalyzer(void)
	{

	}

public:
	~XHtmlAnalyzer(void)
	{

	}

public:

	VOID SetIEWebBrowser( IWebBrowser2Ptr pWebBrowser )
	{
		m_pWebBrowser = pWebBrowser;
	}

	IHTMLDocument2Ptr GetHtmlDocument()
	{
		LPDISPATCH pDispatch = NULL;
		m_pWebBrowser->get_Document( &pDispatch );
		IHTMLDocument2Ptr pHtmlDoc2 = pDispatch;
		if( pDispatch )
			pDispatch->Release();
		return pHtmlDoc2;
	}

protected:
	IWebBrowser2Ptr m_pWebBrowser;
};

#endif //__XHTMLANALYZER_H__