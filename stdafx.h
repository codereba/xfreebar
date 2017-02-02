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

#define ISOLATION_AWARE_ENABLED 1

#ifndef STRICT
#define STRICT
#endif

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows 95 �� Windows NT 4 ����߰汾�Ĺ��ܡ�
#define WINVER 0x0500		// ���˸���Ϊ����� Windows 98 �� Windows 2000 ����߰汾�ĺ��ʵ�ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows NT 4 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0400	// ���˸���Ϊ����� Windows 2000 ����߰汾�ĺ��ʵ�ֵ��
#endif							

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ���˸���Ϊ����� Windows Me ����߰汾�ĺ��ʵ�ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 4.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0500	// ���˸���Ϊ����� IE 5.0 ����߰汾�ĺ��ʵ�ֵ��
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_USE_CSTRING
#define _ATL_USE_CSTRING_FLOAT
#define _ATL_USE_DDX_FLOAT

#define _ATL_LPCTSTR_EXPLICIT_CONSTRUCTORS	// ĳЩ LPCTSTR ���캯����Ϊ��ʽ��

// �ر� ATL ��ĳЩ������ȫ���Եĳ���������Ϣ������
#define _ATL_ALL_WARNINGS

#include "resource.h"
#include <atlbase.h>
#include <comdef.h>
#include <atlcom.h>

#include <atlapp.h>

#include <atlcrack.h>

#include <atlwin.h>
#include <atlctrls.h>
#include <AtlCtrlx.h>
#include <atlCtrlw.h>
#include <atlmisc.h>
#include <atlframe.h>

#include <winuser.h>
#include "Global.h"
#include <sstream>
#include "commctrl.h"

#pragma warning(disable: 4251) //'' : class '' needs to have dll-interface to be used by clients of class ''
#pragma warning(disable: 4275)  //non dll-interface class
#pragma warning(disable: 4786) //identifier was truncated to '255' characters in the debug information

#define OCM_COMMAND_CODE_HANDLER(code, func) \
	if(uMsg == OCM_COMMAND && code == HIWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define OCM_COMMAND_ID_HANDLER(id, func) \
	if(uMsg == OCM_COMMAND && id == LOWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if (bHandled) \
			return TRUE; \
	}

#define OCM_NOTIFY_CODE_HANDLER(cd, func) \
	if(uMsg == OCM_NOTIFY && cd == ((LPNMHDR)lParam)->code) \
	{ \
		bHandled = TRUE; \
		lResult = func((INT32)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define OCM_NOTIFY_ID_HANDLER(id, func) \
	if(uMsg == OCM_NOTIFY && id == ((LPNMHDR)lParam)->idFrom) \
	{ \
		bHandled = TRUE; \
		lResult = func((INT32)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define OCM_NOTIFY_HANDLER(id, cd, func) \
	if(uMsg == OCM_NOTIFY && id == ((LPNMHDR)lParam)->idFrom, && cd ((LPNMHDR)lParam)->code) \
	{ \
		bHandled = TRUE; \
		lResult = func((INT32)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}