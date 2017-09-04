// **************************************************************************
// opcda.cpp
//
// Description:
//	Implements OPC data access.  
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


/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Oct 14 12:14:58 1998
 */
/* Compiler settings for opcda.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


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

const IID IID_IOPCServer = {0x39c13a4d,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCServerPublicGroups = {0x39c13a4e,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCBrowseServerAddressSpace = {0x39c13a4f,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCGroupStateMgt = {0x39c13a50,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCPublicGroupStateMgt = {0x39c13a51,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCSyncIO = {0x39c13a52,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCAsyncIO = {0x39c13a53,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCItemMgt = {0x39c13a54,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IEnumOPCItemAttributes = {0x39c13a55,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCDataCallback = {0x39c13a70,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCAsyncIO2 = {0x39c13a71,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCItemProperties = {0x39c13a72,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID LIBID_OPCDA = {0xB28EEDB2,0xAC6F,0x11d1,{0x84,0xD5,0x00,0x60,0x8C,0xB8,0xA7,0xE9}};


#ifdef __cplusplus
}
#endif

