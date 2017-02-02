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

#ifndef DHTML_EVENT_H
#define DHTML_EVENT_H

template <class T> class XHtmlEventObject : public IDispatch  
{

typedef void (T::*EVENTFUNCTIONCALLBACK)(DISPID id, VARIANT* pVarResult);

public:
   XHtmlEventObject() { m_cRef = 0; }	
   ~XHtmlEventObject() {}

   HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject)
   {
      *ppvObject = NULL;

      if (IsEqualGUID(riid, IID_IUnknown))
         *ppvObject = reinterpret_cast<void**>(this);

      if (IsEqualGUID(riid, IID_IDispatch))
         *ppvObject = reinterpret_cast<void**>(this);

      if (*ppvObject)
      {
         ((IUnknown*)*ppvObject)->AddRef();
         return S_OK;
      }
      else return E_NOINTERFACE;
   }

   DWORD __stdcall AddRef()
   {
      return InterlockedIncrement(&m_cRef);
   }

   DWORD __stdcall Release()
   {
      if (InterlockedDecrement(&m_cRef) == 0)
      {
         delete this;
         return 0;
      }
      return m_cRef;
   }

   STDMETHOD(GetTypeInfoCount)(UINT32 FAR* pctinfo)
   { return E_NOTIMPL; }

   STDMETHOD(GetTypeInfo)(UINT32 iTInfo, LCID  lcid, ITypeInfo FAR* FAR*  ppTInfo)
   { return E_NOTIMPL; }

   STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT32 cNames, LCID lcid, DISPID FAR* rgDispId)
   { return S_OK; }

   STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid,
      WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult,
      EXCEPINFO * pExcepInfo, UINT * puArgErr)
   {
      if (DISPID_VALUE == dispIdMember)
         (m_pT->*m_pFunc)(m_uID, pVarResult);
      else
      {
         //AtlTrace(_T("Invoke dispid = %d\n"), dispIdMember);
      }
	
      return S_OK;
   }

public:
   static LPDISPATCH CreateHandler(T* pT,
                        EVENTFUNCTIONCALLBACK pFunc, DISPID id)
   {
      XHtmlEventObject<T>* pFO = new XHtmlEventObject<T>;
      pFO->m_pT = pT;
      pFO->m_pFunc = pFunc;
      pFO->m_uID = id;
      return reinterpret_cast<LPDISPATCH>(pFO);
   }

protected:
   T* m_pT;
   EVENTFUNCTIONCALLBACK m_pFunc;
   DISPID m_uID;
   long m_cRef;
};

#endif DHTML_EVENT_H