// **************************************************************************
// opccomn.h
//
// Description:
//	Defines OPC common data.  
//
//	File supplied by OPC Foundataion.
//
// DISCLAIMER:
//	This programming example is provided "AS IS".  As such Kepware, Inc.
//	makes no claims to the worthiness of the code and does not warranty
//	the code to be error free.  It is provided freely and can be used in
//	your own projects.  If you do find this code useful, place a little
//	marketing plug for Kepware in your code.  While we would love to help
//	every one who is trying to write a great OPC client application, the 
//	uniqueness of every project and the limited number of hours in a day 
//	simply prevents us from doing so.  If you really find yourself in a
//	bind, please contact Kepware's technical support.  We will not be able
//	to assist you with server related problems unless you are using KepServer
//	or KepServerEx.
// **************************************************************************

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Fri Jun 19 14:14:38 1998
 */
/* Compiler settings for opccomn.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __opccomn_h__
#define __opccomn_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IOPCShutdown_FWD_DEFINED__
#define __IOPCShutdown_FWD_DEFINED__
typedef interface IOPCShutdown IOPCShutdown;
#endif 	/* __IOPCShutdown_FWD_DEFINED__ */


#ifndef __IOPCCommon_FWD_DEFINED__
#define __IOPCCommon_FWD_DEFINED__
typedef interface IOPCCommon IOPCCommon;
#endif 	/* __IOPCCommon_FWD_DEFINED__ */


#ifndef __IOPCCommon_FWD_DEFINED__
#define __IOPCCommon_FWD_DEFINED__
typedef interface IOPCCommon IOPCCommon;
#endif 	/* __IOPCCommon_FWD_DEFINED__ */


#ifndef __IOPCShutdown_FWD_DEFINED__
#define __IOPCShutdown_FWD_DEFINED__
typedef interface IOPCShutdown IOPCShutdown;
#endif 	/* __IOPCShutdown_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IOPCShutdown_INTERFACE_DEFINED__
#define __IOPCShutdown_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IOPCShutdown
 * at Fri Jun 19 14:14:38 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IOPCShutdown;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("F31DFDE1-07B6-11d2-B2D8-0060083BA1FB")
    IOPCShutdown : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ShutdownRequest( 
            /* [string][in] */ LPCWSTR szReason) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOPCShutdownVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOPCShutdown __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOPCShutdown __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOPCShutdown __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShutdownRequest )( 
            IOPCShutdown __RPC_FAR * This,
            /* [string][in] */ LPCWSTR szReason);
        
        END_INTERFACE
    } IOPCShutdownVtbl;

    interface IOPCShutdown
    {
        CONST_VTBL struct IOPCShutdownVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOPCShutdown_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOPCShutdown_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOPCShutdown_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOPCShutdown_ShutdownRequest(This,szReason)	\
    (This)->lpVtbl -> ShutdownRequest(This,szReason)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOPCShutdown_ShutdownRequest_Proxy( 
    IOPCShutdown __RPC_FAR * This,
    /* [string][in] */ LPCWSTR szReason);


void __RPC_STUB IOPCShutdown_ShutdownRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOPCShutdown_INTERFACE_DEFINED__ */


#ifndef __IOPCCommon_INTERFACE_DEFINED__
#define __IOPCCommon_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IOPCCommon
 * at Fri Jun 19 14:14:38 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IOPCCommon;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("F31DFDE2-07B6-11d2-B2D8-0060083BA1FB")
    IOPCCommon : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetLocaleID( 
            /* [in] */ LCID dwLcid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocaleID( 
            /* [out] */ LCID __RPC_FAR *pdwLcid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryAvailableLocaleIDs( 
            /* [out] */ DWORD __RPC_FAR *pdwCount,
            /* [size_is][size_is][out] */ LCID __RPC_FAR *__RPC_FAR *pdwLcid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetErrorString( 
            /* [in] */ HRESULT dwError,
            /* [string][out] */ LPWSTR __RPC_FAR *ppString) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetClientName( 
            /* [string][in] */ LPCWSTR szName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOPCCommonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOPCCommon __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOPCCommon __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOPCCommon __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetLocaleID )( 
            IOPCCommon __RPC_FAR * This,
            /* [in] */ LCID dwLcid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLocaleID )( 
            IOPCCommon __RPC_FAR * This,
            /* [out] */ LCID __RPC_FAR *pdwLcid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryAvailableLocaleIDs )( 
            IOPCCommon __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwCount,
            /* [size_is][size_is][out] */ LCID __RPC_FAR *__RPC_FAR *pdwLcid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrorString )( 
            IOPCCommon __RPC_FAR * This,
            /* [in] */ HRESULT dwError,
            /* [string][out] */ LPWSTR __RPC_FAR *ppString);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetClientName )( 
            IOPCCommon __RPC_FAR * This,
            /* [string][in] */ LPCWSTR szName);
        
        END_INTERFACE
    } IOPCCommonVtbl;

    interface IOPCCommon
    {
        CONST_VTBL struct IOPCCommonVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOPCCommon_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOPCCommon_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOPCCommon_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOPCCommon_SetLocaleID(This,dwLcid)	\
    (This)->lpVtbl -> SetLocaleID(This,dwLcid)

#define IOPCCommon_GetLocaleID(This,pdwLcid)	\
    (This)->lpVtbl -> GetLocaleID(This,pdwLcid)

#define IOPCCommon_QueryAvailableLocaleIDs(This,pdwCount,pdwLcid)	\
    (This)->lpVtbl -> QueryAvailableLocaleIDs(This,pdwCount,pdwLcid)

#define IOPCCommon_GetErrorString(This,dwError,ppString)	\
    (This)->lpVtbl -> GetErrorString(This,dwError,ppString)

#define IOPCCommon_SetClientName(This,szName)	\
    (This)->lpVtbl -> SetClientName(This,szName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOPCCommon_SetLocaleID_Proxy( 
    IOPCCommon __RPC_FAR * This,
    /* [in] */ LCID dwLcid);


void __RPC_STUB IOPCCommon_SetLocaleID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOPCCommon_GetLocaleID_Proxy( 
    IOPCCommon __RPC_FAR * This,
    /* [out] */ LCID __RPC_FAR *pdwLcid);


void __RPC_STUB IOPCCommon_GetLocaleID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOPCCommon_QueryAvailableLocaleIDs_Proxy( 
    IOPCCommon __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwCount,
    /* [size_is][size_is][out] */ LCID __RPC_FAR *__RPC_FAR *pdwLcid);


void __RPC_STUB IOPCCommon_QueryAvailableLocaleIDs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOPCCommon_GetErrorString_Proxy( 
    IOPCCommon __RPC_FAR * This,
    /* [in] */ HRESULT dwError,
    /* [string][out] */ LPWSTR __RPC_FAR *ppString);


void __RPC_STUB IOPCCommon_GetErrorString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOPCCommon_SetClientName_Proxy( 
    IOPCCommon __RPC_FAR * This,
    /* [string][in] */ LPCWSTR szName);


void __RPC_STUB IOPCCommon_SetClientName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOPCCommon_INTERFACE_DEFINED__ */



#ifndef __OPCCOMN_LIBRARY_DEFINED__
#define __OPCCOMN_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: OPCCOMN
 * at Fri Jun 19 14:14:38 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 





EXTERN_C const IID LIBID_OPCCOMN;
#endif /* __OPCCOMN_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
