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

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

using namespace ATL;

_COM_SMARTPTR_TYPEDEF(IHTMLDocument2, __uuidof(IHTMLDocument2));
_COM_SMARTPTR_TYPEDEF(IWebBrowser, __uuidof(IWebBrowser));
_COM_SMARTPTR_TYPEDEF(IShellWindows, __uuidof(IShellWindows));
_COM_SMARTPTR_TYPEDEF(IHTMLEventObj, __uuidof(IHTMLEventObj));
_COM_SMARTPTR_TYPEDEF(IHTMLWindow2, __uuidof(IHTMLWindow2));
_COM_SMARTPTR_TYPEDEF(IHTMLOptionElement, __uuidof(IHTMLOptionElement));
_COM_SMARTPTR_TYPEDEF(IHTMLElement, __uuidof(IHTMLElement));
_COM_SMARTPTR_TYPEDEF(IWebBrowser2, __uuidof(IWebBrowser2));
_COM_SMARTPTR_TYPEDEF(IHTMLDocument2, __uuidof(IHTMLDocument2));

_COM_SMARTPTR_TYPEDEF(IHTMLElement2, __uuidof(IHTMLElement2));
_COM_SMARTPTR_TYPEDEF(IHTMLInputElement, __uuidof(IHTMLInputElement));
_COM_SMARTPTR_TYPEDEF(IHTMLSelectElement, __uuidof(IHTMLSelectElement));
_COM_SMARTPTR_TYPEDEF(IHTMLDocument3, __uuidof(IHTMLDocument3));
_COM_SMARTPTR_TYPEDEF(IDispatch, __uuidof(IDispatch));

class XComModule : public CComModule
{
public:
	XComModule()
	{
	}

	~XComModule()
	{
	}
};

extern CComModule _Module;

#import "msxml.tlb"  named_guids
using namespace std;
#include <string>

typedef int INT32;
typedef std::basic_string<TCHAR> XString;

#include "XErrorCode.h"
#include "GlobalDef.h"
#include "CmnUtil.h"
#include "XLogger.h"

extern XLogger g_XLog;

#define LOG_INFO_MSG_LEVEL 0
#define LOG_EXCL_MSG_LEVEL 1
#define LOG_FATAL_MSG_LEVEL 2
#define XMODE_LOG( level, mode, output ) g_XLog.Print( level, mode, ( output ) );
#ifdef _DEBUG
#define XLOG( output ) g_XLog.Print output;
#else
#define XLOG( output ) g_XLog.Print output;
#endif

#include "XToolBarConfig.h"

#endif //__GLOBAL_H__