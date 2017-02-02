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
#include "XHtmlAnalyzer.h"
#include "XHtmlFunction.h"

struct IHtmlResponsor
{
	virtual INT32 OnHtmlPageDownloaded( VOID *pParam ) = 0;
	virtual INT32 OnMouseBtnDown( VOID *pParam ) = 0;
	virtual INT32 HtmlRespIsInited() = 0;
};

class XHtmlOption : public XHtmlFunction
{
public:
	XHtmlOption(void);
public:
	~XHtmlOption(void);

public:
	VOID OnMouseBtnDown( DISPID DispId, VARIANT* pVarResult );
	STDMETHODIMP OnDownloadComplete();
	VOID SetHtmlResponser( IHtmlResponsor *pHtmlResponsor )
	{
		ATLASSERT( NULL != pHtmlResponsor );
		m_pHtmlResponsor = pHtmlResponsor;
	}

protected:
	IHtmlResponsor *m_pHtmlResponsor;
};
