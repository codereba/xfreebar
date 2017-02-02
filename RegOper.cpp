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
#include ".\regoper.h"

const _bstr_t sxmlconfig = "XMLConfigFile";
const _bstr_t xmlpathvaluename = "path";
const _bstr_t xmlfilenameval = "filename";

extern _bstr_t xmlFileName;

INT32 RegisterConfigPath( LPCSTR lpszXmlPath )
{
	HKEY keyApp = GetAppRegistryKey();
	HKEY hKeyXmlPath;
	DWORD dw;
	DWORD dwKeyType = REG_BINARY;
	char cKeyType = REG_BINARY;
	RegCreateKeyEx(keyApp, sxmlconfig , 0, &cKeyType, 
		REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL,
		&hKeyXmlPath, &dw);
	RegCloseKey(keyApp);

	if(hKeyXmlPath != NULL)
	{
		RegSetValueEx(hKeyXmlPath, xmlpathvaluename, 0, dwKeyType, (LPBYTE)lpszXmlPath, strlen(lpszXmlPath));
		RegCloseKey(hKeyXmlPath);
		return 1;
	}

	return 0;
}

INT32 RegisterXMLPath( LPCSTR lpszXmlPath )
{
	HKEY keyApp = GetAppRegistryKey();
	HKEY hKeyXmlPath;
	DWORD dw;
	DWORD dwKeyType = REG_BINARY;
	char cKeyType = REG_BINARY;
	RegCreateKeyEx(keyApp, sxmlconfig , 0, &cKeyType, 
		REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL,
		&hKeyXmlPath, &dw);
	RegCloseKey(keyApp);

	if(hKeyXmlPath != NULL)
	{
		RegSetValueEx(hKeyXmlPath, xmlfilenameval, 0, dwKeyType, (LPBYTE)lpszXmlPath, strlen(lpszXmlPath));
		RegCloseKey(hKeyXmlPath);
		return 1;
	}

	return 0;
}

void UnRegisterXMLPath()
{
	RegisterXMLPath("");
}

void UnRegisterConfigPath()
{
	HKEY keyApp = GetAppRegistryKey();
	RegDeleteKey(keyApp, sxmlconfig);
	RegCloseKey(keyApp);
}

INT32 GetConfigPath(_bstr_t &strXmlPath)
{
	HKEY keyApp = GetAppRegistryKey();
	HKEY hKeyXmlPath;
	DWORD dwValLen = MAX_PATH;
	DWORD dwKeyType = REG_BINARY;
	LONG lRet;

	char szXmlPath[MAX_PATH];
	ZeroMemory(szXmlPath, sizeof(szXmlPath));

	RegOpenKeyEx(keyApp, sxmlconfig , 0, KEY_WRITE | KEY_READ,
		&hKeyXmlPath);
	RegCloseKey(keyApp);

	if(hKeyXmlPath != NULL)
	{
		lRet = RegQueryValueEx(hKeyXmlPath, xmlpathvaluename, 0, &dwKeyType, (LPBYTE)szXmlPath, &dwValLen);
		if(lRet)
			return 0;
		strXmlPath = szXmlPath;
		RegCloseKey(hKeyXmlPath);
		return 1;
	}
	return 0;
}

INT32 GetXmlPath(_bstr_t &strXmlPath)
{
	HKEY keyApp = GetAppRegistryKey();
	HKEY hKeyXmlPath;
	DWORD dwValLen = MAX_PATH;
	DWORD dwKeyType = REG_BINARY;
	LONG lRet;

	char szXmlPath[MAX_PATH];
	ZeroMemory(szXmlPath, sizeof(szXmlPath));

	RegOpenKeyEx(keyApp, sxmlconfig , 0, KEY_WRITE | KEY_READ,
		&hKeyXmlPath);
	RegCloseKey(keyApp);

	if(hKeyXmlPath != NULL)
	{
		lRet = RegQueryValueEx(hKeyXmlPath, xmlfilenameval, 0, &dwKeyType, (LPBYTE)szXmlPath, &dwValLen);
		if(lRet)
			return 0;
		strXmlPath = szXmlPath;
		RegCloseKey(hKeyXmlPath);
		return 1;
	}
	return 0;
}
