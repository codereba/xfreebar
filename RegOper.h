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
#include "CmnUtil.h"

INT32 RegisterXMLPath(LPCSTR lpszXmlPath);
void UnRegisterXMLPath();
INT32 GetXmlPath(_bstr_t &lpstrXmlPath);
INT32 RegisterConfigPath( LPCSTR lpszXmlPath );
void UnRegisterConfigPath();
INT32 GetConfigPath( _bstr_t &lpszXmlPath );

class RegIterator
{
public:
    RegIterator() : m_nIndex(0)
    {
		m_hKey.Create(GetAppRegistryKey(), _T("History"));
        m_nIndex = 0;
        operator++ (1);
    }

	RegIterator(const XString& id)
	{
		m_hKey.Create(GetAppRegistryKey(),getHistName(id));
		m_nIndex=0;
		operator++(1);
	}

    ~RegIterator() {}

	_bstr_t getValue() 
	{
		TCHAR cValue[1024];
		DWORD pdwCount = 1000;
		m_hKey.QueryValue(cValue, m_szBuf,&pdwCount);
		return cValue;
	}

    void operator++ (INT32)
    {
        unsigned long size = 200;
        m_bValid = ((::RegEnumValue(m_hKey, m_nIndex++, m_szBuf, &size, 0, 0, 0, 0)) != ERROR_NO_MORE_ITEMS);
    }

    operator bool() {return m_bValid;}
    operator TCHAR* () { return m_szBuf;}
    operator INT32() { return m_nIndex - 1;}

protected:
    CRegKey m_hKey;
    TCHAR m_szBuf[200];
    INT32 m_nIndex;
    bool m_bValid;
};