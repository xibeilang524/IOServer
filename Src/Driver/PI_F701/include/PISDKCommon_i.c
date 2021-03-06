

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Mon Jun 06 09:53:51 2005
 */
/* Compiler settings for .\PISDKCommon.idl:
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

MIDL_DEFINE_GUID(IID, IID_INameResolver,0x289040A1,0x599F,0x11d2,0x81,0x18,0x00,0xA0,0x24,0x0B,0x7F,0xDC);


MIDL_DEFINE_GUID(IID, IID_IRefresh,0xB8AEB9C1,0x5227,0x11d2,0xBE,0xFA,0x00,0x60,0xB0,0x29,0x01,0x78);


MIDL_DEFINE_GUID(IID, IID_ISlimFast,0xAE619C61,0x1793,0x11d2,0x80,0xEE,0x00,0xA0,0x24,0x0B,0x7F,0xDC);


MIDL_DEFINE_GUID(IID, IID_ITraverse,0xDB1D5986,0x8A3F,0x11d2,0xBD,0x40,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(IID, IID__EnumNamedValue,0x8A5AC971,0x5D60,0x11d1,0x80,0x8B,0x00,0xA0,0x24,0x0B,0x7F,0xDC);


MIDL_DEFINE_GUID(IID, IID__NamedValues,0x010F0970,0x5A49,0x11D1,0x80,0x88,0x00,0xA0,0x24,0x0B,0x7F,0xDC);


MIDL_DEFINE_GUID(IID, IID_INamedValues2,0xB5546F22,0xEF21,0x11d3,0xBD,0xC7,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(IID, IID_NamedValue,0x444DBB11,0x5A50,0x11d1,0x80,0x88,0x00,0xA0,0x24,0x0B,0x7F,0xDC);


MIDL_DEFINE_GUID(IID, IID_INamedValue2,0xB5546F21,0xEF21,0x11d3,0xBD,0xC7,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(IID, IID__PIErrors,0x21CC3DE6,0xEF1F,0x11D3,0xBD,0xC7,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(IID, IID__PIError,0x21CC3DE8,0xEF1F,0x11D3,0xBD,0xC7,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(IID, IID__PIAsynchStatus,0x36FC9461,0x5334,0x11d2,0xBE,0xFA,0x00,0x60,0xB0,0x29,0x01,0x78);


MIDL_DEFINE_GUID(IID, IID_IPIAsynchStatus2,0xECA80073,0xB652,0x478a,0xB1,0x91,0xE8,0x7A,0x92,0x4E,0xB7,0x23);


MIDL_DEFINE_GUID(IID, IID__PIGlobalRestorer,0x908DDF80,0xEA77,0x11D3,0xBD,0xC5,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(IID, LIBID_PISDKCommon,0x76A44786,0xEBC8,0x11D3,0xBD,0xC5,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(CLSID, CLSID_NamedValues,0x010F0971,0x5A49,0x11D1,0x80,0x88,0x00,0xA0,0x24,0x0B,0x7F,0xDC);


MIDL_DEFINE_GUID(CLSID, CLSID_NamedValue2,0xA6CCC641,0x5F6C,0x11d4,0xBD,0xF9,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(CLSID, CLSID_PIErrors,0x21CC3DE7,0xEF1F,0x11D3,0xBD,0xC7,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(CLSID, CLSID_PIError,0x21CC3DE9,0xEF1F,0x11D3,0xBD,0xC7,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(IID, DIID__PIAsynchStatusEvents,0x36FC9462,0x5334,0x11d2,0xBE,0xFA,0x00,0x60,0xB0,0x29,0x01,0x78);


MIDL_DEFINE_GUID(CLSID, CLSID_PIAsynchStatus,0x36FC9463,0x5334,0x11d2,0xBE,0xFA,0x00,0x60,0xB0,0x29,0x01,0x78);


MIDL_DEFINE_GUID(IID, IID_IPIReset,0xF6A05E85,0xDBA5,0x441d,0x97,0x88,0xCE,0x8E,0x75,0xEC,0x36,0x93);


MIDL_DEFINE_GUID(CLSID, CLSID_PIGlobalRestorer,0x908DDF81,0xEA77,0x11D3,0xBD,0xC5,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(IID, IID_IPIRestorer,0xBA451DA2,0xEA7F,0x11d3,0xBD,0xC5,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(IID, IID_IPIPersist,0xBA451DA1,0xEA7F,0x11d3,0xBD,0xC5,0x00,0xC0,0x4F,0x77,0x9E,0xB2);


MIDL_DEFINE_GUID(IID, IID_IPIServers,0xAE71F984,0x4B6F,0x4FF4,0xA4,0x29,0x2E,0xBD,0x6E,0x2F,0x6C,0x99);


MIDL_DEFINE_GUID(IID, IID_IRefresh2,0x1C0B5B31,0xE883,0x48e2,0xB6,0x2F,0xF8,0xDA,0x1D,0x9E,0xD2,0x01);


MIDL_DEFINE_GUID(IID, IID_IPITransaction,0x8CEC04CE,0xF919,0x4da3,0xBC,0x37,0x68,0x58,0x97,0x76,0x25,0x75);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

