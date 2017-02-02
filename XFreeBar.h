

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Thu Feb 02 13:49:50 2017
 */
/* Compiler settings for .\XFreeBar.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __XFreeBar_h__
#define __XFreeBar_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IToolBarHost_FWD_DEFINED__
#define __IToolBarHost_FWD_DEFINED__
typedef interface IToolBarHost IToolBarHost;
#endif 	/* __IToolBarHost_FWD_DEFINED__ */


#ifndef __IContextItem_FWD_DEFINED__
#define __IContextItem_FWD_DEFINED__
typedef interface IContextItem IContextItem;
#endif 	/* __IContextItem_FWD_DEFINED__ */


#ifndef __IHtmlEventHandler_FWD_DEFINED__
#define __IHtmlEventHandler_FWD_DEFINED__
typedef interface IHtmlEventHandler IHtmlEventHandler;
#endif 	/* __IHtmlEventHandler_FWD_DEFINED__ */


#ifndef __XToolBarHost_FWD_DEFINED__
#define __XToolBarHost_FWD_DEFINED__

#ifdef __cplusplus
typedef class XToolBarHost XToolBarHost;
#else
typedef struct XToolBarHost XToolBarHost;
#endif /* __cplusplus */

#endif 	/* __XToolBarHost_FWD_DEFINED__ */


#ifndef __HtmlEventHandler_FWD_DEFINED__
#define __HtmlEventHandler_FWD_DEFINED__

#ifdef __cplusplus
typedef class HtmlEventHandler HtmlEventHandler;
#else
typedef struct HtmlEventHandler HtmlEventHandler;
#endif /* __cplusplus */

#endif 	/* __HtmlEventHandler_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IToolBarHost_INTERFACE_DEFINED__
#define __IToolBarHost_INTERFACE_DEFINED__

/* interface IToolBarHost */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IToolBarHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("11C03B6A-8DEF-4CE8-9D43-9B67AABE2B94")
    IToolBarHost : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IToolBarHostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IToolBarHost * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IToolBarHost * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IToolBarHost * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IToolBarHost * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IToolBarHost * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IToolBarHost * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IToolBarHost * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IToolBarHostVtbl;

    interface IToolBarHost
    {
        CONST_VTBL struct IToolBarHostVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IToolBarHost_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IToolBarHost_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IToolBarHost_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IToolBarHost_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IToolBarHost_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IToolBarHost_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IToolBarHost_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IToolBarHost_INTERFACE_DEFINED__ */


#ifndef __IContextItem_INTERFACE_DEFINED__
#define __IContextItem_INTERFACE_DEFINED__

/* interface IContextItem */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IContextItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EAF2CCEE-21A1-4203-9F36-4929FD104D43")
    IContextItem : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Run( 
            /* [in] */ LPDISPATCH pDispatch) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IContextItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IContextItem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IContextItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IContextItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IContextItem * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IContextItem * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IContextItem * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IContextItem * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Run )( 
            IContextItem * This,
            /* [in] */ LPDISPATCH pDispatch);
        
        END_INTERFACE
    } IContextItemVtbl;

    interface IContextItem
    {
        CONST_VTBL struct IContextItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IContextItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IContextItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IContextItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IContextItem_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IContextItem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IContextItem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IContextItem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IContextItem_Run(This,pDispatch)	\
    ( (This)->lpVtbl -> Run(This,pDispatch) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IContextItem_INTERFACE_DEFINED__ */


#ifndef __IHtmlEventHandler_INTERFACE_DEFINED__
#define __IHtmlEventHandler_INTERFACE_DEFINED__

/* interface IHtmlEventHandler */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IHtmlEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F001AFE6-49D4-4C4B-8809-6BFFFE792A1B")
    IHtmlEventHandler : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IHtmlEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHtmlEventHandler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHtmlEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHtmlEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHtmlEventHandler * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHtmlEventHandler * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHtmlEventHandler * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHtmlEventHandler * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IHtmlEventHandlerVtbl;

    interface IHtmlEventHandler
    {
        CONST_VTBL struct IHtmlEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHtmlEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHtmlEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHtmlEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHtmlEventHandler_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IHtmlEventHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IHtmlEventHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IHtmlEventHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHtmlEventHandler_INTERFACE_DEFINED__ */



#ifndef __XFreeBarLib_LIBRARY_DEFINED__
#define __XFreeBarLib_LIBRARY_DEFINED__

/* library XFreeBarLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_XFreeBarLib;

EXTERN_C const CLSID CLSID_XToolBarHost;

#ifdef __cplusplus

class DECLSPEC_UUID("D8B50602-86BE-4887-B5D5-017C59D3EB14")
XToolBarHost;
#endif

EXTERN_C const CLSID CLSID_HtmlEventHandler;

#ifdef __cplusplus

class DECLSPEC_UUID("C4043D9E-842C-4EBD-8575-A7DDE3614E01")
HtmlEventHandler;
#endif
#endif /* __XFreeBarLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


