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
#include "XmlNode.h"

//-------------------------------------------------------------------------
// base on CXml CXmlNode CXmlNodes by sky-sea-earth@21cn.com(2005-10-07)
//-------------------------------------------------------------------------

class CXml
{
public:
	CXml(void);
	CXml(IXMLDOMDocumentPtr pDoc);
	~CXml(void);

protected:
	MSXML::IXMLDOMDocumentPtr m_pDoc; 
	_bstr_t						m_strFilePath;

public:
	BOOL Open(_bstr_t strXmlFilePath);
	BOOL Save(_bstr_t lpszFilePath = _T(""));
	void Close(void);

	_bstr_t GetLastError(void);

	CXmlNodePtr GetRoot(bstr_t strDefName = "TOOLBAR");	
	CXmlNodePtr AppendRoot(bstr_t strDefName = "TOOLBAR");

	static BOOL IsFileExist(_bstr_t strFilePath);
	CXmlNodePtr SelectSingleNode(_bstr_t lpszPath);
	CXmlNodesPtr SelectNodes(_bstr_t lpszPath);
};
