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
#include "XmlParser.h"
#include "CmnUtil.h"

/////////////////////////////////////////////////////////////////////////////
// XmlParser 

using XML::XmlParser;

XmlParser::XmlParser() : 
                           m_preserveWhiteSpace(VARIANT_FALSE),
                           m_validateOnParse(VARIANT_TRUE),
                           m_resolveExternals(VARIANT_FALSE) 
{
}

VARIANT_BOOL  XmlParser::doparseFile(const _bstr_t& strFileName)
{
  VARIANT_BOOL vBool = m_pIXMLDOMDocument->load(_variant_t(strFileName));
  return vBool;
}
VARIANT_BOOL  XmlParser::doparseStr(const _bstr_t& inputStr)
{
	VARIANT_BOOL vBool = m_pIXMLDOMDocument->loadXML(inputStr);
  return vBool;
}

void XmlParser::load(MSXML::IXMLDOMNodeListPtr list) 
{
  on_parse();
  createDocument();
  static _bstr_t documentID = "#document - document";

  for(INT32 i = 0; i<list->Getlength(); i++)
  {
      MSXML::IXMLDOMElementPtr pNode = list->Getitem(i);
      startNode(documentID);
    	IterateChildNodes(pNode);
      endNode();
		  //m_pIXMLDOMDocument->appendChild(p);
  }
  on_end_parse();
  m_pIXMLDOMDocument = 0;
}

void XmlParser::parse(const _bstr_t& strFileName, bool isFile) 
{
	if (SUCCEEDED(createDocument()))
	{
    on_parse();
		if ( SUCCEEDED(m_pIXMLDOMDocument->put_async(VARIANT_FALSE)) &&
        SUCCEEDED(m_pIXMLDOMDocument->put_preserveWhiteSpace(m_preserveWhiteSpace)) &&
        SUCCEEDED(m_pIXMLDOMDocument->put_validateOnParse(m_validateOnParse)) &&
        SUCCEEDED(m_pIXMLDOMDocument->put_resolveExternals(m_resolveExternals)) 
        )
		{
      VARIANT_BOOL vBool = isFile ? doparseFile(strFileName) : doparseStr(strFileName);
      if( vBool == VARIANT_TRUE )
			{
				MSXML::IXMLDOMNodePtr pNode  = m_pIXMLDOMDocument;
				if (pNode)
				{
					IterateChildNodes(pNode);
				}
			}
      else
      {
        MSXML::IXMLDOMParseErrorPtr pIParseError = 
                                  m_pIXMLDOMDocument->GetparseError();
        ReportError(pIParseError);
      }
		}
    on_end_parse();
    m_pIXMLDOMDocument = NULL;
	}
  else
  {
    throwException(E_FAIL,_T("Can't create DOMDocument"),_T(__FILE__),__LINE__);
  }
}

#ifdef _XML_SAVE
void XmlParser::save(_bstr_t& outputStr,bool isFile)
{
  build();
  if(isFile)
    m_pIXMLDOMDocument->save(_variant_t(outputStr));
  else
    outputStr = m_pIXMLDOMDocument->Getxml();
  m_pIXMLDOMDocument = 0;
}

MSXML::IXMLDOMDocumentPtr XmlParser::build()
{
  if (SUCCEEDED(createDocument()))
	{
    do_save();
  }
  return m_pIXMLDOMDocument;
}
#endif

///////////////////////////////////////////////////////////////////////////////
// ReportError
//
// Standardised reporting mechanism for COM errors to do with XML.
//
HRESULT XmlParser::ReportError(MSXML::IXMLDOMParseErrorPtr pXMLError)
{
	long lLine;
	long lLinePos;
	long lErrorCode;

	//
	// Whilst these could all return errors, we'll assume that since the
	// COM was awake enough to report an error (and return the structure)
	// that it can work with these calls.
	//
	pXMLError->get_line(&lLine);
	pXMLError->get_linepos(&lLinePos);
	pXMLError->get_errorCode(&lErrorCode);
	_bstr_t bstrReason = pXMLError->Getreason();
	_bstr_t bstrFile = pXMLError->Geturl();

	//
	// Format this message into a suitable output.
	//
	if ( lLine >= 0 )
	{
    wstring strError;

    strError += "Error processing XML file: " + bstrFile + "\n";
    strError += "Error on line " + IntToStr(lLine) + ", position " + IntToStr(lLinePos) + "\n";
    strError += "Description: " + bstrReason;

    throwException(E_FAIL,strError.c_str(),_T(__FILE__),__LINE__);
  }

	return NOERROR;
}

///////////////////////////////////////////////////////////////////////////////
// WalkTopLevel
//
bool XmlParser::IterateChildNodes(MSXML::IXMLDOMNodePtr pNode)
{
	BSTR bstrNodeName;

	_bstr_t strOutput;
	pNode->get_nodeName(&bstrNodeName);

	//
	// Find out the node type (as a string).
	//
	_bstr_t strType = pNode->GetnodeTypeString();

	MSXML::DOMNodeType eEnum;
	pNode->get_nodeType(&eEnum);

	INT32 nImg = 0;

	if ( eEnum == MSXML::NODE_TEXT )
	{
		strOutput = pNode->Gettext();
		nImg = 3;
	}
	else if ( eEnum == MSXML::NODE_CDATA_SECTION)
	{
		strOutput = pNode->Gettext();
		nImg = 4;
	}
	else if ( eEnum == MSXML::NODE_COMMENT )
	{
		VARIANT vValue;
		pNode->get_nodeValue(&vValue);

		if ( vValue.vt == VT_BSTR )
			strOutput = V_BSTR(&vValue);
		else
			strOutput = "Unknown comment type";
		VariantClear(&vValue);

		nImg = 8;
	}
	else if ( eEnum == MSXML::NODE_PROCESSING_INSTRUCTION )
	{
		strOutput = bstrNodeName;
		nImg = 4;
	}
	else if ( eEnum == MSXML::NODE_ELEMENT )
	{
		strOutput = bstrNodeName;
		nImg = 7;
	}
	else
	{
		// Other types, include the type name.
		//
		strOutput = bstrNodeName + _bstr_t(" - ") + strType;
	}
	SysFreeString(bstrNodeName);

	startNode(strOutput);

	IterateAttibutes(pNode);

	//
	// Any child nodes of this node need displaying too.
	//
	MSXML::IXMLDOMNodePtr pChild = pNode->GetfirstChild();
	while( pChild )
	{
		IterateChildNodes(pChild);
		pChild = pChild->GetnextSibling();
	}

	endNode();

	return true;
}					


///////////////////////////////////////////////////////////////////////////////
// IterateAttibutes
//
bool XmlParser::IterateAttibutes(MSXML::IXMLDOMNodePtr pNode)
{
	MSXML::IXMLDOMNamedNodeMapPtr pAttrs = pNode->Getattributes();

	if(pAttrs)
	{
		MSXML::IXMLDOMNodePtr pChild = pAttrs->nextNode();
		while(pChild)
		{
			BSTR bstrName;
			VARIANT vValue;

			pChild->get_nodeName(&bstrName);
			pChild->get_nodeValue(&vValue);

			_bstr_t strValue;
			switch ( vValue.vt )
			{
			case VT_BSTR:
				strValue = V_BSTR(&vValue);
				break;
			default:
				strValue = _T("Unsupport type");
				break;
			}

			_bstr_t strAttrib = bstrName + _bstr_t("=") + strValue;
			SysFreeString(bstrName);
			VariantClear(&vValue);

			pChild = pAttrs->nextNode();

			processAttribute(_bstr_t(bstrName),strValue);
		}
	}
	return true;
}

void XmlParser::preserveWhiteSpace(bool p)
{
	m_preserveWhiteSpace = p ? VARIANT_TRUE : VARIANT_FALSE;
}

void XmlParser::validateOnParse(bool p)
{
  m_validateOnParse = p ? VARIANT_TRUE : VARIANT_FALSE;
}

void XmlParser::resolveExternals(bool p)
{
  m_resolveExternals = p ? VARIANT_TRUE : VARIANT_FALSE;
}

HRESULT XmlParser::createDocument()
{
  return m_pIXMLDOMDocument.CreateInstance(MSXML::CLSID_DOMDocument);
}
