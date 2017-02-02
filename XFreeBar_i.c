

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IToolBarHost,0x11C03B6A,0x8DEF,0x4CE8,0x9D,0x43,0x9B,0x67,0xAA,0xBE,0x2B,0x94);


MIDL_DEFINE_GUID(IID, IID_IContextItem,0xEAF2CCEE,0x21A1,0x4203,0x9F,0x36,0x49,0x29,0xFD,0x10,0x4D,0x43);


MIDL_DEFINE_GUID(IID, IID_IHtmlEventHandler,0xF001AFE6,0x49D4,0x4C4B,0x88,0x09,0x6B,0xFF,0xFE,0x79,0x2A,0x1B);


MIDL_DEFINE_GUID(IID, LIBID_XFreeBarLib,0xB1486773,0x3E27,0x4A64,0xB3,0xEF,0x86,0xAB,0xB3,0x41,0x5F,0xE6);


MIDL_DEFINE_GUID(CLSID, CLSID_XToolBarHost,0xD8B50602,0x86BE,0x4887,0xB5,0xD5,0x01,0x7C,0x59,0xD3,0xEB,0x14);


MIDL_DEFINE_GUID(CLSID, CLSID_HtmlEventHandler,0xC4043D9E,0x842C,0x4EBD,0x85,0x75,0xA7,0xDD,0xE3,0x61,0x4E,0x01);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



