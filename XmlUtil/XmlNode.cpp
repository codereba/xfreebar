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
#include ".\xmlnode.h"
#include "xml.h"




//-------------------------------------------------------------------------
// Function Name    :CXmlNode
// Parameter(s)     : MSXML::IXMLDOMNodePtr pNode	[in]
// Return           :
// Memo             :构造器
//-------------------------------------------------------------------------
CXmlNode::CXmlNode( MSXML::IXMLDOMNodePtr pNode IN)
{
	m_pNode = pNode;
}


//-------------------------------------------------------------------------
// Function Name    :CXmlNode
// Parameter(s)     :void
// Return           :
// Memo             :构造器
//-------------------------------------------------------------------------
CXmlNode::CXmlNode(void)
{
	m_pNode = NULL;	
}


//-------------------------------------------------------------------------
// Function Name    :CXmlNode
// Parameter(s)     :CXmlNode & refNode [in]
// Return           :
// Memo             :拷贝构造函数
//-------------------------------------------------------------------------
CXmlNode::CXmlNode( CXmlNode & refNode IN)
{
	m_pNode = refNode.m_pNode;
}


//-------------------------------------------------------------------------
// Function Name    :~CXmlNode
// Parameter(s)     :void
// Return           :
// Memo             :析构函数
//-------------------------------------------------------------------------
CXmlNode::~CXmlNode(void)
{
	m_pNode = NULL;
}





//-------------------------------------------------------------------------
// Function Name    :operator =
// Parameter(s)     :CXmlNodePtr pNode	[in]
// Return           :CXmlNodePtr
// Memo             :重载 = 操作符
//-------------------------------------------------------------------------
CXmlNodePtr CXmlNode::operator = ( CXmlNodePtr pNode IN)
{
	m_pNode = pNode->m_pNode;
	return pNode;
}


//-------------------------------------------------------------------------
// Function Name    :operator =
// Parameter(s)     :CXmlNode & refNode	[in]
// Return           :CXmlNode &
// Memo             :重载 = 操作符
//-------------------------------------------------------------------------
CXmlNode & CXmlNode::operator = ( CXmlNode & refNode IN)
{
	m_pNode = refNode.m_pNode;
	return (*this);
}


//-------------------------------------------------------------------------
// Function Name    :IsNull
// Parameter(s)     :
// Return           :BOOL
// Memo             :判断此结点是否是空节点
//-------------------------------------------------------------------------
BOOL CXmlNode::IsNull(void)
{
	return m_pNode == NULL;
}



//-------------------------------------------------------------------------
// Function Name    :GetChild
// Parameter(s)     :_bstr_t strName	节点名称
// Return           :儿子节点指针
// Memo             :获取儿子
//-------------------------------------------------------------------------
CXmlNodePtr CXmlNode::GetChild(_bstr_t strName IN)
{
	ATLASSERT( m_pNode != NULL );

	MSXML::IXMLDOMNodePtr pChildNode = NULL;
	pChildNode = m_pNode->selectSingleNode(strName);
	
	if( pChildNode == NULL )
	{
		return CXmlNodePtr(new CXmlNode());
	}

	CXmlNodePtr pChild( new CXmlNode(pChildNode) );
	
	SAFE_RELEASE(pChildNode);

	return pChild;
}

CXmlNodePtr CXmlNode::GetChild(_bstr_t strAttribName, _bstr_t strAttribVal IN)
{
	ATLASSERT( m_pNode != NULL);
	ATLASSERT( strAttribName.length() != 0 );
	HRESULT hr = S_OK;
	MSXML::IXMLDOMNodeListPtr pNodeList = NULL;
	hr = m_pNode->get_childNodes(&pNodeList);
	if( !SUCCEEDED(hr) )
	{
		return CXmlNodePtr(new CXmlNode());
	}



	MSXML::IXMLDOMNodePtr pItem = NULL;
	bstr_t strValue;
	CXmlNodePtr pNode ( new CXmlNode() );

	for( INT32 i = 0; i < pNodeList->length; i++)
	{
		pItem = pNodeList->item[i];

		MSXML::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;
		hr = pItem->get_attributes(&pIXMLDOMNamedNodeMap);
		if( !SUCCEEDED(hr) )
		{
			continue;
		}

		MSXML::IXMLDOMNodePtr pIXMLDOMNode = NULL;
		pIXMLDOMNode = pIXMLDOMNamedNodeMap->getNamedItem( strAttribName );

		if( pIXMLDOMNode == NULL )
		{
			continue;
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

CXmlNodePtr CXmlNode::AppendChild(bstr_t strName)
{
	ATLASSERT( m_pNode != NULL );
	ATLASSERT(strName.length() != 0);

	MSXML::IXMLDOMDocumentPtr pDoc = NULL;
	HRESULT hr = m_pNode->get_ownerDocument(&pDoc);
	if(!SUCCEEDED(hr) )
	{
		return CXmlNodePtr(new CXmlNode());
	}

	MSXML::IXMLDOMNodePtr pChildNode = pDoc->createElement( strName );
	if( pChildNode == NULL )
	{
		return CXmlNodePtr(new CXmlNode());
	}

	m_pNode->appendChild(pChildNode);

	SAFE_RELEASE(pDoc);
	return CXmlNodePtr(new CXmlNode(pChildNode));
}


CXmlNodePtr CXmlNode:: InsertSiblingBefore(bstr_t strName)
{
	ATLASSERT( m_pNode != NULL );
	ATLASSERT(strName.length() != 0);

	MSXML::IXMLDOMDocumentPtr pDoc = NULL;
	HRESULT hr = m_pNode->get_ownerDocument(&pDoc);
	if(!SUCCEEDED(hr) )
	{
		return CXmlNodePtr(new CXmlNode());
	}

	MSXML::IXMLDOMNodePtr pChildNode = pDoc->createElement( strName );
	if( pChildNode == NULL )
	{
		return CXmlNodePtr(new CXmlNode());
	}

	variant_t varTemp;
	varTemp = (IDispatch*)m_pNode; 
	MSXML::IXMLDOMNodePtr pParentNode = NULL;
	hr = m_pNode->get_parentNode(&pParentNode);
	if( !SUCCEEDED(hr) )
	{
		return CXmlNodePtr(new CXmlNode());
	}

	pParentNode->insertBefore(pChildNode, varTemp);

	SAFE_RELEASE(pDoc);
	SAFE_RELEASE(pParentNode);
	return CXmlNodePtr(new CXmlNode(pChildNode));;
}


BOOL CXmlNode::RemoveChild(bstr_t strName)
{
	ATLASSERT( m_pNode != NULL );
	ATLASSERT(strName.length() != 0);

	MSXML::IXMLDOMNodePtr pChildNode = m_pNode->selectSingleNode(strName);

	if(pChildNode == NULL)
	{
		return FALSE;
	}
	m_pNode->removeChild(pChildNode);
	SAFE_RELEASE(pChildNode);
	return TRUE;
}

BOOL CXmlNode::RemoveChild(CXmlNodePtr pXmlNode)
{
	ATLASSERT( m_pNode != NULL );
	ATLASSERT(pXmlNode->m_pNode != NULL);
	
	m_pNode->removeChild(pXmlNode->m_pNode);
	return TRUE;
}

CXmlNodePtr CXmlNode::GetFirstChild()
{
	ATLASSERT( m_pNode != NULL );

	MSXML::IXMLDOMNodePtr pChildNode = NULL;
	pChildNode = m_pNode->GetfirstChild();
	
	if( pChildNode == NULL )
	{
		return CXmlNodePtr(new CXmlNode());
	}

	CXmlNodePtr pChild( new CXmlNode(pChildNode) );
	
	SAFE_RELEASE(pChildNode);

	return pChild;
}

CXmlNodePtr CXmlNode::GetNextSibling()
{
	ATLASSERT( m_pNode != NULL );

	MSXML::IXMLDOMNodePtr pChildNode = NULL;
	pChildNode = m_pNode->GetnextSibling();
	
	if( pChildNode == NULL )
	{
		return CXmlNodePtr(new CXmlNode());
	}

	CXmlNodePtr pChild( new CXmlNode(pChildNode) );
	
	SAFE_RELEASE(pChildNode);

	return pChild;
}

INT32 CXmlNode::HasChilds()
{
	ATLASSERT( m_pNode != NULL );

	VARIANT_BOOL bRet = m_pNode->hasChildNodes();
	if(!bRet)
		return FALSE;
	return TRUE;
}

//-------------------------------------------------------------------------
// Function Name    :_GetValue
// Parameter(s)     :_bstr_t & strValue IN	值
// Return           :BOOL	是否成功
// Memo             :获取当前节点的值
//-------------------------------------------------------------------------
BOOL CXmlNode::_GetValue(_bstr_t & strValue OUT)
{
	ATLASSERT( m_pNode != NULL );

	if( HasChildren() )
	{
		// 下面有子结点
		//ATLASSERT( FALSE );
		return FALSE;
	}

	HRESULT hr = m_pNode->get_text( &(strValue.GetBSTR()) );
	if (SUCCEEDED(hr) )
	{
		return FALSE;
	}

	if( strValue.length() == 0 )
	{
		strValue = "";
	}
	
	return SUCCEEDED(hr);
}



//-------------------------------------------------------------------------
// Function Name    :_SetValue
// Parameter(s)     :_bstr_t & strValue IN
// Return           :BOOL	是否成功
// Memo             :获取当前节点的值
//-------------------------------------------------------------------------
BOOL CXmlNode:: _SetValue(_bstr_t & strValue IN)
{
	ATLASSERT( m_pNode != NULL );

	if( HasChildren() )
	{
		// 下面有子结点 
		//ATLASSERT( FALSE );
		return FALSE;
	}
	
	HRESULT hr = S_OK;
	hr = m_pNode->put_text( strValue);
	if( !SUCCEEDED(hr) )
	{
		return FALSE;
	}

	return SUCCEEDED(hr);
}



//-------------------------------------------------------------------------
// Function Name    :GetName
// Parameter(s)     :void
// Return           :_bstr_t
// Memo             :获取当前节点名称
//-------------------------------------------------------------------------
BOOL CXmlNode::GetName(_bstr_t &strName IN OUT)
{
	ATLASSERT( m_pNode != NULL );

	HRESULT hr = m_pNode->get_nodeName(&strName.GetBSTR());
	if( !SUCCEEDED(hr) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CXmlNode::CheckName(bstr_t strChecked)
{
	bstr_t strName;
	if(!GetName(strName))
		return FALSE;
	if(strName != strChecked)
		return FALSE;
	return TRUE;
}

BOOL CXmlNode::ModifyName(bstr_t strNewName)
{
	MSXML::IXMLDOMDocumentPtr pDoc = NULL;
	HRESULT hr = m_pNode->get_ownerDocument(&pDoc);
	if( !SUCCEEDED(hr) )
	{
		return FALSE;
	}

	MSXML::IXMLDOMNodePtr pChildNode = pDoc->createElement( strNewName );
	if( pChildNode == NULL )
	{
		return FALSE;
	}

	variant_t varTemp;
	varTemp = (IDispatch*)m_pNode; 
	MSXML::IXMLDOMNodePtr pParentNode = NULL;
	hr = m_pNode->get_parentNode(&pParentNode);
	if( !SUCCEEDED(hr) )
	{
		return FALSE;
	}

	pParentNode->insertBefore(pChildNode, varTemp);
	pParentNode->removeChild(m_pNode);

	SAFE_RELEASE(pDoc);
	SAFE_RELEASE(pParentNode);
	SAFE_RELEASE(pChildNode);
	return TRUE;
}




//-------------------------------------------------------------------------
// Function Name    :GetAttribute
// Parameter(s)     :_bstr_t strName		属性名
//					:_bstr_t lpszDefault	默认值
// Return           :_bstr_t
// Memo             :获取属性值
//-------------------------------------------------------------------------
_bstr_t CXmlNode::GetAttribute( _bstr_t strName IN
							   , _bstr_t lpszDefault /* = NULL */ IN)
{
	ATLASSERT( m_pNode != NULL );

	HRESULT hr = S_OK;
	MSXML::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;
	hr = m_pNode->get_attributes(&pIXMLDOMNamedNodeMap);
	if( !SUCCEEDED(hr) )
	{
		return bstr_t("");
	}

	MSXML::IXMLDOMNodePtr pIXMLDOMNode = NULL;
	pIXMLDOMNode = pIXMLDOMNamedNodeMap->getNamedItem( strName );
	
	_bstr_t strValue;
	if( pIXMLDOMNode == NULL )
	{
		return bstr_t("");
	}
	else
	{
		VARIANT varValue;
		hr = pIXMLDOMNode->get_nodeValue(&varValue);
		if( !SUCCEEDED(hr) )
		{
			return  bstr_t("");
		}
		strValue = varValue;
	}

	pIXMLDOMNode = NULL;

	pIXMLDOMNamedNodeMap = NULL;
	return strValue;
}


//-------------------------------------------------------------------------
// Function Name    :SetAttribute
// Parameter(s)     :_bstr_t strValue	值
// Return           :BOOL
// Memo             :设置属性值
//-------------------------------------------------------------------------
BOOL CXmlNode::SetAttribute( _bstr_t strName IN
							, _bstr_t strValue IN)
{
	ATLASSERT( m_pNode != NULL );

	HRESULT hr = S_OK;
	MSXML::IXMLDOMElementPtr pElement = NULL;
	pElement = static_cast<IXMLDOMElementPtr> (m_pNode);
	hr = pElement->setAttribute( strName, _variant_t(strValue) );
	if( !SUCCEEDED(hr) )
	{
		return FALSE;
	}

	SAFE_RELEASE(pElement);
    
	return SUCCEEDED(hr);
}

//-------------------------------------------------------------------------
// Function Name    :DelAttribute
// Parameter(s)     :_bstr_t strName IN 属性名称
// Return           :BOOL
// Memo             :删除属性
//-------------------------------------------------------------------------
BOOL CXmlNode::DelAttribute( _bstr_t strName IN )
{
	ATLASSERT( m_pNode != NULL );

	HRESULT hr = S_OK;
	MSXML::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;
	hr = m_pNode->get_attributes(&pIXMLDOMNamedNodeMap);
	if( !SUCCEEDED(hr) )
	{
		return FALSE;
	}

	hr = pIXMLDOMNamedNodeMap->removeNamedItem( strName);
	if( !SUCCEEDED(hr) )
	{
		return FALSE;
	}


	SAFE_RELEASE(pIXMLDOMNamedNodeMap);
	return SUCCEEDED(hr);
}



//-------------------------------------------------------------------------
// Function Name    :HasChildren
// Parameter(s)     :
// Return           :
// Memo             :是否有子结点
//-------------------------------------------------------------------------
BOOL CXmlNode::HasChildren(void)
{
	ATLASSERT( m_pNode != NULL );

	BOOL bHasChildren = FALSE;

	HRESULT hr = S_OK;
	MSXML::IXMLDOMNodePtr pNode = NULL;
	MSXML::DOMNodeType NodeType;
	MSXML::IXMLDOMNodeListPtr pNodeList = NULL;
	hr = m_pNode->get_childNodes(&pNodeList);
	if( !SUCCEEDED(hr) )
	{
		return FALSE;
	}

	for( INT32 i = 0; i < pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		
		hr = pNode->get_nodeType(&NodeType);
		if( !SUCCEEDED(hr) )
		{
			return FALSE;
		}
		
		SAFE_RELEASE(pNode);

		if( NodeType == MSXML::NODE_ELEMENT )
		{
			bHasChildren = TRUE;
			break;
		}
	}

	SAFE_RELEASE(pNodeList);

	return bHasChildren;
}



//-------------------------------------------------------------------------
// Function Name    :Remove
// Parameter(s)     :
// Return           :
// Memo             :删除此结点
//-------------------------------------------------------------------------
BOOL CXmlNode::Remove(void)
{
	ATLASSERT( m_pNode != NULL );

	HRESULT hr = S_OK;
	MSXML::IXMLDOMNodePtr pNode = NULL;
	hr = m_pNode->get_parentNode(&pNode);
	if( !SUCCEEDED(hr) )
	{
		return FALSE;
	}

	
	pNode->removeChild(m_pNode);

	return TRUE;
}

//-------------------------------------------------------------------------
// Function Name    :RemoveChildren
// Parameter(s)     :
// Return           :BOOL
// Memo             :删除子结点
//-------------------------------------------------------------------------
BOOL CXmlNode::RemoveChildren(void)
{
	ATLASSERT( m_pNode != NULL );

	if( !HasChildren() )
		return TRUE;
	
	HRESULT hr = S_OK;
	MSXML::IXMLDOMNodePtr pNode = NULL;
	MSXML::DOMNodeType NodeType;
	MSXML::IXMLDOMNodeListPtr pNodeList = NULL;
	hr = m_pNode->get_childNodes(&pNodeList);
	if( !SUCCEEDED(hr) )
	{
		return FALSE;
	}

	for( INT32 i = 0; i < pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];

		hr = pNode->get_nodeType(&NodeType);
		if( !SUCCEEDED(hr) )
		{
			return FALSE;
		}


		if( NodeType == MSXML::NODE_ELEMENT )
		{
			m_pNode->removeChild(pNode);
		}

		SAFE_RELEASE(pNode);
	}

	SAFE_RELEASE(pNodeList);
	return TRUE;
}


//-------------------------------------------------------------------------
// Function Name    :GetChildren
// Parameter(s)     :void
// Return           :CXmlNodesPtr
// Memo             :获取子结点
//-------------------------------------------------------------------------
CXmlNodesPtr CXmlNode::GetChildren()
{
	ATLASSERT( m_pNode != NULL );

	HRESULT hr = S_OK;
	MSXML::IXMLDOMNodeListPtr pNodeList = NULL;
	hr = m_pNode->get_childNodes(&pNodeList);
	if( !SUCCEEDED(hr) )
	{
		return CXmlNodesPtr(new CXmlNodes());
	}


	CXmlNodesPtr pNodes(new CXmlNodes(pNodeList));

	return pNodes;
}




//////////////////////////////////////////////////////////////////////////
// 下列为取值的重载方法

_bstr_t CXmlNode::GetString(_bstr_t lpszValue /* = NULL */ IN)
{
	_bstr_t strValue;
	_GetValue(strValue);

	if( strValue.length() == 0 &&
		lpszValue.length() != 0 )
	{
		strValue = lpszValue;
		_SetValue(strValue);
	}

	return strValue;
}



BOOL CXmlNode::GetBOOL( BOOL bDefault /* = FALSE */ IN)
{
	_bstr_t strValue;
	_bstr_t strTemp;
	_GetValue(strValue);

	strTemp = _T("true");
	if( strValue != strTemp )
	{
		return TRUE;
	}
	else
	{
		strTemp = _T("false");
		if( strValue != strTemp )
		{
			return FALSE;
		}
		else
		{
			strValue = bDefault ? "true" : "false";
			_SetValue(strValue);
			return bDefault;
		}	
	}
}


INT CXmlNode::GetInt( INT nDefault /* = 0 */ IN)
{
	_bstr_t strValue;
	_GetValue(strValue);
	char szIntVal[50];

	if( strValue.length() == 0 )
	{
		sprintf(szIntVal,"%ld", nDefault);
		strValue = szIntVal;
		_SetValue(strValue);
	}

	return atoi(szIntVal);
}



DOUBLE CXmlNode::GetFloat( DOUBLE fDefault /* = 0.0 */)
{
	_bstr_t strValue;
	_GetValue(strValue);
	char szFloatVal[50];

	if( strValue.length() == 0 )
	{
		sprintf(szFloatVal, "%f", fDefault);
		strValue = szFloatVal;
		_SetValue(strValue);
	}

	return atof(szFloatVal);
}


//////////////////////////////////////////////////////////////////////////
// 下列为赋值的重载方法

BOOL CXmlNode::SetString( _bstr_t lpszValue )
{
	_bstr_t strValue(lpszValue);
	return _SetValue(strValue);
}


BOOL CXmlNode::SetBOOL( BOOL bValue )
{
	_bstr_t strValue;
	strValue = bValue ? "true" : "false";

	return _SetValue(strValue);
}

BOOL CXmlNode::SetInt( INT nValue )
{
	_bstr_t strValue;
	char szIntVal[50];

	sprintf(szIntVal, "%ld", nValue);
	strValue = szIntVal;

	return _SetValue(strValue);
}

BOOL CXmlNode::SetFloat( FLOAT fValue )
{
	_bstr_t strValue;
	char szFloatVal[50];

	sprintf(szFloatVal, "%f", fValue);
	strValue = szFloatVal;

	return _SetValue(strValue);
}

CXml* CXmlNode::GetOwnerDoc()
{
	MSXML::IXMLDOMDocumentPtr pDoc = NULL;
	HRESULT hr = m_pNode->get_ownerDocument(&pDoc);
	if( !SUCCEEDED(hr) )
	{
		return new CXml();
	}

	return new CXml(pDoc);
}
//////////////////////////////////////////////////////////////////////////

