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
#include ".\xmlnodes.h"


// 下面是构造函数 和 析构函数
CXmlNodes::CXmlNodes(MSXML::IXMLDOMNodeListPtr pNodeList)
{
	m_pNodeList = pNodeList;
}

CXmlNodes::CXmlNodes( CXmlNodes & refNodes )
{
	m_pNodeList = refNodes.m_pNodeList;
}

CXmlNodes::CXmlNodes(void)
: m_pNodeList(NULL)
{
}

CXmlNodes::~CXmlNodes(void)
{
	SAFE_RELEASE(m_pNodeList);
}

//////////////////////////////////////////////////////////////////////////



//-------------------------------------------------------------------------
// Function Name    :operator =
// Parameter(s)     :CXmlNodesPtr pNodes
// Return           :CXmlNodesPtr
// Memo             :重载 = 号
//-------------------------------------------------------------------------
CXmlNodesPtr CXmlNodes::operator = ( CXmlNodesPtr pNodes IN )
{
	SAFE_RELEASE(m_pNodeList);

	m_pNodeList = pNodes->m_pNodeList;
	return pNodes;
}


//-------------------------------------------------------------------------
// Function Name    :operator =
// Parameter(s)     :CXmlNodes & refNodes
// Return           :CXmlNodes &
// Memo             :重载 = 号
//-------------------------------------------------------------------------
CXmlNodes & CXmlNodes::operator = (CXmlNodes & refNodes)
{
	SAFE_RELEASE(m_pNodeList);

	m_pNodeList = refNodes.m_pNodeList;
	return (*this);
}



//-------------------------------------------------------------------------
// Function Name    :GetCount
// Parameter(s)     :void
// Return           :LONG
// Memo             :获取数量
//-------------------------------------------------------------------------
LONG CXmlNodes::GetCount(void)
{
	ATLASSERT( m_pNodeList != NULL );

	LONG lCount = 0l;
	HRESULT hr = S_OK;
	MSXML::DOMNodeType NodeType;
	MSXML::IXMLDOMNodePtr pNode = NULL;
	
	for( INT32 i = 0; i < m_pNodeList->length; i++)
	{
		pNode = m_pNodeList->item[i];

		hr = pNode->get_nodeType(&NodeType);
		ATLASSERT( SUCCEEDED(hr) );

		if( NodeType == MSXML::NODE_ELEMENT )
		{
			lCount ++;
		}

		SAFE_RELEASE(pNode);
	}

	return lCount;
}



//-------------------------------------------------------------------------
// Function Name    :GetItem
// Parameter(s)     :LONG nIndex	索引
// Return           :CXmlNodePtr
// Memo             :获取集合中的结点
//-------------------------------------------------------------------------
CXmlNodePtr CXmlNodes::GetItem( LONG nIndex )
{
	ATLASSERT( nIndex >= 0 && nIndex < GetCount() );

	LONG lCount = 0l;
	HRESULT hr = S_OK;
	MSXML::DOMNodeType NodeType;
	MSXML::IXMLDOMNodePtr pItem = NULL;

	CXmlNodePtr pNode ( new CXmlNode() );

	for( INT32 i = 0; i < m_pNodeList->length; i++)
	{
		pItem = m_pNodeList->item[i];

		hr = pItem->get_nodeType(&NodeType);
		ATLASSERT( SUCCEEDED(hr) );


		if( NodeType == MSXML::NODE_ELEMENT )
		{
			if( lCount == nIndex )
			{
				pNode->m_pNode = pItem;
				SAFE_RELEASE(pItem);
				return pNode;
			}

			lCount ++;
		}

		SAFE_RELEASE(pItem);
	}

	return pNode;
}


//-------------------------------------------------------------------------
// Function Name    :operator[]
// Parameter(s)     :INT nIndex
// Return           :CXmlNodePtr
// Memo             :作用同 GetItem( LONG nIndex )
//-------------------------------------------------------------------------
CXmlNodePtr CXmlNodes::operator[] ( LONG lIndex )
{
	return GetItem(lIndex);
}


CXmlNodePtr CXmlNodes::GetItem( _bstr_t strAttribName, _bstr_t strAttribVal )
{
	ATLASSERT( strAttribName.length() != 0 );
	
	HRESULT hr = S_OK;
	MSXML::IXMLDOMNodePtr pItem = NULL;
	bstr_t strValue;
	CXmlNodePtr pNode ( new CXmlNode() );

	for( INT32 i = 0; i < m_pNodeList->length; i++)
	{
		pItem = m_pNodeList->item[i];

		MSXML::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;
		hr = pItem->get_attributes(&pIXMLDOMNamedNodeMap);
		if( !SUCCEEDED(hr) )
		{
			return pNode;
		}

		MSXML::IXMLDOMNodePtr pIXMLDOMNode = NULL;
		pIXMLDOMNode = pIXMLDOMNamedNodeMap->getNamedItem( strAttribName );

		if( pIXMLDOMNode == NULL )
		{
			return pNode;
		}
		else
		{
			VARIANT varValue;
			hr = pIXMLDOMNode->get_nodeValue(&varValue);
			if( !SUCCEEDED(hr) )
			{
				return pNode;
			}

			strValue = varValue;
		}

		pIXMLDOMNode = NULL;
		pIXMLDOMNamedNodeMap = NULL;

		if( strValue == strAttribVal)
		{
			pNode->m_pNode = pItem;
			SAFE_RELEASE(pItem);
			return pNode;
		}

		SAFE_RELEASE(pItem);
	}

	return pNode;
}

BOOL CXmlNodes::IsNull(void)
{
	return m_pNodeList == NULL;
}

//CXmlNodePtr CXmlNodes::operator[] ( _bstr_t lpszName )
//{
//	return GetItem(lpszName);
//}
