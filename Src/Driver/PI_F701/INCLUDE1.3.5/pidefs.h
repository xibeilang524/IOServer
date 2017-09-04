/*-----------------------------------------------------------------------------

$Archive: /pi/freeze/intel/piapi/pidefs.h $
  
-------------------------------------------------------------------------------
    
Copyright (c) 1994-2002 OSI Software, Inc. All rights reserved. 
Unpublished - rights reserved under the copyright law of the United States.
      
USE OF A COPYRIGHT NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION 
OR DISCLOSURE.
        
THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE SECRETS OF 
OSI SOFTWARE, INC.  USE, DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT 
THE PRIOR EXPRESS WRITTEN PERMISSION OF OSI SOFTWARE, INC.
          
RESTRICTED RIGHTS LEGEND
Use, duplication, or disclosure by the Government is subject to restrictions 
as set forth in subparagraph (c)(1)(ii) of the Rights in Technical Data and 
Computer Software clause at DFARS 252.227.7013
            
OSI Software, Inc.
777 Davis Street, Suite 250, San Leandro CA 94577

-------------------------------------------------------------------------------

Description:
    General PI System data type definitions.

-------------------------------------------------------------------------------

History:

$Log: /pi/freeze/intel/piapi/pidefs.h $

27    2/17/07 11:18a Jeggleston

48    10/19/06 11:08a Atang

47    10/19/06 10:24a Atang
MAXINT and MININT was incorrectly defined in Linux

46    10/13/06 8:08a Dcoda
Cleaned up UNIX defines (added #include of limits.h) to better
incorporate LONG_BIT and WORD_BIT OS defintions to determine sizes of
various types (LONG, DWORD, etc.)

45    10/11/06 8:55 Charlie
Make C++ -> C comments.

44    10/08/06 1:52p Atang
Added Linux64 bit support

43    10/08/06 11:13a Atang
Added Linux support

42    06-02-03 12:41 Charlie
Modified for UNIX compile of PIAPI - remove warnings and conditionally
remove dependencies on pthreads.

41    8/23/05 11:34a Cmuraski
For AIX, use u_int32 instead of unsigned long to define PIDatPtr and
PIFuncPtr, consistent with AIX definition for uint32

40    8/16/05 1:05p Ssortland
Added the PIFuncPtr and PIDataPtr types on 32 and 64 bit Windows

39    7/18/05 11:11a Cmuraski
Added Windows-compatible typedef for BYTE for all UNIX platforms

38    7/12/05 12:44p Matzen
ARCHIVE_COMPONENTNAME_ID is no longer used

37    5/24/05 1:26p Ssortland
Replaced // style comments

36    5/05/05 7:53a Cmuraski
UNIX threads; for AIX only, define TRUE to match the AIX definition

35    4/21/05 12:06p Matzen
Punchlist 2523OSI8 - Start adding support for non-vss backups via the
backup subsystem.

34    4/13/05 12:15p Ssortland
Began to reorganize the build process on the X64 and IA64 platforms:

Removed some of the Makefile projects and began taking advantage of
Visual Studio's ability to use different compilers with the /useenv
startup parameter. Now IA64 and X64 are just another configuration in
the pre-existing projects.

Created 3 batch files for doing X86, X64 and IA64 development: 

devX86.bat
devX64.bat
devIA64.bat

note: These are only necessary if you're only going to be building on
64 bit Windows.

Modified pidefs.h, so that if you attempt to build (for example) the
X64 platform after running devIA64.bat you get a compilation error.

33    4/13/05 10:54a Matzen
Punchlist 2523OSI8 - The PIRESTOREDIR environment variable now sets the
directory for VSS restores for 3rd party backups. 

32    3/24/05 1:55p Jon
64bit port.

31    3/24/05 11:13a Jwong
removed WIN64_EXCULSIVE def

30    3/02/05 2:54p Ssortland
Socket handles and thread handles are now first rank types (typdefed
differently on each platform.) Was necessary for the 64 bit platforms,
where sockets and thread handles are defined to be 64 bit values (a
pointer.)

socket handles go from type int to SOCKET
thread handles go from type int to PIthread_handle

29    2/18/05 6:34p Dvacher
PLI 7763OSI8: Use Win32 API for PIfilebase and piut_fileXXX access
functions

28    2/01/05 5:06p Rulik
Added archive expand mode for arccollect calls

27    1/27/05 8:52p Rulik
Added TableAction enum
for ptsrc and DbSecurity table interfaces

26    12/03/04 1:19p Matzen
Punchlist 2523OSI8. Move the definition of the Base_Snap_PrimaryArchive
from pidefs to pisubsys.hxx.

25    12/03/04 9:48a Matzen
Punchlist 2523OSI8 - Remove backup reigistry. Work in progress.

24    10/18/04 2:32p Cmuraski
Added definition of _ASSERTE() for UNIX

23    9/27/04 7:24a Matzen
Punchlist 2523OSI8.  Move fstat definitions from pifile.cxx to
pidefs.hxx.  Create new piut functions piut_getmodificationtime,
piut_fileclose.  Move function prototypes for piut functions from
pifile.hxx to pifileutil.hxx.

22    9/07/04 8:42a Cmuraski
UNIX threads; add definitions for MAXINT64, MININT64, and MAXUINT64 for
osf1 only

21    8/24/04 12:13p Cmuraski
UNIX threads; for Tru64 (osf1), don't redefine FALSE and TRUE

20    6/14/04 8:07a Cmuraski
UNIX threads port; add definition of WINAPI for UNIX

19    6/02/04 2:47p Jon
PLI 5885OSI8: IPV6 support. Note: Winsock2.h must be included before
windows.h to prevent multiply defined symbols. Some files were modified
only for that requirement.

18    6/02/04 2:18p Cmuraski
UNIX Threads port; the following apply to UNIX platforms only; added
Windows-compatible typedefs for BOOL, LONG, LPLONG, LPCSTR, LPCTSTR;
added Windows-compatible definitions for _TEXT(), FALSE, TRUE, WAIT_*
event  return codes, MAX_PATH, EVENT_ALL_ACCESS

17    5/27/04 7:30a Cmuraski
UNIX threads port; modified defintion of INT_PTR for UNIX in pidefs.h
to avoid multiple definition error from piut_dump(INT_PTR&, ...) in
piobject.hxx

16    5/08/04 6:46p Jon
PLI 5407OSI8: Windows 64 bit

15    4/27/04 1:08p Cmuraski
Changed #ifdef label from UNIX to unix for definition of INT_PTR
(again)

14    4/14/04 8:44a Cmuraski
Final merge from 3.4Unix

5     4/09/04 2:41p Cmuraski
Added typedef for SIZE_T for UNIX

4     4/09/04 2:05p Cmuraski
Updated 64-bit type definitions for Solaris; changed ifdef label UNIX
to unix for INT_PTR definition

13    4/05/04 5:07p Ssortland
Added Threads definitions for NT and Unix

3     4/05/04 9:22a Cmuraski
Expose definition of INVALID_THREAD_ID for both Windows and UNIX

12    4/02/04 10:59a Ssortland
Unix only defines lowercase size_t, typedef SIZE_T to size_t

2     4/02/04 10:03a Cmuraski
Change value of INVALID_THREAD_ID from 0 to 0xffffffff to reconcile
with 364 version

11    3/25/04 5:57p Jon
PLI 5407OSI8: Get code to compile with 64 bit compiler.

10    3/24/04 5:06p Jon
PLI 5407OSI8: Changes to support 64 bit Windows.

9     3/24/04 12:59p Cmuraski
UNIX port; added definition of _ASSERT() for UNIX

7     11/22/02 4:04p Jon
Threading work.

6     11/21/02 2:51p Rulik
Added PIFuncPtr and PIDataPtr types

5     11/12/02 2:00p Rulik
Merging from $/PI/3.3/libraries/piut/pidefs.h

8     9/09/02 9:58a Ray
Comment out ENABLESTACKPROTECTION.

7     8/12/02 1:17p Ray
Enable ENABLESTACKPROTECTION.

6     8/12/02 11:26a Jon
Protocol trace work. Stack protection macros.

5     8/09/02 5:48p Jon
Crash debug tools.

4     02-04-22 10:07 Charlie
For WIN32 also need to include time.h for time_t definition in
PIGetCurrenttime.

3     4/21/02 11:12a Jon
Replaced C++ '//' comment with c comment.

2     4/19/02 3:20p Jon
Punchlist 2019OSI8

38    1/17/02 4:36p Jon
Punchlist 1705OSI8 parameterize license limits.

37    11/16/01 1:34p Etam
remove "long long" for Solaris definition; otherwise, compiling with
-Xc fails

36    11/07/01 4:55p Jon
Audit work.

35    11/02/01 4:05p Etam
put #ifndef aix around inline void* operator new (size_t, void *p) to
satisfy new compiler

34    11/01/01 10:11p Rulik
Changed enumeration to 0x40000000 to appease Dec-Unix (Maybe...)

33    10/31/01 9:07a Rulik
Added Enumerations for snap/arch Auditing

32    10/25/01 12:02p Rulik
Reordered DB enumeration by DB type and added DBsecurity

31    10/18/01 11:32p Rulik
PIdb enumeration as powers of 2 - to allow auditing specification for
individual DB

30    10/17/01 6:35p Jon
Audits.

29    9/28/01 1:58p Jon
Unix port. Punchlist 1200OSI8

28    9/14/01 4:35p Jon

27    6/07/01 11:08a Jon
Unix Builds. Punchlist 12000OSI8

26    5/23/01 2:31p Jon
Unix port. Punch list 1200OSI8.

25    5/22/01 2:10p Jon
Unix port. Punchlist 1200OSI8

24    5/21/01 4:43p Jon
Unix port. Punchlist 1200OSI8

23    8/09/00 16:54 Rulik
added summary codes for pctgood by time and values

22    3/06/00 9:24a Ken
Added boundary condition enumeration for ModuleDB values calls

21    2/29/00 2:40p Ken
Added enum support for Module Database Value calls: eventprevious,
eventnext

20    2/04/:0 3:36p Charlie
MAXUINT32 needs UL type, not L. 

19    1/31/00 2:53p Richard
Migration to Standard Library Iostreams.

18    12/10/99 11:40a Richard
Added enum support for Mode 4 piar_value calls.

17    10/07/99 2:10p Richard
Merge from 3.2.357.17.

15    5/19/99 10:44a Richard
Removed Intellution as default build option.

14    3/25/99 12:30p Richard
Added support for Intellution specific requirements.

16    8/23/99 11:14a Piadmin
v15 comment changed to use words, not comment characters. 

15    8/23/99 8:16a Piadmin
Changed header comments from C++ to C style.

14    6/13/99 1:09p Richard
Merge from 3.2.357.9.

15    5/19/99 10:44a Richard
Removed Intellution as default build option.

14    3/25/99 12:30p Richard
Added support for Intellution specific requirements.

12    10/05/98 1:40p Richard
Added pragma to disable warning about new operator and exception
handling.

11    9/21/98 11:48a Bgallardo

10    8/07/98 1:04p Charlie
changed second int64 def to uint64

9     8/07/98 12:18p Charlie
Added check to see if MAXINT32/MAXUINT16 were already defined.

8     7/30/98 9:36a Ray
add definition of typdef "bool" for vms.

   18 Jan 93 BV  Original version.
   25 Jun 93 RAB Added pistatus.
   02 Jul 93 RAB Changed pistatus to an enum and moved into pistatus.h
   26 Jul 93 MDH removed "signed" from int8, removed "long double" -
                                       warnings from H-P C++ compiler
   27 Jul 93 MDH add MIN* and MAX* #define's for 8,16,32 bit typedefs
   10 Sep 93 BV  add global operator new for specific object placement
   14 Sep 93 BV  include <stdlib.h>
   15 Sep 93 BV  bugfix: include <stddef.h> instead of <stdlib.h>
   15 Sep 93 ET  put #ifdef around #include <stddef.h>
   01 Nov 93 MDH moved pistatus.h include after typedef for bool
   08 Aug 94 RP  added PI_Data_Type enumeration
   16 Aug 94 RP  added MAX_LONG_NAME_LEN (from VAX) 
   24 Aug 94 RP  moved type definitions to pitypes.h
   14 Sep 94 jhp removed ET fix of 15 sep 93--not sure why ET did this
   10 Nov 94 RAB modified MININT32 to accomodate HP-UX CC bug.
   06 Dec 94 jhp typedef bool rather than #define, breaks some Microsoft NT
                 header files.
   23 Jan 94 RAB added endian defines.
   22 May 95 JY  added PATHSEPARATOR, DEFDATAPATH, PIPARAMSPATH, PIPARAMSFILE
    5 Oct 95 RP  Added PIsleep macro
   17 Oct 95 RCV Moved PIsleep macros to left margin, changed VAX to vax
   10 Nov 95 HKS Declared PIPROC on WIN32 to include WINAPI for _stdcall
   14 Nov 95 HKS Added PIsleep definition (NULL) for Windows to avoid inclusion
                 of unistd.h
    1 Dec 95 HKS Conditionally include new(size_t,void *) implementation. 
    4 Dec 95 RAB Previous Change Breaks PI 3.0 on NT since we don't use
		 MFC - #defined check is changed to use _MFC_VER as a basis
   18 Mar 96 jhp Replace bool with pibool--compatability problem with some
                 compilers.
   29-Mar-96 RP Added PIcompoper and PIarcoper
   21-Feb-97 jhp Define _BIG_ENDIAN and _LITTLE_ENDIAN on appropriate machines
                 if not already defined. This fixes a piut_passivate ( float64 )
                 and a piut_activate ( float64 ) bug on AIX. The passivates were
                 relying on the built in _BIG_ENDIAN and this is not defined on AIX.

-----------------------------------------------------------------------------*/ 
#ifndef PI__DEFINITIONS
#define PI__DEFINITIONS

#ifdef PI_SHARED_BUILD
#include "piutshr.h"
#else
#define PIUTSHR_API 
#endif

/*----------------------------------------------- includes ------------------*/
#include <stddef.h>          /* for size_t */
#include <limits.h>          /* for int(word), long sizes */
/*----------------------------------------------- typedefs ------------------*/

#ifdef UNICODE
typedef wchar_t TCHAR;                        /* text characters */
#else
typedef char TCHAR;
#ifdef unix
#define _TEXT(s) s
#endif
#endif

#if defined( hppa ) || defined( sparc ) || defined( rs6000 ) || defined(aix) || defined (__HHW_BIG_ENDIAN__)
#define PI_BIGENDIAN
#ifndef _BIG_ENDIAN
#define _BIG_ENDIAN
#endif
#else
#define PI_LITTLEENDIAN 
#ifndef _LITTLE_ENDIAN
#define _LITTLE_ENDIAN
#endif
#endif

#if defined(WIN32) && !defined(WIN64)
#define WIN32_EXCLUSIVE
#endif

#if defined(WIN32) || defined(WIN64)
#define WIN32_OR_WIN64
#include <winsock2.h>
#include <windows.h>
#endif

/*backup subsystem*/
#ifdef WIN32_OR_WIN64
#define PIRESTOREDIR_ENV_VAR "PIRESTOREDIR"
#endif

#ifdef WIN32_EXCLUSIVE

   typedef char             int8;                  /* integers */
   typedef short            int16;
   typedef long             int32;
   typedef __int64          int64;
   
   typedef unsigned char    uint8;                  /* unsigned */
   typedef unsigned short   uint16;
   typedef unsigned long    uint32;
   typedef unsigned __int64 uint64;

   typedef unsigned long    PIFuncPtr;
   typedef unsigned long    PIDataPtr;

#elif defined (WIN64)   

   typedef char             int8;                  /* integers */
   typedef short            int16;
   typedef long             int32;
   typedef __int64          int64;
   
   typedef unsigned char    uint8;                  /* unsigned */
   typedef unsigned short   uint16;
   typedef unsigned long    uint32;
   typedef unsigned __int64 uint64;

   typedef unsigned __int64    PIFuncPtr;
   typedef unsigned __int64    PIDataPtr;

#elif defined (aix)  /* aix defines int8, etc. */

#include <sys/inttypes.h>
   typedef u_int8 uint8;                  /* unsigned */
   typedef u_int16 uint16;
   typedef u_int32 uint32;
   typedef u_int64 uint64;
   typedef unsigned char    BYTE;
   typedef int              BOOL;
   typedef int              LONG;
   typedef int *            LPLONG;
   typedef unsigned int     DWORD;
   typedef u_int32          PIFuncPtr;
   typedef u_int32          PIDataPtr;
   typedef const char *     LPCSTR;
   typedef const char *     LPCTSTR;
   typedef int              SOCKET;

#else  /* Unix */

   typedef char               int8;
   typedef unsigned char      uint8;
   typedef short              int16;
   typedef unsigned short     uint16;
#if (WORD_BIT==32)
   typedef int                int32;
   typedef unsigned int       uint32;
#elif (LONG_BIT==32)
   typedef long               int32;
   typedef unsigned long      uint32;
#endif
#if (LONG_BIT==64)
   typedef long               int64;
   typedef unsigned long      uint64;
#else
   typedef long long          int64;
   typedef unsigned long long uint64;
#endif

   typedef unsigned char      BYTE;
   typedef int                BOOL;
#if (WORD_BIT==32)
   typedef int                LONG;
   typedef int *              LPLONG;
   typedef unsigned int       DWORD;
#elif (LONG_BIT==32)
   typedef long               LONG;
   typedef long *             LPLONG;
   typedef unsigned long      DWORD;
#endif
   typedef unsigned long      PIFuncPtr;
   typedef unsigned long      PIDataPtr;
   typedef const char *       LPCSTR;
   typedef const char *       LPCTSTR;
   typedef int                SOCKET;

#endif

#ifdef unix
	typedef size_t            SIZE_T;
#endif

typedef float          float32;               /* floats */
typedef double         float64;


/* typedef long double    float80; */

/*  18 mar 96 jhp following definition obsoletes bool typedef  */
typedef char pibool;
/* Following is to support MFC Version 4.2 Delivered with Visual C++ 4.2 */
#if !defined(_MFC_VER) || (_MFC_VER < 0x420)
#define true           ((pibool)1)
#define false          ((pibool)0)
#endif

#ifdef unix
#ifndef osf1
#define FALSE 0
#ifdef aix
#define TRUE 1
#else
#define TRUE  (!FALSE)
#endif
#endif

#define WAIT_OBJECT_0  0
#define WAIT_ABANDONED 0x10000000
#define WAIT_FAILED    0x20000000
#define WAIT_TIMEOUT   0x40000000

#ifndef MAX_PATH
#define MAX_PATH ((int)(255))
#endif

#define EVENT_ALL_ACCESS 1

#endif

/* 29-jan-98 lfm    add typedef bool for vms */
#if defined(vms)
typedef char bool;
#endif

/* these definitions are consistent with the adopted ANSI/ISO C++ 
   definition of the new type bool
   */
/* #ifndef __GNUG__ bool is defined in gnu c++ */
/* typedef char bool;   jhp  6dec94  */
/*#define true           ((bool)1)
#define false          ((bool)0)*/
/* following typedef has been replaced by defines above. */
/* typedef int            bool; */                 /* boolean */
/*#endif */ 


/* Changed to _WINDOWS from _DOS below to avoid MFC problem 30aug95 hks */
#if defined (_WINDOWS) && (!defined (WIN32) && !defined(_WIN32))
#include <windows.h>     /*  WINAPI definition  */
#define PIPTR _far
#define PIPROC WINAPI   /* WINAPI = far pascal */
#elif (defined(WIN32) || defined(_WIN32)) 
#include <windows.h>     /*  WINAPI definition  */
#define PIPTR 
#define PIPROC WINAPI   /* WINAPI = _stdcall */ 
#else
#define PIPTR 
#define PIPROC
#ifdef unix
#define WINAPI
#endif
#endif

/*
Pointer size on Windows is defined as INT_PTR (__int64 or int for 32 bit).
Pointer size on UNIX is the same as size of long.
*/
#ifdef unix
   typedef long INT_PTR;
#endif

/* general file descriptor definition */
#ifdef WIN32_OR_WIN64
#define PIFILEHANDLE HANDLE
#define _PISTATSTRUCT _stat
#define _PISTATFUNC ::_stat
#else
#define PIFILEHANDLE int
#define _PICLOSE ::close
#define _PIFSTAT ::fstat
#define _PISTATSTRUCT stat
#define _PILSEEK ::lseek
#define _PIWRITE ::write
#define _PIREAD ::read
#define _PISTATFUNC ::stat
#define _PIUNLINK ::unlink
#endif

#define MININT8 -128
#define MAXINT8 127
#define MININT16 -32768
#define MAXINT16 32767
#define MININT32 (-2147483647L - 1)
#ifndef MAXINT32  /* 98/08/07 */
#define MAXINT32 2147483647L
#endif
#define MAXUINT8 255
#ifndef MAXUINT16  /* 98/08/07 */
#define MAXUINT16 65535
#endif
#ifndef linux
#define MININT64 (-9223372036854775807L - 1)
#define MAXINT64 9223372036854775807L
#define MAXUINT64 18446744073709551615UL
#else
#define MININT64 (-9223372036854775807LL-1)
#define MAXINT64 9223372036854775807LL
#define MAXUINT64 18446744073709551615ULL
#endif /* linux */
#ifdef osf1
#define MAXUINT32 4294967295U
#else
#define MAXUINT32 4294967295UL
#endif

#define MAX_LONG_NAME_LEN (80)

#ifdef WIN32
#define PATHSEPARATOR (TCHAR) '\\'
#define PIPARAMSPATH "\\system\\"
#define PIPARAMSFILE "\\system\\piparams.default"
#define DEFDATAPATH "c:\\PI\\dat\\"
#else
#define PATHSEPARATOR (TCHAR) '/'
#define PIPARAMSPATH "/etc/"
#define PIPARAMSFILE "/etc/piparams.default"
#define DEFDATAPATH "/usr/PI/dat/"
#endif

#define VP_ROOT 0
#define VP_PIADMIN 1

/*----- PIarcmode -------------------------------------------------------------
   Archive Data Access Modes
-----------------------------------------------------------------------------*/
enum PIarcmode 
{
   eventprevious = -3,     /* previous value, not including query date */
   eventinterpbefore = -2, /* equivalent to PI 2.x - Mode 4 */
   eventbefore = -1,       /* equivalent to PI 2.x - Mode 1 */
   eventinterp = 0,        /* equivalent to PI 2.x - Mode 3 */
   eventafter = 1,         /* equivalent to PI 2.x - Mode 2 */
   eventnext = 3           /* next value, not including query date */
};

/*----- PIsummarycode ---------------------------------------------------------
   Archive summary method modes.
-----------------------------------------------------------------------------*/
enum PIsummarycode
{
   PIarctotal,
   PIarcminimum,
   PIarcmaximum,
   PIarcstdev,
   PIarcrange,
   PIarcaverage,
   PIarcmean,
   PIarcPctTimeGood,
   PIarcPctValGood
};
enum PIarcflag
{
   ARCflag_all=0x001,
   ARCflag_numb=0x002,
   ARCflag_time=0x004,
   ARCflag_even=0x008,
   ARCflag_filter=0x010,
   ARCflag_mark=0x020,
   ARCflag_comp=0x040,
   ARCflag_expand=0x080
};

/* archive and general purpose enums */
enum PIcompoper {EQ, NE, GT, GE, LT, LE};
enum PIarcoper {ARC_read, ARC_write, ARC_delete, ARC_create, ARC_shift};
/*
 BoundaryTypeConstants for SDK
*/
enum PIarcBoundaryTypeConstants{
   piarcbtInside     = 0,          /* values inside the start and end times */
   piarcbtOutside    = 1,          /* values outside the start and end times (error if none) */
   piarcbtInterp     = 2,          /* values interpolated at the start and times */
   piarcbtAuto       = 3           /* values inside of step tag, else interpolated */
};


/*  Database enum  */
enum PIDBNAMES {
   PIdb_Null=0, 
   PIdb_Point=1, 
   PIdb_Digital=2, 
   PIdb_Attribset=4, 
   PIdb_Context=8, 
   PIdb_PointClass=0x10,
   PIdb_User=0x20, 
   PIdb_Group=0x40, 
   PIdb_Trust=0x80, 
   PIdb_Module=0x100, 
   PIdb_Heading=0x200, 
   PIdb_TransferRecord=0x400, 
   PIdb_Campaign=0x800, 
   PIdb_Batch=0x1000, 
   PIdb_Unit=0x2000, 
   PIdb_Snapshot=0x10000000,
   PIdb_Archive=0x20000000,
   PIdb_DBsecurity=0x40000000 
};
enum AuditAction {
   AUDITADD, 
   AUDITEDIT, 
   AUDITREMOVE,
   AUDITEDITATTEMPT,
   AUDITREMOVEATTEMPT
};

enum TableAction {
   TABLEDUMMY=0,
   TABLELIST=1, 
   TABLEADD, 
   TABLEEDIT, 
   TABLEREMOVE,
   TABLERENAME,
   TABLEADDOREDIT,
   TABLENOACTION
};
/*----------------------------------------------- macros and inlines ---------*/
#ifdef vms
#define PIsleep(x) {float wait_time = 0.10 * x; lib$wait ( &wait_time );}
#elif defined( WIN32 )
#define PIsleep(x) {Sleep ( x*1000 );}
#elif defined( _WINDOWS )
#define PIsleep(x) 
#else
#include <unistd.h>
#define PIsleep(x) {sleep ( x );}
#endif
/*
 get time_t current utc time. Uses CRT on Unix
 WIN32 calls on NT to avoid inconsistencies with NT
 and the CRT. For exaple, when tz is set, WIN32 CRT calls
 have different behavior. This function also avoids dependency
 on pitime.hxx
*/
#ifdef __cplusplus
#ifdef WIN32
#include <time.h>
inline time_t PIGetCurrentTime( time_t *pretVal )
{ 
   double dTime;
   time_t val;
   SYSTEMTIME st;   
   GetSystemTime( &st ); 
   if ( st.wMonth <= 2 ) 
      dTime = ( (1461L * (st.wYear - 1L)/4L) + (153L * (st.wMonth + 13L) / 5L) + st.wDay ) - 719606L; 
   else 
      dTime = ( (1461L * (int32) st.wYear / 4L) +          (153L * ((int32) st.wMonth +1L) / 5L) + (int32) st.wDay ) - 719606L; 
   dTime = ((dTime * 24 + st.wHour) * 60 + st.wMinute) * 60 + st.wSecond;
   val = (time_t)(dTime);
   if ( pretVal )
      *pretVal = val;
   return val;
} 
#else
#include <time.h>
inline time_t PIGetCurrentTime( time_t *pretVal )
{ 
   time_t val = time(0);
   if ( pretVal ) *pretVal=val; 
   return val;
} 
#endif
#endif      
#ifdef __cplusplus
#if(!defined(_MFC_VER) || (_MFC_VER < 0x400)) 
#ifndef __PLACEMENT_NEW_INLINE
/*----- placement operator new ----------------------------------------
 * This operator new places the object allocated at the passed address.
 * 10 Sep 93 BV  See C++ ARM page 279.
 *---------------------------------------------------------------------*/
#define __PLACEMENT_NEW_INLINE
#ifdef WIN32
#pragma warning(disable:4291)  /* get rid of the Microsoft heave */
#endif

#if !defined(aix) && !defined(linux)   /* otherwise, new compiler complains about declaration conflict */
inline void* operator new (size_t, void *p) {
   return p;
}
#endif 

#endif /* __PLACEMENT_NEW_INLINE */
#endif /* _MFC_VER */
#endif /* __cplusplus */



#ifdef WIN32

// #define ENABLESTACKPROTECTION

#ifdef ENABLESTACKPROTECTION

#define PAGEMASK 0xFFFFF000
#define PAGESIZE 0x00001000

#define PROTECTSTACK() __asm  \
{ \
_asm mov ebx,esp                 /* keep track of the current esp*/ \
_asm and esp,PAGEMASK            /* round down to nearest pageboundary     */ \
_asm sub esp,PAGESIZE            /* move down one more page*/ \
_asm mov [esp],ebx               /* store the old esp (it is in ebx)here   */ \
_asm mov ebx,esp                 /* store this address to protectlater     */ \
_asm sub esp,4                   /* move esp to now point to thenext page  */ \
\
\
_asm push esp                    /* Dummy location for the 4th "out" param  */ \
_asm push PAGE_NOACCESS          /* allow NOACCESS*/ \
_asm push 1                      /* 1 byte (which gets the wholepage)      */ \
_asm push ebx                    /* the stored page to change access*/ \
_asm call dword ptr [VirtualProtect]  /* make the call to change the access */ \
} 


#define UNPROTECTSTACK() _asm \
{ \
/* if all is ok, esp should now be just below the protected page*/ \
_asm mov ebx,esp                 /* start here*/ \
_asm add ebx,4                   /* protected page is 4 bytes up*/ \
\
/* make the function call to VirtualProtect.*/ \
_asm push esp                    /* Dummy location for the 4th "out"param  */ \
_asm push PAGE_READWRITE         /* allow NOACCESS*/ \
_asm push 1                      /* 1 byte (which gets the whole page)      */ \
_asm push ebx                    /* the previous page to change access      */ \
_asm call dword ptr [VirtualProtect]  /* make the call to change theaccess */ \
\
_asm mov esp,[ebx]               /* restore the old esp (storedearlier)    */ \
} 
#else
#define PROTECTSTACK() {}
#define UNPROTECTSTACK() {}
#endif
#else
#define PROTECTSTACK() {}
#define UNPROTECTSTACK() {}
#endif


/* Replacements for Microsoft _ASSERT() and _ASSERTE() on UNIX
*/
#ifdef unix

#ifdef _DEBUG
#define _ASSERT(e) \
   {if (!(e)) \
      cout << "Error: " << __FILE__ << ", " << __LINE__ << endl << flush;}
#else
#define _ASSERT(e)
#endif

#ifdef _DEBUG
#define _ASSERTE(e) \
   {if (!(e)) \
      cout << "Error: " << __FILE__ << ", " << __LINE__ << endl << flush;}
#else
#define _ASSERTE(e)
#endif

#endif  /* unix */


#ifdef unix

/* Subtitute for Microsoft's definition of INVALID_HANDLE_VALUE
*/
#define INVALID_HANDLE_VALUE 0

#endif  /* unix */

/* Defined invalid thread ID
 * for both Windows and UNIX
 */
#define INVALID_THREAD_ID 0xffffffff


#ifdef WIN32_OR_WIN64

#include <windows.h>
typedef HANDLE			    PIthread_handle;
typedef uint32			    PIthread_id;
typedef LPTHREAD_START_ROUTINE	    PIthread_func;
typedef LPVOID			    PIthread_args;

#else

#if ( (defined(aix) && defined(_THREAD_SAFE)) ||\
      (defined(hpux) && defined(_REENTRANT)) ||\
      (defined(solaris) && defined(_REENTRANT)) ||\
      (defined(osf1) && defined(_REENTRANT)) )
#define  PI_THREADSAFE
#include <sys/times.h>
#include <pthread.h>
typedef pthread_t                   PIthread_handle;
typedef uint32			    PIthread_id;
typedef void*(*PIthread_func)(void* args);
typedef void*                       PIthread_args;
#endif  /* _REENTRANT */

#endif  /* WIN32_OR_WIN64 */

#ifdef linux
#include <sys/times.h>
#include <pthread.h>
typedef uint32 PIthread_id;
typedef pthread_t PIthread_handle;
#endif

/* Added compilation check for Windows, error if we're building with the wrong compiler.
*/

#if defined(X64) && !defined(_AMD64_) 
#ifdef _IA64_
#error ERROR: Building X64 in the IA64 environment
#else
#error ERROR: Building X64 in the X86 environment
#endif
#endif
#if defined(IA64) && !defined(_IA64_)
#ifdef _AMD64_
#error ERROR: Building IA64 in the X64 environment
#else
#error ERROR: Building IA64 in the X86 environment
#endif
#endif
#if ((defined(X64) || defined(IA64)) && !defined(_AMD64_) && !defined(_IA64_)) || \
    (!defined(X64) && !defined(IA64) && (defined(_AMD64_) || defined(_IA64_)))
#ifdef _IA64_
#error ERROR: Building X86 in the IA64 environment
#else
#error ERROR: Building X86 in the X64 environment
#endif
#endif


#endif /* PI__DEFINITIONS */
/*===== end of include file PIDEFS.H =========================================*/

