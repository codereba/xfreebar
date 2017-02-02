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

#ifndef __XML_PARSER_H__
#define __XML_PARSER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>
#include <comdef.h>

#include <string>
#include <vector>
using std::wstring;
using std::vector;

void throwException(HRESULT hr,_bstr_t str,const TCHAR* file,INT32 line);
_bstr_t IntToStr(INT32 nVal, INT32 type);

namespace  XML
{

template<class T>
class vectorDel : public vector<T>
{
  public:
    ~vectorDel() 
    {
      clear();
    }
    void clear()
    {
      for(iterator i=begin();i!=end();i++) 
        delete *i;
      vector<T>::clear();
    }
};

class XmlParser
{
public:
  XmlParser();
  virtual ~XmlParser() {}

  void                        load(MSXML::IXMLDOMNodeListPtr doc);
  MSXML::IXMLDOMDocumentPtr   build();

  virtual void                parse(const _bstr_t& inputStr ,bool isFile = true);

  void                        save (      _bstr_t& outputStr,bool isFile = true);

  // operations

  void preserveWhiteSpace(bool p);
  void validateOnParse(bool p);
  void resolveExternals(bool p);

protected:

  // load
  virtual void startNode(_bstr_t& str) = 0;
  virtual void endNode() = 0;

  virtual void processAttribute(_bstr_t& bstrName,_bstr_t& strValue) = 0;

  VARIANT_BOOL doparseFile(const _bstr_t&  fileName);
  VARIANT_BOOL doparseStr (const _bstr_t&  inputStr);

  // save
  virtual void do_save() = 0;
  // read
  virtual void on_parse() = 0;
  virtual void on_end_parse() = 0;

	//
	// XML support code.
	//
	HRESULT ReportError(MSXML::IXMLDOMParseErrorPtr pXMLError);
	bool IterateChildNodes(MSXML::IXMLDOMNodePtr pNode);
	bool IterateAttibutes(MSXML::IXMLDOMNodePtr pNode);

  HRESULT createDocument();

  MSXML::IXMLDOMDocumentPtr m_pIXMLDOMDocument;
  VARIANT_BOOL              m_preserveWhiteSpace;
  VARIANT_BOOL              m_resolveExternals;
  VARIANT_BOOL              m_validateOnParse;
};
} // end namespace XML

#endif // __XML_PARSER_H__
