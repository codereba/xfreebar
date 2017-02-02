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
#include ".\xml.h"

CXml::CXml(void)
: m_strFilePath(_T(""))
, m_pDoc(NULL)
{
}

CXml::CXml(IXMLDOMDocumentPtr pDoc)
: m_strFilePath(_T(""))
{
	m_pDoc = pDoc;
}

CXml::~CXml(void)
{
	Close();
}


//-------------------------------------------------------------------------
// Function Name    :IsFileExist	[static]
// Parameter(s)     :_bstr_t strFilePath	�ļ�·��������
// Return           :BOOL
// Memo             :�ж��ļ��Ƿ����
//-------------------------------------------------------------------------
BOOL CXml::IsFileExist(_bstr_t strFilePath)
{
	BOOL bExist = FALSE;
	HANDLE hFile = NULL;

	hFile = CreateFile( strFilePath
		, GENERIC_READ
		, FILE_SHARE_READ | FILE_SHARE_WRITE
		, NULL
		, OPEN_EXISTING
		, 0
		, 0
		);

	if( hFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle( hFile );
		bExist = TRUE;
	}

	return (bExist);
}


//-------------------------------------------------------------------------
// Function Name    :Open
// Parameter(s)     :_bstr_t strXmlFilePath	�ļ�·��������
// Return           :BOOL	�Ƿ�ɹ�
// Memo             :��XML�ļ� ����������򴴽�֮
//-------------------------------------------------------------------------
BOOL CXml::Open( _bstr_t strXmlFilePath )
{
	HRESULT hr = S_OK;

	hr = m_pDoc.CreateInstance( __uuidof(MSXML::DOMDocument) );
	if( !SUCCEEDED(hr) )
	{
		// ��û��CoInitialize������
		ATLASSERT(FALSE);
		return FALSE;
	}

	ATLASSERT( m_pDoc != NULL );
	m_pDoc->preserveWhiteSpace = VARIANT_FALSE;
	m_pDoc->validateOnParse = VARIANT_FALSE;
	m_strFilePath = strXmlFilePath;

	if( IsFileExist(strXmlFilePath) )
	{
		VARIANT_BOOL vbSuccessful = VARIANT_FALSE;

		try
		{			
			vbSuccessful = m_pDoc->load( variant_t(strXmlFilePath) );
		}
		catch(...)
		{
			vbSuccessful = VARIANT_FALSE;
		}

		return  vbSuccessful == VARIANT_TRUE;
	}
	else
	{
		MSXML::IXMLDOMProcessingInstructionPtr pPI = NULL;
		pPI = m_pDoc->createProcessingInstruction( _bstr_t(_T("xml")), _bstr_t(_T("version=\"1.0\"")) );
		if( pPI == NULL )
		{
			return FALSE;
		}

		hr = m_pDoc->appendChild(pPI);
		if( !SUCCEEDED(hr) )
		{
			return FALSE;
		}

		return SUCCEEDED(hr);
	}
}


//-------------------------------------------------------------------------
// Function Name    :Close
// Parameter(s)     :void
// Return           :void
// Memo             :�ر�
//-------------------------------------------------------------------------
void CXml::Close(void)
{
	m_pDoc = NULL;
}



//-------------------------------------------------------------------------
// Function Name    :GetLastError
// Parameter(s)     :void
// Return           :_bstr_t
// Memo             :��ȡ���ĳ�����Ϣ
//-------------------------------------------------------------------------
_bstr_t CXml::GetLastError(void)
{
	ATLASSERT( m_pDoc != NULL );

	HRESULT hr = S_OK;
	MSXML::IXMLDOMParseErrorPtr pIParseError = NULL;

	hr = m_pDoc->get_parseError(&pIParseError);
	if( !SUCCEEDED(hr) )
	{
		return bstr_t("");
	}

	_bstr_t strRet;
	if( pIParseError != NULL )
	{
		LONG value;
		hr = pIParseError->get_errorCode(&value);
		if( !SUCCEEDED(hr) )
		{
			return bstr_t("");
		}


		hr = pIParseError->get_reason(&strRet.GetBSTR());
		if( !SUCCEEDED(hr) )
		{
			return bstr_t("");
		}

		SAFE_RELEASE(pIParseError);		
	}

	return strRet;
}



//-------------------------------------------------------------------------
// Function Name    :GetRoot
// Parameter(s)     :
// Return           :
// Memo             :��ȡ���ڵ�
//-------------------------------------------------------------------------
CXmlNodePtr CXml::GetRoot(bstr_t strDefName)
{	
	ATLASSERT( m_pDoc != NULL );

	MSXML::IXMLDOMElementPtr pElement = NULL;
	HRESULT hr = m_pDoc->get_documentElement(&pElement);
	if( !SUCCEEDED(hr) )
	{
		return CXmlNodePtr(new CXmlNode());
	}

	CXmlNodePtr pNode( new CXmlNode(pElement) );

	SAFE_RELEASE(pElement);
	
	return pNode;
}

CXmlNodePtr CXml::AppendRoot(bstr_t strDefName)
{	
	ATLASSERT( m_pDoc != NULL );

	MSXML::IXMLDOMElementPtr pElement = NULL;

	pElement = m_pDoc->createElement(strDefName);
	if( pElement == NULL )
	{
		return CXmlNodePtr(new CXmlNode());
	}

	MSXML::IXMLDOMNodePtr pElementNode =  m_pDoc->appendChild(pElement);

	if(pElementNode == NULL)
	{
		return CXmlNodePtr(new CXmlNode());
	}
	CXmlNodePtr pNode( new CXmlNode(pElement) );

	SAFE_RELEASE(pElement);
	
	return pNode;
}

//-------------------------------------------------------------------------
// Function Name    :Save
// Parameter(s)     :_bstr_t lpszFilePath	[in] �����λ��
// Return           :BOOL
// Memo             :����Xml�ļ�
//-------------------------------------------------------------------------
BOOL CXml::Save(_bstr_t lpszFilePath /* = NULL */)
{
	ATLASSERT( m_pDoc != NULL );

	HRESULT hr = S_OK;
	if( lpszFilePath.length() == 0 )
		hr = m_pDoc->save( _variant_t( m_strFilePath) );
	else
		hr = m_pDoc->save( _variant_t( lpszFilePath ) );

	return SUCCEEDED(hr);
}



//-------------------------------------------------------------------------
// Function Name    :SelectSingleNode
// Parameter(s)     :_bstr_t strPath  ·����
// Return           :CXmlNodePtr
// Memo             :ȡ�����ڵ�
//-------------------------------------------------------------------------
CXmlNodePtr CXml::SelectSingleNode(_bstr_t lpszPath)
{
	ATLASSERT( m_pDoc != NULL );

	MSXML::IXMLDOMNodePtr pItem = NULL;
	pItem = m_pDoc->selectSingleNode( lpszPath );

	CXmlNodePtr pNode ( new CXmlNode(pItem) );
	SAFE_RELEASE(pItem);
	
	return pNode;
}



//-------------------------------------------------------------------------
// Function Name    :SelectNodes
// Parameter(s)     :_bstr_t strPath	·����
// Return           :CXmlNodesPtr
// Memo             :ȡ��㼯��
//-------------------------------------------------------------------------
CXmlNodesPtr CXml::SelectNodes(_bstr_t lpszPath)
{
	ATLASSERT( m_pDoc != NULL );

	MSXML::IXMLDOMNodeListPtr pNodeList = NULL;
	pNodeList =	m_pDoc->selectNodes( lpszPath );

	CXmlNodesPtr pNodes ( new CXmlNodes(pNodeList) );
	SAFE_RELEASE(pNodeList);

	return pNodes;
}
