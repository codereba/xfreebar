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

#include <memory>


#define SAFE_RELEASE(x)	\
	if( x != NULL )		\
	{					\
		x.Release();	\
		x = NULL;		\
	}


class CXml;

class CXmlNode;
typedef std::auto_ptr<CXmlNode>  CXmlNodePtr;

class CXmlNodes;
typedef std::auto_ptr<CXmlNodes>  CXmlNodesPtr;



#include ".\XmlNodes.h"



class CXmlNode
{
	friend class CXml;
	friend class CXmlNode;
	friend class CXmlNodes;


public:	
	~CXmlNode(void);

protected:
	
	MSXML::IXMLDOMNodePtr   m_pNode;

	CXmlNode( MSXML::IXMLDOMNodePtr pNode);	
	BOOL _GetValue(_bstr_t & strValue);
	BOOL _SetValue(_bstr_t & strValue);

public:

	//		_________________
	//______|	构造函数	|___________________________________________________________________
	CXmlNode(void);
	CXmlNode( CXmlNode & refNode IN);


	//		_________________
	//______|	重载 = 号	|___________________________________________________________________
	CXmlNodePtr operator = ( CXmlNodePtr pNode);
	CXmlNode & operator = ( CXmlNode & refNode);



	//		_________________
	//______|  获取结点名称	|___________________________________________________________________
	BOOL GetName(_bstr_t &strName IN OUT);
	BOOL CheckName(bstr_t strChecked);
	


	//		_________________
	//______|	属性操作	|___________________________________________________________________
	_bstr_t GetAttribute( _bstr_t strName, _bstr_t lpszDefault = _T(""));
	BOOL SetAttribute(  _bstr_t strName, _bstr_t strValue);
	BOOL DelAttribute( _bstr_t strName );



	//		_________________
	//______|	子结点操作	|___________________________________________________________________
	CXmlNodePtr GetChild(_bstr_t strName);
	CXmlNodePtr GetChild(_bstr_t strAttribName, _bstr_t strAttribVal IN);
	CXmlNodesPtr GetChildren();
	CXmlNodePtr GetFirstChild();
	CXmlNodePtr GetNextSibling();
	CXmlNodePtr GetEndChild();
	CXmlNodePtr GetPrevSibling();
	CXmlNodePtr AppendChild(bstr_t strName);
	CXmlNodePtr InsertSiblingBefore(bstr_t strName);
	//BOOL InsertChild(UINT uIndex);
	BOOL RemoveChild(bstr_t strName);
	BOOL RemoveChild(CXmlNodePtr pXmlNode);
	BOOL ModifyName(bstr_t strNewName);

	BOOL HasChildren(void);
	INT32 HasChilds();
	BOOL RemoveChildren(void);
	


	//		_________________
	//______|	删除结点	|___________________________________________________________________
	BOOL Remove(void);
	
	CXml* GetOwnerDoc();

	//		_________________
	//______|	取值操作	|___________________________________________________________________	
	_bstr_t GetString( _bstr_t lpszDefault = _T("") );
	BOOL GetBOOL( BOOL bDefault = FALSE );
	INT GetInt( INT nDefault = 0 );
	DOUBLE GetFloat( DOUBLE fDefault = 0.0f );
	


	//		_________________
	//______|	赋值操作	|___________________________________________________________________
	BOOL SetString( _bstr_t lpszValue );
	BOOL SetBOOL( BOOL bValue );
	BOOL SetInt( INT nValue );
	BOOL SetFloat( FLOAT fValue );
	
	BOOL IsNull(void);
};
