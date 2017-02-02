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

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "XToolBarHost.h"
#include "dlldatax.h"
#include "XFreeBar.h"
#include "regoper.h"

#define XLOG_WORK_LEVEL 9
#ifdef _DEBUG
#define XLOG_OUTPUT_MODE LOG_TO_DEBUGER | LOG_TO_FILE
#else
#define XLOG_OUTPUT_MODE LOG_TO_FILE
#endif

CComModule _Module;
XLogger g_XLog( NULL, XLOG_WORK_LEVEL, XLOG_OUTPUT_MODE );

BEGIN_OBJECT_MAP( ObjectMap)
	OBJECT_ENTRY(CLSID_XToolBarHost, XToolBarHost)
END_OBJECT_MAP()

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{

#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif

	if(dwReason == DLL_PROCESS_ATTACH)
	{
		_Module.Init( ObjectMap, hInstance, &LIBID_XFreeBarLib );
		DisableThreadLibraryCalls( hInstance );
	}
	else if ( dwReason == DLL_PROCESS_DETACH )
		_Module.Term();

	return TRUE; 
}

STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hr = PrxDllCanUnloadNow();
    if (FAILED(hr))
        return hr;
#endif
	return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
	return _Module.GetClassObject(rclsid, riid, ppv);
}

void ShowBand();
void UnRegisterServer();

// DllRegisterServer 
STDAPI DllRegisterServer(void)
{
	TCHAR szXmlConfDir[_MAX_PATH];
	if( 0 > GetXmlConfDir( szXmlConfDir, _MAX_PATH ) )
		return S_FALSE;

	RegisterXMLPath( XML_CONF_FILE_NAME );
	RegisterConfigPath( szXmlConfDir );

	HRESULT hr = _Module.RegisterServer(TRUE);
#ifdef _MERGE_PROXYSTUB
    if ( FAILED( hr ) )
        return hr;
    hr = PrxDllRegisterServer();
#endif

	ShowBand();
	return hr;
}


// DllUnregisterServer 
STDAPI DllUnregisterServer(void)
{
	UnRegisterXMLPath();
	UnRegisterConfigPath();
	HRESULT hr = _Module.UnregisterServer();

#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
    if (FAILED(hr))
        return hr;
    hr = PrxDllUnregisterServer();
#endif
	return hr;
}
