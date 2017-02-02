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


#include ".\XmlNode.h"


class CXmlNodes
{
	friend class CXml;
	friend class CXmlNode;
	friend class CXmlNodes;

public:
	~CXmlNodes(void);
	CXmlNodes(void);
	CXmlNodes( CXmlNodes & refNodes );

	CXmlNodesPtr operator = (CXmlNodesPtr pNodes);
	CXmlNodes & operator = (CXmlNodes & refNodes);
	CXmlNodePtr operator[] ( LONG lIndex );
	BOOL IsNull(void);
	//CXmlNodePtr operator[] ( _bstr_t lpszName );


	LONG GetCount(void);

	CXmlNodePtr GetItem( LONG nIndex );
	CXmlNodePtr GetItem( _bstr_t strAttribName, _bstr_t strAttribVal );

protected:
	CXmlNodes(MSXML::IXMLDOMNodeListPtr pNodeList);
	MSXML::IXMLDOMNodeListPtr m_pNodeList;
	
};
