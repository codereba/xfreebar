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
#include "XHtmlSearch.h"

XHtmlSearch::XHtmlSearch(void)
{
}

XHtmlSearch::~XHtmlSearch(void)
{
}

void XHtmlSearch::SearchTextInHTML(_bstr_t strSearchText, long lFlags, BOOL bNNF /* =FALSE  (for internal use)*/)
{
	if(IHTMLDocument2 *lpHtmlDocument = GetHtmlDocument())
	{
		SearchTextInHTML(lpHtmlDocument,strSearchText,lFlags,bNNF);

#ifdef _REVERSE_SEARCH
		XString s = strSearchText;
		std::reverse(s.begin(),s.end());
		SearchTextInHTML( lpHtmlDocument, s.c_str(), lFlags, bNNF);
#endif
	}
}

void XHtmlSearch::SearchTextInHTML(IHTMLDocument2 *lpHtmlDocument,_bstr_t strSearchText, long lFlags, BOOL bNNF)
{
	if(!lpHtmlDocument)
		return;

	static _bstr_t sLastSearch;
	static BSTR    lastBookmark = 0;

	if(sLastSearch != strSearchText)
		lastBookmark = NULL;
	sLastSearch = strSearchText;

	IHTMLElement *lpBodyElm;
	IHTMLBodyElement *lpBody;
	IHTMLTxtRange *lpTxtRange;

	lpHtmlDocument->get_body(&lpBodyElm);
	if(!lpBodyElm)
		return;

	lpBodyElm->QueryInterface(IID_IHTMLBodyElement,(void**)&lpBody);
	lpBodyElm->Release();

	IHTMLFramesCollection2 *frames = 0;
	lpHtmlDocument->get_frames(&frames);

	if(frames)
	{
		long p;
		frames->get_length(&p);
		for(long f=0;f<p;f++)
		{
			variant_t frame;
			frames->item(&_variant_t(f),&frame);
			if((IDispatch*)frame)
			{
				IHTMLWindow2  *elem = 0;
				((IDispatch*)frame)->QueryInterface(IID_IHTMLWindow2, (void**)&elem);
				if(elem)
				{
					IHTMLDocument2*  lpHtmlDocument = NULL;
					elem->get_document(&lpHtmlDocument);
					SearchTextInHTML(lpHtmlDocument,strSearchText,lFlags,bNNF);
				}
			}
		}
	}

	if(lpBody)
	{
		lpBody->createTextRange(&lpTxtRange);
		lpBody->Release();

		CComBSTR search( (LPCTSTR) strSearchText);
		VARIANT_BOOL bFound,bTest;
		long t;

		if(lastBookmark!=NULL)
		{
			lpTxtRange->moveToBookmark(lastBookmark,(VARIANT_BOOL*)&bTest);

			if(!bTest)
			{
				lastBookmark=NULL;
				lpTxtRange->moveStart((BSTR)CComBSTR("Textedit"),1,&t);
				lpTxtRange->moveEnd((BSTR)CComBSTR("Textedit"),1,&t);
			} 
			else
			{
				lpTxtRange->moveStart((BSTR)CComBSTR("Character"),1,&t);
				lpTxtRange->moveEnd((BSTR)CComBSTR("Textedit"),1,&t);
			}
		} 
		else
		{
			lpTxtRange->moveStart((BSTR)CComBSTR("Textedit"),0,&t);
			lpTxtRange->moveEnd((BSTR)CComBSTR("Textedit"),1,&t);
		}
		lpTxtRange->findText((BSTR)search,0,lFlags,(VARIANT_BOOL*)&bFound);

		if(!bFound)
		{
			if(lastBookmark==NULL && !bNNF)
			{
			} 
			else if(lastBookmark!=NULL)
			{
				lastBookmark = NULL;
				SearchTextInHTML(strSearchText,lFlags,TRUE);
			}
		} 
		else
		{
			if(lpTxtRange->getBookmark(&lastBookmark)!=S_OK)
				lastBookmark=NULL;

			VARIANT_BOOL pfRet;
			HRESULT h0= lpTxtRange->queryCommandEnabled(_bstr_t("Bold"),& pfRet);
			if(h0==0 && pfRet==VARIANT_TRUE)
			{
				HRESULT h1 = lpTxtRange->select();
				HRESULT h2 = lpTxtRange->scrollIntoView(TRUE);
			}
		}

		lpTxtRange->Release();
	}
}

void XHtmlSearch::SearchAndTipTextInHTML(_bstr_t strSearchText, long lFlags , _bstr_t strMatchStyle, _bstr_t strSearchId )
{
	SearchAndTipTextInHTML( GetHtmlDocument(),strSearchText,lFlags,strMatchStyle,strSearchId);
}

void XHtmlSearch::SearchAndTipTextInHTML(IHTMLDocument2 *lpHtmlDocument,_bstr_t strSearchText, long lFlags /* =2 */, _bstr_t strMatchStyle, _bstr_t strSearchId )
{
	if( NULL == lpHtmlDocument)
		return;

	IHTMLElement *lpBodyElm;
	IHTMLBodyElement *lpBody;
	IHTMLTxtRange *lpTxtRange;

	lpHtmlDocument->get_body(&lpBodyElm);
	if(!lpBodyElm)
		return;

	lpBodyElm->QueryInterface(IID_IHTMLBodyElement,(void**)&lpBody);
	lpBodyElm->Release();

	if( lpBody )
	{
		lpBody->createTextRange(&lpTxtRange);
		lpBody->Release();

		CComBSTR html;
		CComBSTR newhtml;
		CComBSTR search((LPCTSTR)strSearchText);

		long t;
		VARIANT_BOOL bFound;
		HRESULT h;
		while(lpTxtRange->findText(search,0,lFlags,&bFound)==S_OK && bFound==VARIANT_TRUE)
		{
			newhtml.Empty();
			h = lpTxtRange->get_htmlText(&html);
			newhtml.Append("<span id='");
			newhtml.Append((LPCTSTR)strSearchId);
			newhtml.Append("' style='");
			newhtml.Append((LPCTSTR)strMatchStyle);
			newhtml.Append("'>");
			if(strSearchText==_bstr_t(" "))
				newhtml.Append("&nbsp;"); // doesn't work very well, but prevents (some) shit
			else 
				newhtml.AppendBSTR(html);
			newhtml.Append("</span>");

			VARIANT_BOOL pfRet;
			HRESULT h0= lpTxtRange->queryCommandEnabled(_bstr_t("Bold"),& pfRet);
			if(h0==0 && pfRet==VARIANT_TRUE)
			{
				h = lpTxtRange->pasteHTML(newhtml);
			}
			h = lpTxtRange->moveStart((BSTR)_bstr_t("Character"),1,&t);
			h = lpTxtRange->moveEnd((BSTR)CComBSTR("TextEdit"),1,&t);
		}

		lpTxtRange->Release();
	}
	IHTMLFramesCollection2 *frames = 0;
	lpHtmlDocument->get_frames(&frames);

	if(frames)
	{
		long p;
		frames->get_length(&p);
		for(long f=0;f<p;f++)
		{
			variant_t frame;
			frames->item(&_variant_t(f),&frame);
			if((IDispatch*)frame)
			{
				IHTMLWindow2  *elem = 0;
				((IDispatch*)frame)->QueryInterface(IID_IHTMLWindow2, (void**)&elem);
				if(elem)
				{
					IHTMLDocument2*  lpHtmlDocument = NULL;
					HRESULT ff = elem->get_document(&lpHtmlDocument);
					SearchAndTipTextInHTML(lpHtmlDocument,strSearchText,lFlags,strMatchStyle,strSearchId);
				}
			}
		}
	}
}


void XHtmlSearch::ClearSearchResults( _bstr_t strSearchId )
{
	ClearSearchResults( GetHtmlDocument(), strSearchId );
}

void XHtmlSearch::ClearSearchResults( IHTMLDocument2 *lpHtmlDocument,_bstr_t strSearchId )
{
	if(!lpHtmlDocument)
		return;

	CComBSTR testid(strSearchId.length() + 1,(const char*)strSearchId);
	CComBSTR testtag(5,"SPAN");

	IHTMLElementCollection *lpAllElements;
	lpHtmlDocument->get_all(&lpAllElements);

	IHTMLBodyElement *lpBody;
	IHTMLElement *lpBodyElm;
	lpHtmlDocument->get_body(&lpBodyElm);
	if(!lpBodyElm)
		return;

	lpBodyElm->QueryInterface(IID_IHTMLBodyElement,(void**)&lpBody);
	lpBodyElm->Release();

	if(lpBody)
	{
		IUnknown *lpUnk;
		IEnumVARIANT *lpNewEnum;
		if (SUCCEEDED(lpAllElements->get__newEnum(&lpUnk)) && lpUnk != NULL)
		{
			lpUnk->QueryInterface(IID_IEnumVARIANT,(void**)&lpNewEnum);
			VARIANT varElement;
			IHTMLElement *lpElement;


			while (lpNewEnum->Next(1, &varElement, NULL) == S_OK)
			{
				_ASSERTE(varElement.vt == VT_DISPATCH);
				varElement.pdispVal->QueryInterface(IID_IHTMLElement,(void**)&lpElement);
				if (lpElement)
				{
					CComBSTR id;
					CComBSTR tag;
					lpElement->get_id(&id);
					lpElement->get_tagName(&tag);
					if((id==testid)&&(tag==testtag))
					{
						BSTR innerText;
						lpElement->get_innerHTML(&innerText);
						lpElement->put_outerHTML(innerText);
					}
				}
				VariantClear(&varElement);
			}
		}
	}
	{
		IHTMLFramesCollection2 *frames = 0;
		lpHtmlDocument->get_frames(&frames);

		if(frames)
		{
			long p;
			frames->get_length(&p);
			for(long f=0;f<p;f++)
			{
				variant_t frame;
				frames->item(&_variant_t(f),&frame);
				if((IDispatch*)frame)
				{
					IHTMLWindow2  *elem = 0;
					((IDispatch*)frame)->QueryInterface(IID_IHTMLWindow2, (void**)&elem);
					if(elem)
					{
						IHTMLDocument2*  lpHtmlDocument = NULL;
						elem->get_document(&lpHtmlDocument);
						ClearSearchResults(lpHtmlDocument,strSearchId);
					}
				}
			}
		}
	}
}

