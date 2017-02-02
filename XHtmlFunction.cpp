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
#include "XHtmlFunction.h"

XHtmlFunction::XHtmlFunction(void)
{
}

XHtmlFunction::~XHtmlFunction(void)
{
}

// set focus to current document helper
void XHtmlFunction::MoveFocusToHTML()
{
	IHTMLDocument2Ptr pHtmlDoc = GetHtmlDocument();
	if( pHtmlDoc )
	{
		IHTMLWindow2Ptr pHtmlWin = NULL;
		pHtmlDoc->get_parentWindow( &pHtmlWin );
		if( pHtmlWin )
			pHtmlWin->focus();
	}
}

