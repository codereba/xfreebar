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
#include <mshtmdid.h>
#include "htmleventhandler.h"

STDMETHODIMP CHtmlEventHandler::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
                                         DISPPARAMS* pDispParams, VARIANT* pvarResult,
                                         EXCEPINFO*  pExcepInfo,  UINT* puArgErr)
{
	if (!pDispParams)
		return E_INVALIDARG;
	
	if (!pvarResult)
		return E_POINTER;

	switch (dispidMember)
	{
	case DISPID_HTMLDOCUMENTEVENTS2_ONCLICK:
		ATLTRACE(_T("HTMLDocumentEvents::onclick fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONMOUSEDOWN:
		ATLTRACE(_T("HTMLDocumentEvents::onmousedown fired\n"));
		{
			CComPtr<IDispatch> spDisp;
			spDisp = pDispParams->rgvarg[0].pdispVal;
			if (spDisp)
			{
				CComQIPtr<IHTMLEventObj, &IID_IHTMLEventObj> spEventObj(spDisp);
				if (spEventObj)
				{
					long nMouseBtn = 0;
					HRESULT hr = spEventObj->get_button(&nMouseBtn);
					if (SUCCEEDED(hr) && nMouseBtn == 0)
					{
						ATLTRACE(_T("HTMLDocumentEvents::onmousedown fired -- simulated by script\n"));
						*m_pbUserClickedLink = FALSE;
						break;
					}
					
					CComPtr<IHTMLElement> spElem;
					hr = spEventObj->get_srcElement(&spElem);
					if (SUCCEEDED(hr) && spElem)
					{
						CComBSTR bsTagName;

						while (1)
						{
							spElem->get_tagName(&bsTagName);
							if(bsTagName.Length() > 0)
							{

								bsTagName.ToUpper();

								if (bsTagName == L"BODY")
									break;	// did not click a link

								if (bsTagName == L"A" ||
									bsTagName == L"AREA" ||
									bsTagName == L"INPUT" ||
									bsTagName == L"IMG")
								{
									ATLTRACE(_T("HTMLDocumentEvents::onclick fired -- user clicked a link\n"));
									*m_pbUserClickedLink = TRUE;
									break;
								}
							}
							CComPtr<IHTMLElement> spParentElem;
							hr = spElem->get_parentElement(&spParentElem);
							if (FAILED(hr) || !spParentElem)
								break;
							spElem = spParentElem;
						}
					}
				}
			}
		}
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONPROPERTYCHANGE:
		ATLTRACE(_T("HTMLDocumentEvents::onpropertychange fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONREADYSTATECHANGE:
		ATLTRACE(_T("HTMLDocumentEvents::onreadystatechange fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONBEFOREUPDATE:
		ATLTRACE(_T("HTMLDocumentEvents::onbeforeupdate fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONAFTERUPDATE:
		ATLTRACE(_T("HTMLDocumentEvents::onafterupdate fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONROWEXIT:
		ATLTRACE(_T("HTMLDocumentEvents::onrowexit fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONROWENTER:
		ATLTRACE(_T("HTMLDocumentEvents::onrowenter fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONDRAGSTART:
		ATLTRACE(_T("HTMLDocumentEvents::ondragstart fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONSELECTSTART:
		ATLTRACE(_T("HTMLDocumentEvents::onselectstart fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONERRORUPDATE:
		ATLTRACE(_T("HTMLDocumentEvents::onerrorupdate fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONCONTEXTMENU:
		ATLTRACE(_T("HTMLDocumentEvents::oncontextmenu fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONSTOP:
		ATLTRACE(_T("HTMLDocumentEvents::onstop fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONROWSDELETE:
		ATLTRACE(_T("HTMLDocumentEvents::onrowsdelete fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONROWSINSERTED:
		ATLTRACE(_T("HTMLDocumentEvents::onrowsinserted fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONCELLCHANGE:
		ATLTRACE(_T("HTMLDocumentEvents::oncellchange fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONDATASETCHANGED:
		ATLTRACE(_T("HTMLDocumentEvents::ondatasetchanged fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONDATAAVAILABLE:
		ATLTRACE(_T("HTMLDocumentEvents::ondataavailable fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONDATASETCOMPLETE:
		ATLTRACE(_T("HTMLDocumentEvents::ondatasetcomplete fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONBEFOREEDITFOCUS:
		ATLTRACE(_T("HTMLDocumentEvents::onbeforeeditfocus fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONSELECTIONCHANGE:
		ATLTRACE(_T("HTMLDocumentEvents::onselectionchange fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONCONTROLSELECT:
		ATLTRACE(_T("HTMLDocumentEvents::oncontrolselect fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONFOCUSIN:
		ATLTRACE(_T("HTMLDocumentEvents::onfocusin fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONFOCUSOUT:
		ATLTRACE(_T("HTMLDocumentEvents::onfocusout fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONACTIVATE:
		ATLTRACE(_T("HTMLDocumentEvents::onactivate fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONDEACTIVATE:
		ATLTRACE(_T("HTMLDocumentEvents::ondeactivate fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONBEFOREACTIVATE:
		ATLTRACE(_T("HTMLDocumentEvents::onbeforeactivate fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONBEFOREDEACTIVATE:
		ATLTRACE(_T("HTMLDocumentEvents::onbeforedeactivate fired\n"));
		return DISP_E_MEMBERNOTFOUND;

	case DISPID_HTMLDOCUMENTEVENTS2_ONDBLCLICK:
	case DISPID_HTMLDOCUMENTEVENTS2_ONKEYDOWN:
	case DISPID_HTMLDOCUMENTEVENTS2_ONKEYUP:
	case DISPID_HTMLDOCUMENTEVENTS2_ONKEYPRESS:
	case DISPID_HTMLDOCUMENTEVENTS2_ONHELP:
	case DISPID_HTMLDOCUMENTEVENTS2_ONMOUSEMOVE:
	case DISPID_HTMLDOCUMENTEVENTS2_ONMOUSEUP:
	case DISPID_HTMLDOCUMENTEVENTS2_ONMOUSEOUT:
	case DISPID_HTMLDOCUMENTEVENTS2_ONMOUSEOVER:
	case DISPID_HTMLDOCUMENTEVENTS2_ONMOUSEWHEEL:
		return DISP_E_MEMBERNOTFOUND;

	default:
		ATLTRACE(_T("HTMLDocumentEvents2 event %ld (0x%x) fired\n"), dispidMember, dispidMember);
		return DISP_E_MEMBERNOTFOUND;
	}
	return S_OK;
}
