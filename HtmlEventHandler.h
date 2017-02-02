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
#include "resource.h"       // Ö÷·ûºÅ

#include "XFreeBar.h"


// CHtmlEventHandler

class ATL_NO_VTABLE CHtmlEventHandler : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHtmlEventHandler, &CLSID_HtmlEventHandler>,
	public IDispatchImpl<IHtmlEventHandler, &IID_IHtmlEventHandler, &LIBID_XFreeBarLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_HTMLEVENTHANDLER)

	BEGIN_COM_MAP(CHtmlEventHandler)
		COM_INTERFACE_ENTRY(IHtmlEventHandler)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public:
	CHtmlEventHandler() :
	  m_pbUserClickedLink(NULL)
	  {
	  }


	  DECLARE_PROTECT_FINAL_CONSTRUCT()

	  HRESULT FinalConstruct()
	  {
		  return S_OK;
	  }

	  void FinalRelease() 
	  {
	  }

public:

	//
	// IDispatch Methods
	//
	STDMETHOD(Invoke)(DISPID dispidMember,REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS * pdispparams, VARIANT * pvarResult,
		EXCEPINFO * pexcepinfo, UINT * puArgErr);

public:
	bool*	m_pbUserClickedLink;
};

OBJECT_ENTRY_AUTO(__uuidof(HtmlEventHandler), CHtmlEventHandler)
