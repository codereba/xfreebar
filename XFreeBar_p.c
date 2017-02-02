

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "XFreeBar.h"

#define TYPE_FORMAT_STRING_SIZE   21                                
#define PROC_FORMAT_STRING_SIZE   37                                
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _XFreeBar_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } XFreeBar_MIDL_TYPE_FORMAT_STRING;

typedef struct _XFreeBar_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } XFreeBar_MIDL_PROC_FORMAT_STRING;

typedef struct _XFreeBar_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } XFreeBar_MIDL_EXPR_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const XFreeBar_MIDL_TYPE_FORMAT_STRING XFreeBar__MIDL_TypeFormatString;
extern const XFreeBar_MIDL_PROC_FORMAT_STRING XFreeBar__MIDL_ProcFormatString;
extern const XFreeBar_MIDL_EXPR_FORMAT_STRING XFreeBar__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IToolBarHost_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IToolBarHost_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IContextItem_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IContextItem_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IHtmlEventHandler_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IHtmlEventHandler_ProxyInfo;



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const XFreeBar_MIDL_PROC_FORMAT_STRING XFreeBar__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure Run */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pDispatch */

/* 24 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 30 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const XFreeBar_MIDL_TYPE_FORMAT_STRING XFreeBar__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  4 */	NdrFcLong( 0x20400 ),	/* 132096 */
/*  8 */	NdrFcShort( 0x0 ),	/* 0 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 14 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 16 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 18 */	0x0,		/* 0 */
			0x46,		/* 70 */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IToolBarHost, ver. 0.0,
   GUID={0x11C03B6A,0x8DEF,0x4CE8,{0x9D,0x43,0x9B,0x67,0xAA,0xBE,0x2B,0x94}} */

#pragma code_seg(".orpc")
static const unsigned short IToolBarHost_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IToolBarHost_ProxyInfo =
    {
    &Object_StubDesc,
    XFreeBar__MIDL_ProcFormatString.Format,
    &IToolBarHost_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IToolBarHost_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    XFreeBar__MIDL_ProcFormatString.Format,
    &IToolBarHost_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IToolBarHostProxyVtbl = 
{
    0,
    &IID_IToolBarHost,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION IToolBarHost_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IToolBarHostStubVtbl =
{
    &IID_IToolBarHost,
    &IToolBarHost_ServerInfo,
    7,
    &IToolBarHost_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IContextItem, ver. 0.0,
   GUID={0xEAF2CCEE,0x21A1,0x4203,{0x9F,0x36,0x49,0x29,0xFD,0x10,0x4D,0x43}} */

#pragma code_seg(".orpc")
static const unsigned short IContextItem_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IContextItem_ProxyInfo =
    {
    &Object_StubDesc,
    XFreeBar__MIDL_ProcFormatString.Format,
    &IContextItem_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IContextItem_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    XFreeBar__MIDL_ProcFormatString.Format,
    &IContextItem_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IContextItemProxyVtbl = 
{
    &IContextItem_ProxyInfo,
    &IID_IContextItem,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IContextItem::Run */
};


static const PRPC_STUB_FUNCTION IContextItem_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2
};

CInterfaceStubVtbl _IContextItemStubVtbl =
{
    &IID_IContextItem,
    &IContextItem_ServerInfo,
    8,
    &IContextItem_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IHtmlEventHandler, ver. 0.0,
   GUID={0xF001AFE6,0x49D4,0x4C4B,{0x88,0x09,0x6B,0xFF,0xFE,0x79,0x2A,0x1B}} */

#pragma code_seg(".orpc")
static const unsigned short IHtmlEventHandler_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IHtmlEventHandler_ProxyInfo =
    {
    &Object_StubDesc,
    XFreeBar__MIDL_ProcFormatString.Format,
    &IHtmlEventHandler_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IHtmlEventHandler_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    XFreeBar__MIDL_ProcFormatString.Format,
    &IHtmlEventHandler_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IHtmlEventHandlerProxyVtbl = 
{
    0,
    &IID_IHtmlEventHandler,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION IHtmlEventHandler_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IHtmlEventHandlerStubVtbl =
{
    &IID_IHtmlEventHandler,
    &IHtmlEventHandler_ServerInfo,
    7,
    &IHtmlEventHandler_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    XFreeBar__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x70001f4, /* MIDL Version 7.0.500 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * _XFreeBar_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IToolBarHostProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IHtmlEventHandlerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IContextItemProxyVtbl,
    0
};

const CInterfaceStubVtbl * _XFreeBar_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IToolBarHostStubVtbl,
    ( CInterfaceStubVtbl *) &_IHtmlEventHandlerStubVtbl,
    ( CInterfaceStubVtbl *) &_IContextItemStubVtbl,
    0
};

PCInterfaceName const _XFreeBar_InterfaceNamesList[] = 
{
    "IToolBarHost",
    "IHtmlEventHandler",
    "IContextItem",
    0
};

const IID *  _XFreeBar_BaseIIDList[] = 
{
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    0
};


#define _XFreeBar_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _XFreeBar, pIID, n)

int __stdcall _XFreeBar_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _XFreeBar, 3, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _XFreeBar, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _XFreeBar, 3, *pIndex )
    
}

const ExtendedProxyFileInfo XFreeBar_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _XFreeBar_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _XFreeBar_StubVtblList,
    (const PCInterfaceName * ) & _XFreeBar_InterfaceNamesList,
    (const IID ** ) & _XFreeBar_BaseIIDList,
    & _XFreeBar_IID_Lookup, 
    3,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

