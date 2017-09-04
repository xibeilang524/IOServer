/*-----------------------------------------------------------------------------

$Archive: /PI/freeze/intel/piapi/pistatus.h $
  
-------------------------------------------------------------------------------
    
Copyright (c) 1994-2001 OSI Software, Inc. All rights reserved. 
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

This file enumerates the pistatus states. It may be included in C or C++
programs.

Note that the C functions use the typedef for the status code enumerations
directly. This exposes the structure of the status object to C programs and
therefore makes it difficult to change that structure in a backward compatible
manner. For example, if we add another field to the structure for parameters
or detailed error codes, C programs could not use the new features without
changing code. The alternative is to use a pointer to the PIstatus structure
rather than the pistatus enum. This would let us change that structure without
affecting C programs. The shortcoming is that all C code would have to call
C++ routines to allocate and deallocate PIstatus structures. This is less
convenient and more error prone than just declaring a variable. Also, the
implementation used below is what C programmers expect to see - an error code
like errno or a VMS condition code. (MDH)

-------------------------------------------------------------------------------

$Log: /PI/freeze/intel/piapi/pistatus.h $

58    11/12/03 10:14a Ssortland
Updated to PI-API version 1.3.9.1

7     2/11/03 9:49a Cmuraski
Add PI_NET_NO_BYTES and PI_NET_EXCESSIVEREADRETRIES

6     10/15/02 12:36p Azheng
Punchlist #2646OSI8: When gethostbyname fails, the status is set to
GetLastError on Windows and errno on UNIX. However, on some UNIX
platforms, errno is not correctly set and equals zero, which implies
good status.  This has been fixed by introducing a new error number
PI_UNABLETORETRIEVEUNIXERRNO which would be returned if errno equals to
0.

5     8/26/02 2:18p Cmuraski
Remove assert() for invalid variant type in PIvariant::activate()

4     8/21/02 5:22p Jon
Punchlist 2458OSI8, 2454OSI8 and 2425OSI8.

3     7/12/02 1:30p Jon
Merging from $/PI/3.4/libraries/piut/pistatus.h

10    7/12/02 1:19p Jon
Threading

9     7/02/02 6:15p Harry
Added 3.3 status enums -11101 thru -11104 and renumbered
PI_AR_NULLLOADREC and PI_AR_PTLOCKFAIL to -11105 and -11106
respectively.

8     5/12/02 5:15p Jon
Threading

7     5/10/02 4:58p Jon

6     5/02/02 3:26p Rulik
New Archive cache statuses

123   2/26/02 5:38p Jon
Punchlist 1877OSI8.

122   1/25/02 8:10a Jon
Moved setsyserror into cxx file punchlist 1704OSI8.

121   1/24/02 6:11p Jon
Punchlisg 1705OSI8

120   1/22/02 7:59a Jon
Punchlist 1747OSI8.

119   1/10/02 11:24a Jon
Punchlist 1705OSI8

118   1/09/02 5:48p Jon
Punchlist 17005OSI8.

117   1/06/02 10:26a Jon
Punchlist 1697OSI8

116   12/27/01 6:32p Jon
Punchlist 1677OSI8. New ServerID strategy.

115   12/04/01 3:58p Ling
Add error code for qcnt routine

114   01/11/12 16:45 Charlie
invalid tz configuration/data

113   11/09/01 6:41p Rulik
Prevent creation of "Null Trust" (trust with only name and PIuser)

112   10/18/01 10:53p Rulik
Audit status codes

111   9/27/01 7:07p Rulik
Added bad dig translation code

110   9/25/01 9:14a Rulik
Better archive backup errors - mode mismatch

109   9/06/01 11:58a Jwong
fixed error from last check in

108   9/06/01 11:57a Jwong
Added warning PI_CTR_DONOTINSTALLCOUNTERS

107   01/08/06 15:55 Charlie
1249osi8; Added PI_PATHNOTFOUND for PIdir's failure to find a requested
path.

106   7/23/01 12:04p Rulik
Punchlist 1307OSI8
Return warning when consumer in overflow state and some events lost. 

105   7/07/01 7:01p Rulik
New archive codes (shift/backup)

104   7/03/01 2:04p Ray
Fixed unterminated C-style comment. Caught by DEC UNIX.

103   6/05/01 8:53p Rulik
Added more test codes

102   5/08/01 3:01p Jon

101   5/05/01 9:21a Jon
Added new MDB error. (1141OSI8)

100   5/01/01 21:12 Rulik
Added Test utility statuses

99    3/29/01 10:52a Ray
Added comments for PI_TABLEINVIDENTIFIER and PI_PE_PCT_GOOD_TOO_SMALL.

98    3/26/01 16:45 Rulik
Better error codes for duplicate trust and for IP address without Mask

97    2/27/01 15:56 Rulik
Added PI_NOTADIG status

96    1/30/01 2:47p Jon

95    1/16/01 5:57p Jon

94    1/05/01 2:32p Klee
Replaced all occurrences of UDA with UDS.

93    11/22/00 9:57 Rulik
Added codes for digital sets/states count limits

92    10/30/00 5:48p Jon
Better error for remote connection error.

91    10/19/00 15:41 Rulik
Trust relation codes

90    10/02/00 6:35p Jon

89    9/27/00 4:38p Jon

88    9/26/00 10:59a Jon

87    8/31/00 14:39 Rulik
Added missing PInet 2 codes 1000-1002

86    7/26/00 5:16p Jon

85    7/26/00 1:18p Ken
Fix error codes that were > 32,000

84    7/20/00 15:49 Rulik
System Backup statuses

83    6/20/00 4:15p Klee
Added UDS errors to the PIstatus enumeration. Error numbers range from
-11200 to -11208. There is an added  #define statement that equates
PI_UDS_UNAVAIL to PI_3PHUNAVAIL for easier identification of UDS error
below the enumeration.

82    6/06/00 4:51p Jon

81    5/31/00 10:18a Jon

80    5/03/00 16:40 Rulik
annotation statuses

79    4/24/00 12:14p Jon

78    2/28/00 15:29 Rulik
added max event error

77    1/31/00 2:55p Richard
Migration to Standard Library Iostreams.

76    1/27/00 5:08p Jon
Added helper macro.

75    1/21/00 5:24p Jon
More moduledb statuses.

74    1/19/00 5:48p Jon
More moduledb error codes

73    1/14/00 3:24p Jwong
Added warning for performance counter installation

72    1/14/00 11:48a Jwong
Added and changed performance counter errors

71    1/11/00 1:31p Jon
More module Db statuses

70    1/10/00 4:04p Jwong
Added performance counter error codes

69    12/30/99 5:04p Jon
Added some pimoduledb errors.

68    12/21/99 9:05p Jwong
added support for pi message subsystem pintlogmgr

67    12/12/99 10:11a Jon
Merging from $/PI/3.2b/libraries/piut/pistatus.h

57    12/07/99 5:41p Jon
More trace work.

56    12/07/99 4:30p Jon
Don't copy _bTrace in = or copy constructor--want to keep the state of
the original.

55    12/07/99 2:26p Jon

54    12/07/99 12:50p Jon
_strOwner fro debugging

53    12/07/99 11:22a Jon
Added a debug trace.

52    9/22/99 10:48p Richard
Added error status for operations on an uninitialized archive file.

51    6/02/99 10:44a Jon
Added completion port error status.

66    11/17/99 4:02p Ray
Fixed C++ comments so that the file is useable from C.

65    11/16/99 5:06p Jon
Continue module implementation.

64    11/12/99 3:56p Jon

63    11/10/99 4:03p Rulik
added PI_INVALID

62    10/18/99 1:00p Jwong
Added PI Performance Counter errors

61    10/07/99 11:45a Richard
Merge from 3.2.357.17.

52    9/22/99 10:48p Richard
Added error status for operations on an uninitialized archive file.

51    6/02/99 10:44a Jon
Added completion port error status.

60    7/27/99 4:54p Ccoen
Undid addition of -529 SQC error because this was meant for PI 3 not
the old Pi errors.

59    6/25/99 4:21p Tgrosz
added new status for use in SQC (-529)

58    6/13/99 12:43p Richard
Merge from 3.2.357.9.

51    6/02/99 10:44a Jon
Added completion port error status.

57    5/03/99 1:36p Jon
Added status for invalid generic table.

56    4/30/99 2:41p Jon

55    4/20/99 5:34p Jon
New status for PInttemplate

53    4/05/99 2:07p Roger
added status for pctgood test fail in calls for archive summary
funcitons.

52    3/22/99 4:42p Richard
Added support for PI3PH specific errors.

51    2/02/99 11:59a Roger
added member functions to activate from character array (testing with
pimonitor object).

49    27/10/98 11:45 Rulik
Added PI_DS_CANTDELETESYSSET status 
(cannot delete system state-set)

48    10/02/98 9:38a Richard
Record locking errors.

47    9/29/98 2:27p Jon

46    9/21/98 3:23p Richard
Added codes for database security.

45    9/21/98 11:24a Jon
Added status for rcp table generation mismatch.

44    9/15/98 11:50a Jwong
Added statuses for message queries in PImsgfile::GetMessages

43    8/24/98 10:55a Jon
Allow duplicate states in state set--changed error code to reflect
this.

42    8/14/98 1:55p Jon
Added statuses for attribute and class name validation

41    8/13/98 3:50p Jon

40    8/13/98 1:21p Richard
Added version information accessor methods to pistatus.hxx and
pistatus.cxx to  allow getting current version information without
being compiler dependent on changes to pivers.hxx. On builds, resources
will be recompiled, modules linked and a couple of necessary cxx
modules will be compiled. This should allow a much cheaper cost of
updating the build number.
In addition, PI_VERS has been removed from pivers.hxx altogether and it
is up to the programmer to define this variable in the project compiler
define directives if needed. 

39    8/12/98 4:03p Jon
Added some dig state error codes

38    7/24/98 2:36p Richard
Added new archive manager errors.

37    7/14/98 12:13p Jon
Added invalid bookmark error

36    7/09/98 4:05p Jon
New codes for point sdk work.

35    7/09/98 2:16p Jon
Added no matching points status

34    6/23/98 1:59p Jon
New codes for point sdk.

33    6/19/98 11:37a Jon

32    6/19/98 11:23a Jon
Added point sdk warning.

31    5/19/98 11:16a Richard
Added archive not writeable and notshiftable error codes.

30    4/16/98 5:28p Jon
Added invalid session id error code

29    2/03/98 4:00p Richard

28    1/30/98 2:58p Jon
Added messages throttled error

27    1/28/98 1:46p Jon
Added a batch error code.

26    12/02/97 6:06p Jon
Include errno.h on unix

25    11/17/97 8:37p Jon
Added method setsyserror()

24    11/10/97 10:27a Richard
Separated shared error code in the archive into three distict code.

23    20/10/97 15:56 Rulik
Added Licence manager codes

22    10/17/97 12:01p Jon
Added low disk space status

21    9/11/97 3:37p Jon
Created function piut_ForceCrash. 

20    9/09/97 12:15p Jon
Added copy constructor and assignment operator which takes a PIstatus.
Lack of this lead to erroneous assignments on AIX.

19    8/01/97 3:27p Ray
Removed PI_SQL_ABORT; duplicates PI_SQL_USERABORT.

18    8/01/97 3:20p Ray
Added PI_SQL_BUSY and PI_SQL_ABORT.

17    7/29/97 8:25p Roger
added a fix to a problem

16    7/29/97 8:16p Roger
added one

14    7/22/97 12:21p Richard
Added new error codes.

13    6/24/97 11:46a Jon
Added excessive zero byte read status

12    5/01/97 5:27p Ray
Changed C++ comment markers to C

11    13-03-97 15:31 Ray
added status codes for PI_SQL and PI_GRID

10    3/05/97 3:12p Rulik
added offline archive codes
made PIstatus::str more efficient (less construction destruction...)

9     2/28/97 10:34p Richard
Added additional archive status'.

8     2/20/97 11:37a Jon
Added more specific pinet rpc codes
    
6     2/11/97 3:09p Jon  
Added two pibatch statuses  
    
5     1/29/97 7:16p Richard  
Added additional archive record loading error codes:  PI_AR_RCHDRMM and  
PI_AR_RCHDPMM.  

Migrated from pistatus.h 1.110 01/24/97
02-Jul-93  RAB  Original
01-Nov-93  MDH  Make pistatus a class w/methods, uppercase PI in PIstatus
27-Jan-94  RAB  Added status codes for the PIfilebase class
 7-Feb-94  BV   Add PIREC error codes, force enum to 32-bit representation
 8-Feb-94  BV   Add PI_ERR_ACTIVATE
 9-Feb-94  BV   Add PI_ERR_PASSIVATE
10-Feb-94  BV   Add destructor
 8-Mar-94  MDH  Add PI_WRONGVALTYPE, PI_NOTABLOB
 9-Mar-94  BV   Add PIREC_NAMETYPEMISMATCH
10-Mar-94  BV   Make getcode() const
21-Apr-94  BV   Add PI_ERR_IMPOSS error code
31-May-94  RP       Add DS table codes
09-Jun-94  JHP  Add PI Net codes
14-Jun-94  RAB  Added PI_TESTSTATUS macro
01-Jul-94  RP   Added #include <iostream.h>
23-Aug-94  RCV  For _WINDOWS: don't define PISTAT_FOR_COMPATIBILITY_WITH_C
31-Aug-94  MDH  activate & passivate return a PIstatus (no exception handling)
                remove PIserialerr class
                remove underscore from PI_TESTSTATUS
12-Sep-94  RAB  changed spelling of PI_ERR_UNDERRRANGE to PI_ERR_UNDERRANGE.
18-Nov-94  RAB  Changed _WINDOWS to _DOS since it has to do with 16 bit
                compilers not windows.
22-Dec-95  jhp  Added block for batch subsystem		
18 Mar 96  jhp  changed bool to pibool
14-May-96  jhp  Removed virtual from passivate, activate and dump.  Reduce overhead
               of v-table for common objects that are not superclasses.
21-Jun-96  RP    Added update-manager codes
16-Jul-96  RP    Added pi batch codes
22-aug-96  jhp   only include iostream.h if--moved include within #ifdef __cplusplus
11-Feb-96  jhp  added pibatch messages, pinet protocol error messages

-----------------------------------------------------------------------------*/ 
#ifndef PICL_PISTATUS
#define PICL_PISTATUS
/*----------------------------------------------- includes ------------------*/

#ifdef PI_SHARED_BUILD
#include "piutshr.h"
#else
#define PIUTSHR_API 
#endif

#ifdef __cplusplus
#include <iostream>
// the following line to be removed after migration 
using namespace std;
//
#endif /* __cplusplus */

#include "pidefs.h"
#ifdef unix
#include "errno.h"
#endif

#ifdef __cplusplus

#define PITESTSTATUS( s ) if ( s.isbad() ) return s

class PIstring;        // msg method returns a PIstring

class PIUTSHR_API PIstatus
{
public:
#endif /* __cplusplus */
enum {
   PI_NORMAL = 0,              /* normal completion */
   PI_ERROR = -1,              /* error completion (check errno) */
   
   /* -1 <= code <= -9999 are old system codes */
   PI_ATT_BLANKTAG        = -2,  /* blank tag */
   PI_ATT_PTDBFULL        = -3,  /* point data base full */
   PI_ATT_MEMDBFULL       = -4,  /* memory data base full */
   PI_ATT_TAGNOTFOUND     = -5,  /* tag not found */
   PI_ATT_BADCHARTAG      = -6,  /* illegal characters in tag */
   PI_ATT_TAGEXISTS       = -7,  /* tag already exists */
   PI_ATT_BADTIME         = -8,  /* time is after current time and date or time is <0*/
   PI_ATT_BADINTVALUE     = -9,  /* illegal status value or integer value */
   PI_ATT_OTHERPROC       = -10, /* cannot create tag b/c another proc is creating tags*/
   PI_ATT_BADDIGCODERANGE = -11, /* digital code is out of range */
   PI_ATT_NODSSTRING      = -12, /* digital state string not found */
   PI_ATT_BADDSCODERANGE  = -13, /* digital state code out of range for this tag */
   PI_ATT_NOSOURCETAG     = -15, /* source tag not found */
   PI_ATT_PTATTZERO       = -19, /* point att zero error (or disk error for ptdb)*/
   PI_ATT_BADZEROSPAN     = -20, /* bad zero or span for integer point */
   PI_ATT_NEGSPAN         = -21, /* span not greater than zero */
   PI_ATT_BADTYPVALUE     = -22, /* typical value not between zero and top of range */
   PI_ATT_BADDSCODE       = -23, /* bad digital state code */
   PI_ATT_BADNUMDS        = -24, /* bad number of digital states */
   PI_ATT_BADPTTYPE       = -25, /* illegal point type */
   PI_ATT_BADPTSOURCE     = -26, /* illegal point source */
   PI_ATT_BADENGUNIT      = -27, /* illegal engineering unit code */
   PI_ATT_BADLOC1         = -28, /* bad location parameter 1 (range depends on pt src)*/
   PI_ATT_BADLOC2         = -29, /* bad location parameter 2 (range depends on pt src)*/
   PI_ATT_BADLOC3         = -30, /* bad location parameter 3 (range depends on pt src)*/
   PI_ATT_BADLOC4         = -31, /* bad location parameter 4 (range depends on pt src)*/
   PI_ATT_BADLOC5         = -32, /* bad location parameter 5 (range depends on pt src)*/
   PI_ATT_BADTOTPTTYPE    = -33, /* point type of totalized point is not real */
   PI_ATT_BADRATEPT       = -34, /* rate point type must be real or integer */
   PI_ATT_BADRESCODE      = -35, /* illegal resolution code */
   PI_ATT_BADARCHONOFF    = -36, /* bad archiving on/off value */
   PI_ATT_BADCOMPONOFF    = -37, /* bad compressing on/off value */
   PI_ATT_BADCOMPDEV      = -38, /* illegal compression deviation specification */
   PI_ATT_BADCOMPMINTIME  = -39, /* illegal comp min time spec */
   PI_ATT_BADCOMPMAXTIME  = -40, /* illegal comp max time spec */
   PI_ATT_BADEXCDEV       = -41, /* illegal exception deviation specification */
   PI_ATT_BADEXCMINTIME   = -42, /* illegal comp min time spec */
   PI_ATT_BADEXCMAXTIME   = -43, /* illegal comp max time spec */
   PI_ATT_BADFILTCODE     = -44, /* illegal filter code */
   PI_ATT_BADTOTCODE      = -45, /* illegal totalization code */
   PI_ATT_BADTOTCONV      = -46, /* illegal totalization conversion factor */
   PI_ATT_BADSQRTCODE     = -49, /* bad square root code */ 
   PI_ATT_BADSCANONOFF    = -50, /* bad scan on/off code */
   PI_ATT_RESCODECONST    = -51, /* cannot change resolution code */
   PI_ATT_BADTIMEFORMAT   = -52, /* bad time and date string (must be VMS time) */
   PI_ATT_POINTNOTTOTAL   = -53, /* point is not a total */
   PI_ATT_TOTDBFULL       = -54, /* totalization data base full */
   PI_ATT_PTNOTINTOTDB    = -55, /* point not defined in totalization data base */
   PI_ATT_SINGLEQUOTE     = -56, /* tagname should be in single quotes, not double */
   PI_ATT_MISSINGTAG      = -57, /* filter or trigger tag does not exist */
   PI_ATT_EXTDESCSYNTAX   = -58, /* syntax error in extended descriptor */
   PI_ATT_BADEXTDESCKEYWD = -59, /* unrecognized keyword in extended descriptor */
   PI_ATT_NOPIDAVAIL      = -60, /* no more PID slots for retrieving point update lists */
   PI_ATT_NOTAGCHANGE     = -61, /* no more tags created, edited, or deleted */
   PI_ATT_NOPTUPDATE      = -62, /* not signed up for point updates */
   PI_ATT_BADDISPDIG      = -63, /* display digits <-20 or >10 */
   PI_ATT_BADCLAMPCODE    = -64, /* bad clamping code (<0 or >3) */
   PI_ATT_BADSCHEDCODE    = -65, /* bad scheduling code (<0 or >2) */
   PI_ATT_NONATSCHED      = -66, /* natural scheduling not allowed for post-processed tags*/
   PI_ATT_BADGROUPPSIZE   = -67, /* bad group size for moving average (<2) */
   PI_ATT_BADPERIOD       = -68, /* bad period */
   PI_ATT_BADOFFSET       = -69, /* bad offset (<0 or >86399 */
   
   PI_EVM_BADNUMPOINTS   = -70, /* number of points less than one */
   PI_EVM_BADPOINTNUM    = -71, /* point number out of range */
   PI_EVM_NOMOREEXCEPT   = -72, /* no more room for progs requesting exceptions */
   PI_EVM_TOOMANYPTS     = -73, /* no room for this many points for this list */
   PI_EVM_NOMOREPTS      = -74, /* no room for more points */
   PI_EVM_NOPTSESTAB     = -75, /* no points established */
   PI_EVM_PTSNOTDISESTAB = -76, /* some points not disestablished */
   PI_EVM_BUFFEROVERFLOW = -77,  /* exception buffer overflow */
   PI_EVM_TIMEOUT        = -78, /* timed out */
   PI_EVM_BADTIMEOUT     = -79, /* bad timeout value */
   PI_EVM_SWITCHSERVER   = -80, /* server changed before exception buffer emptied */
   
   PI_ARCH_DATENOTONLINE   = -101, /* date not on-line */
   PI_ARCH_RECORDNOTFOUND  = -102, /* record not found (empty) */
   PI_ARCH_NODATA          = -103, /* no data for this point at this time */
   PI_ARCH_BADSTARTENDTIME = -105, /* end time not after start time or start time <0 */  
   PI_ARCH_NOGOODDATA      = -106, /* no good data for this point at this time */
   PI_ARCH_NEGSQUAREROOT   = -107, /* cannot calculate square root of negative number */
   PI_ARCH_BADEDITTIME     = -108, /* time is before the editing time limit */
   PI_ARCH_VALUEEXISTS     = -109, /* value already exists at this time */
   PI_ARCH_SLOWARCHIVE     = -110, /* archive program not keeping up with events */
   PI_ARCH_30SECRULE       = -111, /* event not processed by archive program within 30s*/
   PI_ARCH_ARCH1OFFLINE    = -112, /* point not created b/c arch#1 not online */
   PI_ARCH_TOOFEWVALUES    = -113, /*'number of values' arg to arch retrieval routine too small*/
   
   PI_PE_CORRUPTFILE      = -201, /* performance equation file corrupted */
   PI_PE_CANTDELETEPETAG  = -202, /* cannot delete tag that is used in PE */
   PI_PE_CANTDELETETOTTAG = -203, /* cannot delete tag that is used in totalization */
   PI_PE_CANTDELETESQCTAG = -204, /* cannot delete tag that is used in SQC calc */
   
   PI_DC_CORRUPTFILE      = -301, /* display file corrupted */
   PI_DC_CANTDELETETAG    = -302, /* cannot delete tag that is used on a display */

   PI_SQL_MEMERR          = -251, /* memory allocation error */
   PI_SQL_SYNERR          = -252, /* syntax error */
   PI_SQL_INVSTMT         = -253, /* invalid statement */
   PI_SQL_INTERR          = -254, /* severe internal error */
   PI_SQL_NOSUPPORT       = -255, /* unsupported statement */
   PI_SQL_TIMERR          = -256, /* invalid time */
   PI_SQL_TAGERR          = -257, /* tag not found */
   PI_SQL_TYPERR          = -258, /* invalid data type */
   PI_SQL_CALCERR         = -259, /* calculation error */
   PI_SQL_INVWHERE        = -260, /* invalid WHERE clause */
   PI_SQL_COLERR          = -261, /* column count error */
   PI_SQL_NOSELECT        = -262, /* statement is not a SELECT */
   PI_SQL_SYSTEMERR       = -263, /* system error occurred */
   PI_SQL_TIMESTEPERR     = -264, /* TIMESTEP error */
   PI_SQL_NOQUERY         = -265, /* no query found in string */
   PI_SQL_USERABORT       = -266, /* user aborted execution */
   PI_SQL_INVARG          = -267, /* invalid function argument */
   PI_SQL_NOTSAFE         = -268, /* execution too expensive*/
   PI_SQL_INVTABLE        = -269, /* invalid table name */
   PI_SQL_INVALIAS        = -270, /* invalid table alias name */
   PI_SQL_BADINI          = -271, /* invalid INI file entries */
   PI_SQL_TRUNC           = -272, /* returned string truncated */
   PI_SQL_TIMEOUT         = -273, /* query timed out */
   PI_SQL_NOPARAM         = -274, /* run-time parameter missing */
	PI_SQL_BUSY            = -275, /* PI-SQL handle is busy with another operation */

   /* PIgrid: see also codes in range -12149 to -12100 */
   PI_GRID_MEMERR         = -280, /* memory allocation error */
   PI_GRID_RANGERR        = -281, /* column index out of range */
   PI_GRID_NOCOLTYPE      = -282, /* column not defined */
   PI_GRID_NOCOLSIZE      = -283, /* data size not set */
   PI_GRID_INVDTYPE       = -284, /* invalid data type */
   PI_GRID_INVDSIZE       = -285, /* invalid data size */

   PI_SQC_BADCHARTTYPE    = -501, /* bad chart type */
   PI_SQC_BADALARMGROUP   = -502, /* bad alarm group number */
   PI_SQC_CALCDBFULL      = -503, /* sqc calculation database full */
   PI_SQC_BADSAMPLESIZE   = -504, /* bad sample size (<0 or >32767) */
   PI_SQC_BADSAMPLEPERIOD = -505, /* bad sample period (<10 sec) */
   PI_SQC_BADCALCPERIOD   = -506, /* calculation period < sample period */
   PI_SQC_BADPERIODSTART  = -507, /* bad start period time */
   PI_SQC_BADIGNORESAMP   = -508, /* bad number of samples to ignore after trigger */
   PI_SQC_BADMOVAVGWT     = -509, /* bad geo moving avg weight */
   PI_SQC_CUSUMKNEG       = -510, /* CUSUM k <0 */
   PI_SQC_CUSUMHNEG       = -511, /* CUSUM h <0 */
   PI_SQC_BADINITCUSUM    = -512, /* bad initial CUSUM (<0 or >h) */
   PI_SQC_BADTESTGROUP    = -513, /* bad test group number (<1 or >256) */
   PI_SQC_CALCMISSING     = -515, /* sqc calculation not found */
   PI_SQC_RAWTAGMISSING   = -516, /* raw data tag for sqc calc not found */
   PI_SQC_TRIGTAGMISSING  = -517, /* trigger tag for sqc calc not found */
   PI_SQC_CHARTTAGDEFD    = -518, /* sqc chart tag already defined in sqc db */
   PI_SQC_CHARTEQUALSRAW  = -519, /* chart tag is the same as raw tag */
   PI_SQC_INPUTFILEMISSING= -520, /* input file does not exist */
   PI_SQC_BADINPUTFILEFMT = -521, /* input file is not properly formatted */
   PI_SQC_BADRANGEVALUE   = -522, /* value is out of range */
   PI_SQC_BADRANGEALARM   = -523, /* alarm number is out of range */
   PI_SQC_CANTWRITEFILE   = -524, /* cannot open file for writing */
   PI_SQC_COMMENTMISSING  = -525, /* comment not found in comment file */
   PI_SQC_INVARRAYINDEX   = -526, /* array index is out of valid range */
   PI_SQC_CTLLIMTAGMISSING= -527, /* control limit tag is not in sqc structure */
   PI_SQC_CTLLIMNOTINFILE = -528, /* control limits not found in control limit file */
 
   PI_EP_BADCONST        = -801, /* illegal constant in exp */
   PI_EP_BADNUM          = -802, /* illegal number in exp */
   PI_EP_TOOMANYTAGS     = -803, /* too many constants/tags in exp */
   PI_EP_TOOCOMPLEX      = -804, /* exp too complicated */
   PI_EP_BADKEYWORD      = -805, /* illegal keyword in exp */
   PI_EP_BADCHAR         = -806, /* illegal character in exp */
   PI_EP_TOOMANYUNARIES  = -807, /* too many unary operators in exp */
   PI_EP_BADBOOLEANS     = -808, /* illegal combination of bool symbols in exp */
   PI_EP_UNBALPARENS     = -809, /* unbalanced parentheses in exp */
   PI_EP_TOOMANYRTPARENS = -810, /* too many right parentheses in exp */
   PI_EP_NOPARENS        = -811, /* nothing in parentheses in exp */
   PI_EP_BADIF           = -812, /* illegal if-then-else struct in exp */
   PI_EP_CMDSTACKOVERFLOW= -813, /* command stack overflow in exp */
   PI_EP_NULLEQUATION    = -814, /* null exquation for expression */
   PI_EP_REDSTACKOVERFLOW= -815, /* reduction stack overflow in exp */
   PI_EP_BADTOKEN        = -816, /* illegal token in exp */
   PI_EP_SYNTAX          = -817, /* syntax error in exp */
   PI_EP_CALCSTACKOVERFLOW= -818, /* calculation stack overflow in exp */
   PI_EP_DISKERR         = -819, /* disk error reading constant files for exp */
   PI_EP_CALCOVERFLOW    = -820, /* calculation overflow in exp */
   PI_EP_DIVZERO         = -821, /* division by zero in exp */
   PI_EP_BADARGUMENT     = -822, /* illegal function arg in exp */
   PI_EP_BADEXPONENT     = -823, /* non-integer exponent in exp */
   PI_EP_BADCONSTANT     = -824, /* undefined constant in exp */
   
   PI_EE_BADTAG          = -900, /* undefined tag in exp */
   PI_EE_BADIF           = -912, /* illegal if-then-else struct in exp */
   PI_EE_NULLEQUATION    = -914, /* null equation for expression */
   PI_EE_BADTOKEN        = -916, /* illegal token in expression */
   PI_EE_SYNTAX          = -917, /* syntax error in expression */
   PI_EE_CALCSTACKOVERFLOW= -918, /* calculation stack overflow in exp */
   PI_EE_CALCOVERFLOW    = -920, /* calculation overflow in exp */
   PI_EE_DIVZERO         = -921, /* divide by zero in exp */
   PI_EE_BADARGUMENT     = -922, /* illegal function  */
   PI_EE_BADEXPONENT     = -923, /* non-integer exponent in exp */
   PI_EE_BADVALUE        = -925, /* Illogical current value in expression */
   
   PI_NET2_BADTABLEID     = -981, /* table id specified is not supported */
   PI_NET2_BADTABLECODE   = -982, /* specified table code is not supported */
   PI_NET2_BADTRANSMODE   = -983, /* requested transaction mode was not valid */
   PI_NET2_BADFUNCCODE    = -991, /* PINET function code is not valid */
   PI_NET2_BADLENGTH      = -992, /* specified length is not consistant */
   PI_NET2_BADCOUNT       = -993, /* specified count is not valid */
   PI_NET2_BADPINETVER    = -994, /* incompatible PINET version */
   PI_NET2_BADMSGSEQ      = -995, /* bad PINET message sequence */
   PI_NET2_MSGTOOBIG      = -996, /* message too big for PINET */
   PI_NET2_MEMALLOCERR    = -998, /* memory allocation error */
   PI_NET2_LOGINREQD      = -999, /* request not permitted without login */
   PI_NET2_BADGRID        = -1000, /* Grid not implemented */
   PI_NET2_NODEFHOST      = -1001, /* Default host not found */
   PI_NET2_NOBATSUBSYS    = -1002, /* No batch subsystem  on PI2 server*/


   PI_TOPOFOLD = -9999,        /* top of the old PI System Error Codes */
   
   /* -10399 <= PI_ERR_??? <= -10000 */      /* generic errors */
   PI_ERR_MALLOC      = -10000,  /* failed memory allocation */
   PI_ERR_NEW         = -10001,  /* failed new operation */
   PI_ERR_ACTIVATE    = -10002,  /* unable to activate object */
   PI_ERR_PASSIVATE   = -10003,  /* unable to passivate object */
   PI_ERR_IMPOSS      = -10004,  /* internal error (impossible) */
   PI_ERR_UNDERRANGE  = -10005,  /* subscript under range        */
   PI_ERR_OVERRANGE   = -10006,  /* subscript over range        */
   PI_ERR_BADPOINTER  = -10007,  /* bad (or null) pointer */
   PI_ERR_UNSUPPORTED = -10008,  /* unsupported or unimplemented call */
   PI_SHUTDOWN        = -10009,  /* pisystem is being shutdown        */
   PI_ERR_TIMEOUT     = -10010,  /* system timed out */
   PI_ERR_ACCESSLOCK  = -10011,  /* target object in use or locked */
  
   /* Security and User Classes */
   /* -10449 <= PI_XS_??? <= -10400 */
   PI_XS_NOREAD       = -10400, /* no read access - secure object */
   PI_XS_NOWRITE      = -10401, /* no write access - secure object */
   PI_XS_BADSTRING    = -10402, /* badly formed access string */
   PI_XS_BADUGVER     = -10403, /* version mismatch in usergroup activate */  
   PI_XS_BADUSERVER   = -10404, /* version mismatch in user activate */
   PI_XS_CTXIDINUSE   = -10405, /* user context id in user */
   PI_XS_GROUPINUSE   = -10406, /* group id in use */
   PI_XS_NOACCESS     = -10407, /* no read or write access - secure object */
   PI_XS_BADUSERSTR   = -10408, /* invalid user/owner string */
   PI_XS_BADGRPSTR    = -10409, /* invalid group string */
   PI_XS_BADDBNAME    = -10410, /* invalid database name */
   PI_XS_MISMATCH     = -10411, /* security or trust relation mismatch */
   PI_XS_TRUSTUNSUP   = -10412, /* trust relation not supported on server */
   PI_XS_NOTRUST      = -10413, /* No trust relation for this request */
   PI_XS_SIMILARTRUST = -10414, /* Trust records must differ more than name and PIuser */
   PI_XS_IPANDMASK    = -10415, /* Must specify both IP address and Net Mask */
   PI_XS_NONULLTRUST  = -10416, /* Trust definition must contain some specifications */

   /* -10499 <= PI_FB_ <= -10450 */
   PI_FB_BADCREATEFILE   = -10450,  /* create new file failed */
   PI_FB_BADOPENFILE     = -10451,  /* open existing file failed */
   PI_FB_BADFILECLOSE    = -10452,  /* close file failed */
   PI_FB_BADFILEREMOVE   = -10453,  /* delete file failed */
   PI_FB_BADFILETRUNCATE = -10454,  /* truncate file failed */
   PI_FB_INVALIDFILE     = -10455,  /* fstat failed */
   PI_FB_BADFILELOCK     = -10456,  /* fcntl failed to get lock */
   PI_FB_BADFILEUNLOCK   = -10457,  /* fcntl failed to free lock */
   PI_FB_BADHEADER       = -10458,  /* bad header params */
   PI_FB_FILEOPEN        = -10459,  /* file open */
   PI_FB_FILECLOSED      = -10460,  /* file closed */
   PI_FB_BADFILEREAD     = -10461,  /* read to file failed */
   PI_FB_BADFILEWRITE    = -10462,  /* write from file failed */
   PI_FB_BADRECNOVALUE   = -10463,  /* recno greater than directory size */
   PI_FB_RECNOINUSE      = -10464,  /* recno in use */
   PI_FB_RECNONOTFOUND   = -10466,  /* no record available for passed recno */
   PI_FB_BADRECLEN       = -10467,  /* reclen greater than maximum allowed size */
   PI_FB_TIMETOGROWDIR   = -10468,  /* getting new recno, directory is full */
   PI_FB_BADBUFFERSIZE   = -10469,  /* passed buffer to small to hold record */
   PI_FB_BADFILESEEK     = -10470,  /* lseek failed */
   PI_FB_BADFILEHEADER   = -10471,  /* fatally corrupted file header */
   PI_FB_DIRTYHEADER     = -10472,  /* dirty header */
   PI_FB_INVUSERBLOCK    = -10473,  /* invalid user block size */
   PI_FB_BADVERSION      = -10474,  /* version mismatch */
   PI_FB_CNTMISMATCH     = -10475,  /* header - index count mismatch */
   PI_FB_LOWDISKSPACE    = -10476,  /* low disk space, file size cannot be increased */
   
   /* -10699 <= PI_PD_ <= -10550  (Point Database and related classes) */
   PI_PD_TAGEXISTS       = -10550,  /* tag already exists in table */
   PI_PD_INVTAG          = -10551,  /* invalid chars in tag */
   PI_PD_BADCHAIN        = -10552,  /* broken context chain in table */
   PI_PD_INVPTID         = -10553,  /* ptid is not a valid point */
   PI_PD_CTXEXISTS       = -10554,  /* tag already exists for context */
   PI_PD_NOTAG           = -10555,  /* tag does not exist in table */   
   PI_PD_ENDSEARCH       = -10556,  /* end of table reached */
   PI_PD_BADASVER        = -10557,  /* bad ptattributeset version */
   PI_PD_BADPTCLVER      = -10558,  /* bad ptclass version */
   PI_PD_BADPOINTVER     = -10559,  /* bad point version */
   PI_PD_POINTINUSE      = -10560,  /* point already has a ptclass */
   PI_PD_PTCLASSINUSE    = -10561,  /* point class alread defined */
   PI_PD_CLASSESINUSE    = -10562,  /* ptclasses alread defined */
   PI_PD_BADPTCLASSREV   = -10563,  /* point class revision mismatch */
   PI_PD_NOPTCLASS       = -10564,  /* point class not defined */
   PI_PD_NOMOREPTS       = -10565,  /* no more points in search */
   PI_PD_VALIDFAIL       = -10566,  /* point validation failed */
   PI_PD_AMBIGUOUS       = -10567,  /* Ambiguous point specification */
   PI_PD_NOMATCHINGPOINTS= -10568,  /* Point search turned up no matches */
   PI_PD_BADATTRIBUTE    = -10569,  /* Point does not contain specified attribute */
   PI_PD_NOEDIT          = -10570,  /* Attempt to set or edit internally set attribute */
   PI_PD_INVALIDATT      = -10571,  /* Attempt to create attribute set with invalid attribute name */
   PI_PD_INVALIDNAME     = -10572,  /* Invalid name for point class or attribute set */
   PI_PD_NOBASEATT       = -10573,  /* Base attribute set not included in point class definition. */

   /* -10719 <= PI_DS_ <= -10700  (Digital State Table) */
   PI_DS_NAMEEXISTS      = -10700,  /* State  already exists in table */
   PI_DS_SETNOTFOUND     = -10701,  /* SET not found */  
   PI_DS_STATENOTFOUND   = -10702,  /* STATE not found */
   PI_DS_ILLEGALSETDEFINITION = -10703, /*  Illegal set definition, no states define*/
   PI_DS_CANTDELETESYSSET    = -10704, /* cannot delete system set */
   PI_DS_TOOMANYSETS    = -10705, /* Maximum number of Sets Exceeded */
   PI_DS_TOOMANYSTATES    = -10706, /* Maximum number of States Exceeded */


   /*  PI NET 8-jun-94  jhp  */
   /*  -10820 <= PI_NET_ <=  -10720 */
   PI_NET_TOP                = -10720,
   PI_NET_ABORT              = -10721, /* client aborted connection */
   PI_NET_TIMEOUT            = -10722, /* system call timedout      */
   PI_NET_NOCONNECTION       = -10723,
   PI_NET_QUEOVERFLOW        = -10724,
   PI_NET_SYNCHRONOUS        = -10725,
   PI_NET_ASYNCHRONOUS       = -10726,
   PI_NET_RPC_NONEXISTENT    = -10727,
   PI_NET_SEND_ERROR         = -10728,
   PI_NET_RECV_ERROR         = -10729,
   PI_NET_NO_MESSAGE         = -10730,
   PI_NET_INCOMPLETE_MESSAGE = -10731,
   PI_NET_CORRUPTED_MESSAGE  = -10732,
   PI_NET_RPCRESOLVEROFFLINE = -10733,
   PI_NET_BROKENCONNECTION   = -10734,
   PI_NET_TRANSACTIONLISTFULL= -10735,
   PI_NET_ILLEGALRPCID       = -10736,
   PI_NET_PENDING            = -10737,
   PI_NET_DISABLED           = -10738,
   PI_NET_INVALIDRPCSERVERTABLELIST = -10739,
   PI_NET_INVALIDHOST        = -10740,
   PI_NET_RPCRESOLVERNOTAVAILABLE = -10741,
   PI_NET_CONNECTIONLISTFULL = -10742,
   PI_NET_RPCRESOLVERTMPOFFLINE = -10743,
   PI_NET_TRANSLATORINUSE	= -10744,
   PI_NET_MESSAGEVERIFICATIONFAILURE = -10745,
   PI_NET_MESSAGETOOLARGE	= -10746,
   PI_NET_TRANSLATORVERIFICATIONFAILURE = -10747,
   PI_NET_NOROUTEENTRY = -10748,
   PI_NET_NORPCCODE = -10749,
   PI_NET_NORPCNAME = -10750,
   PI_NET_NORPCCALLBACK = -10751,
   PI_NET_EXCESSIVEZEROBYTEREADS = -10752,
   PI_NET_INVALIDSESSIONID = -10753,
   PI_NET_RPCTABLEGENMISMATCH = -10754,
   PI_NET_COMPLETIONPORTERROR = -10755,
   PI_NEWERSERVERVERSIONREQUIRED = -10756,  /* Requested RPC or PI-SDK call requires a newer PI Server Version */
   PI_NOREMOTECONNECTION = -10757, /* The remote PI-SDK connection is closed */
   PI_NET_FAILEDREMOTECONNECTION = -10758,
   PI_NET_NO_BYTES = -10759,       /* recv() returned zero bytes */
   PI_NET_EXCESSIVEREADRETRIES = -10760,
   /* archive related errors */
   /* -11199 <= PI_AR_ <= -11000, */
   PI_AR_RCHDVMM 	= -11001, /* record header version mismatch */
   PI_AR_RCHDDMM 	= -11002, /* record header data mismatch */
   PI_AR_RCHDSMM 	= -11003, /* record header header size mismatch */
   PI_AR_RCHDBADPTRECID = -11004, /* record header bad (0) pt record id */
   PI_AR_RCHDBADRECID 	= -11005, /* record header bad (0) record id */
   PI_AR_RCHDBADSIZE 	= -11006, /* record header bad set size byte count */
   PI_AR_RCHDOVERWRITE 	= -11007, /* record header stream overwrite */
   PI_AR_RCBADDATATYPE 	= -11008, /* attempted to add an event with an invalid data type */
   PI_AR_RCOBJTOOBIG 	= -11009, /* event contents are too big to fit in any record */
   PI_AR_NOTTYPEDIG 	= -11010, /* record not type digital */
   PI_AR_NOTTYPEFLOAT2 	= -11011, /* record not type float2 - zero/span do not apply */
   PI_AR_INVRECCNT 	= -11012, /* invalid record count for creating archive */
   PI_AR_INVRECSIZE 	= -11013, /* invalid record size for creating archive */
   PI_AR_BADSTREAMATT 	= -11014, /* failed to attach archive file to stream */
   PI_AR_ARCFILEVMM 	= -11015, /* archive file version mismatch */
   PI_AR_ARCFILERAB 	= -11016, /* archive file in unknown state */
   PI_AR_CRPTTIME 	= -11017, /* corrupt time values in archive header */
   PI_AR_CRPTPTRS 	= -11018, /* corrupt overflow or primary record pointers */
   PI_AR_ARCNOTMNTD 	= -11019, /* archive file not mounted */
   PI_AR_INVPTCNT 	= -11020, /* point count out of bounds (range) */
   PI_AR_CRPTCACHEREC 	= -11021, /* cache record corrupt (ptrecid mismatch) */
   PI_AR_PTRECIDBND 	= -11022, /* invalid value for requested ptrecid > current */
   PI_AR_BOUNDS 	= -11023, /* archive record full */
   PI_AR_BADCACHELOAD 	= -11024, /* cache->loadrecord failed */
   PI_AR_BADGETTARREC 	= -11025, /* arcmgr::gettargetrecord failed */
   PI_AR_BADOVERFLOW 	= -11026, /* arcfile::overflowrecord failed */
   PI_AR_BADARLOADREC 	= -11027, /* arcfile::loadrecord failed */
   PI_AR_DUPARCFAIL 	= -11028, /* request to load a loaded archive */
   PI_AR_BADSETINDEX 	= -11029, /* an index rec does not have and index */
   PI_AR_BADINDEXSRC 	= -11030, /* set index did not receive and index record */
   PI_AR_EMPTYRECORD 	= -11031, /* no events in record */
   PI_AR_NOEVENTAFTER 	= -11032, /* no events after passed eventid */
   PI_AR_NOEVENTBEFORE 	= -11033, /* no events before passed eventid */
   PI_AR_BADPTRECID 	= -11034, /* invalid ptrecid passed */
   PI_AR_STOREOPEN 	= -11035, /* underlying storage is open */
   PI_AR_BADARCRECORD 	= -11036, /* bad archive record pointer passed*/  
   PI_AR_BADARCFILE 	= -11037, /* bad archive file pointer passed */  
   PI_AR_NOARCHIVEBEFORE = -11038, /* no archive before passed archive */  
   PI_AR_NOARCHIVEAFTER = -11039, /* no archive after passed archive */  
   PI_AR_NORECORDBEFORE = -11040, /* no record before passed record */  
   PI_AR_NORECORDAFTER 	= -11041, /* no record after passed record */  
   PI_AR_RECNOTINCACHE 	= -11042, /* target record not in cache */
   PI_AR_DATENOTONLINE 	= -11043, /* no archive on line for target time */
   PI_AR_EVENTOUTOFORDER = -11044, /* attempting to add an event before last event */
   PI_AR_INDEXRECMISMATCH = -11045, /* add event index mismatch */
   PI_AR_DATEINFUTURE 	= -11046, /* target date in future */
   PI_AR_INVMAXCOUNT 	= -11047, /* invalid maximum count */
   PI_AR_INVINTERVAL 	= -11048, /* invalid intervals for archive call */
   PI_AR_INVTIMES 	= -11049, /* invalid times for archive call */
   PI_AR_INVRECTYPE 	= -11050, /* invalid record type (index/data) */
   PI_AR_COUNTTOOSMALL  = -11051, /* count not large enough for interval */
   PI_AR_SNAPMISMATCH   = -11052, /* count mismatch loading snapshot */
   PI_AR_ARCHIVEFULL    = -11053, /* no more available records in this archive */
   PI_AR_INVALIDSTATE   = -11054, /* invalid archive state for mounting or dismounting */
   PI_AR_INVSTIME       = -11055, /* invalid start time */
   PI_AR_INVETIME       = -11056, /* invalid end time */
   PI_AR_NOTENOUGHVALS  = -11057, /* not enough good values for calculation */
   PI_AR_INVSUMMARYCODE = -11058, /* invalid code for summary function */
   PI_AR_NOGOODDATA     = -11059, /* no good data for this time */
   PI_AR_CALCFAILED     = -11060, /* calculation failed (e.g. divide by zero) */
   PI_AR_INVMODEADD     = -11061, /* invalid mode for add event */
   PI_AR_INVMODEEDIT    = -11062, /* invalid mode for edit event */
   PI_AR_INVMODEDEL     = -11063, /* invalid mode for delete event */
   PI_AR_RCHDRMM 	= -11064, /* record header recid mismatch */
   PI_AR_RCHDPMM 	= -11065, /* record header chain pointer mismatch */
   PI_AR_EMPTYDRECORD 	= -11066, /* no events in data record */
   PI_AR_EMPTYIRECORD 	= -11067, /* no events in index record */
   PI_AR_BADPRIMARY     = -11068, /* error getting primary archive */
   PI_AR_CREATEFLAG     = -11069, /* archive create flag already set */
   PI_AR_NOARCHMOUNT    = -11070, /* no archives mounted */
   PI_AR_NOSHIFTARC     = -11071, /* no archive available for shift */
   PI_AR_REMNER         = -11072, /* No events in record for removal */
   PI_AR_REMENF         = -11073, /* Target event for removal not found in record */
   PI_AR_REPNER         = -11074, /* No events in record to replace */
   PI_AR_REPENF         = -11075, /* Target event for replacement not found in record */
   PI_AR_NAVBEFORE      = -11076, /* Target time before archive start time */
   PI_AR_NAVAFTER       = -11077, /* Target time after archive end time */
   PI_AR_NOTWRITEABLE   = -11078, /* Target archive is not writeable */
   PI_AR_NOTSHIFTABLE   = -11079, /* Target archive is not shiftable */
   PI_AR_DUPPRIMARY     = -11080, /* Attempted to register two primary archives */
   PI_AR_OVERLAP        = -11081, /* Attempted to register overlapping archives */
   PI_AR_RECLOCKFAIL    = -11082, /* Attempted to lock a locked archive record */
   PI_AR_RECUNLOCKFAIL  = -11083, /* Attempted to unlock an unlocked archive record */
   PI_AR_EMPTYFILE      = -11090, /* Attempting operation on an empty archive file */
   PI_AR_TOOMANYEVENTS  = -11091, /* Event collection exceeded maximu allowed */
   PI_AR_NOANNOTATION   = -11092, /* annotation not found in archive*/
   PI_AR_ANNOTMISMATCH  = -11093, /* annotation mismatch (archive) */
   PI_AR_ANNOTEXIST     = -11094, /* annotation already exist in archive */
   PI_AR_SHIFTINPROG    = -11095, /* Archive Shift in progress */
   PI_AR_BCKUPINPROG    = -11096, /* Archive Backup in progress */
   PI_AR_BCKMODEMISMTCH = -11097, /* Backup End must follow Start and vice versa */
   PI_AR_BADDIGDATA     = -11098, /* Cannot convert to a Digital state */
   PI_AR_SAMETIMEREC	= -11099, /* Start time equal end time in archive record */
   PI_AR_SAMETIMEARG	= -11100, /* Start time equal end time arg in qcnt call */
   PI_AR_SCALLFILTER	= -11101, /* All events are filtered in summary calc */
   PI_AR_SCNOEVENT		= -11102, /* no events in the summary calc time range */
   PI_AR_SCOOSCALL		= -11103, /* out of sequence call in summary calc */
   PI_AR_SCOOSEVENT		= -11104, /* out of sequence data event in summary calc */
   PI_AR_NULLLOADREC	= -11105, /* Invalid record pointer for loading */
   PI_AR_PTLOCKFAIL	    = -11106, /* Failed to lock a point */

   /* pi3ph (UDS) related errors */
   /* -11299 <= PI_AR_ <= -11200, */
   PI_3PHUNAVAIL              = -11200, /* PI-UDS is not installed properly or is not running */
   /* Below this enumeration  is a statement #define PI_UDS_UNAVAIL   PI_3PHUNAVAIL
      to keep PI_3PHUNAVAIL for backward compatibility, although for clarity one can use
      PI_UDS_UNAVAIL for the same error. */
   PI_UDS_HISTTIMEDVALUEFAIL  = -11201,	/* PI-UDS could not get archived value from foreign system */
   PI_UDS_HISTTIMEDVALUESFAIL = -11202,	/* PI-UDS could not get archived data from foreign system */
   PI_UDS_SNAPTIMEDVALUEFAIL  = -11203,	/* PI-UDS could not get snapshot value from foreign system */
   PI_UDS_SNAPISLOCALFAIL     = -11204,	/* PI-UDS could not determine whether to cache snapshot values from foreign system */
   PI_UDS_SNAPSSIGNUP	      = -11205,	/* PI-UDS could not signup for updates with foreign system */
   PI_UDS_SNAPSUPDATESFAIL    = -11206,	/* PI-UDS could not get updates from foreign system */
   PI_UDS_SNAPPUTTIMEDVALUEFAIL= -11207,/* PI-UDS could not write snapshots to foreign system */
   PI_UDS_ADDCONNECTORFAIL    = -11208,	/* PI-UDS could not load connector or set UDS system name*/

   /* -11399 <= PI_V_ <= -11300,  PI value/variant errors */
   PI_V_NOANNOTATION         = -11300,  /* annotation not found in PIvalue*/
   PI_V_ANNOTMISMATCH        = -11301,  /* annotation mismatch */
   PI_V_ANNOTEXIST           = -11302,  /* annotation already exist */
   PI_V_ANNOTTOOLONG         = -11303,  /* annotation exceeds size limit */
   PI_V_INVALID_VARIANT_TYPE = -11304,  /* bad variant type code */

   /* offline archive related errors */
   /* -11999 <= PI_OFFL_ <= -11900, */
   PI_OFFL_BADOPENFILE  = -11901, /* error open file */
   PI_OFFL_BADRECNO     = -11902, /* Bad record number for offline input */
   PI_OFFL_BADIDCONV    = -11903, /* Bad Id conversion table */
   PI_OFFL_BADFILEREAD  = -11904, /* failed to read input file (PI2) */
   PI_OFFL_NOTINIDCONV  = -11905, /* Point ID not found in ID conversion table */
   PI_OFFL_PTIDMM       = -11906, /* Point ID mismatch in offline loading */
   PI_OFFL_NOTARGETREC  = -11907, /* Cannot post events, no target record */
   PI_OFFL_INVTIMES     = -11908, /* Invalid Times For Offline loading */
   PI_OFFL_BADARCTYPE   = -11909, /* Invalid archive type for processing */
   PI_OFFL_NOEVENTS     = -11910, /* No events from input file were added to output archive */



   /* PInt and related classes */
   /* -12049 - -12000 */
   PI_TABLEFROZEN  = -12000,   /* PInt is frozen from changes */
   PI_TABLENONAME  = -12001,   /* Name not found in PInt */
   PI_TABLENOCODE  = -12002,   /* Code not found in PInt */
   PI_TABLEDUPNAME = -12003,   /* Name already in use in PInt */
   PI_TABLEDUPCODE = -12004,   /* Code already in use in PInt */
   PI_TABLEINVNAME = -12005,   /* Invalid name for use in PInt */
   PI_TABLEINVSLOT = -12006,   /* Invalid slot for use in PInt */
   PI_TABLEINUSE   = -12007,   /* Table already contains entries */
   PI_TABLELOADMIS = -12008,   /* Count Mismatch on load */
   PI_TABLEFILEUSE = -12009,   /* Underlying File Store in Use */
   PI_TABLEMAXENTRIESEXCEEDED = -12010, /* attempt to activate table larger than allowed */
   PI_TABLENOIDENTIFIER = -12011,       /* Identifier not found in PInttemplate  */
   PI_TABLEINVIDENTIFIER = -12012,      /* Identifier in PInttemplate is not valid */
   PI_TABLENORECORDDEFINITION = -12013, /* Record definition of generic table is empty */
   /* PIarg and related classes */
   /* -12099 - -12050 */
   PI_ARG_NOOWNLIST   = -12050,	/* Arglist is not owned */
   PI_ARG_OWNLIST     = -12051,	/* Arglist is owned */  
   PI_ARG_FROZEN      = -12052,	/* Operation invalid on a frozen list */
   PI_ARG_BADMERGE    = -12053,	/* Failed merge */

   /* PIgrid and related classes: see also codes in range -280 to -285 */
   /* -12149 - -12100 */
   PI_GRID_BADSETCOLINF = -12100, /* setcolinfo failed */
   PI_GRID_BADSETNUMROW = -12101, /* setnumrows failed */
   PI_GRID_BADVERSION   = -12102, /* version mismatch in activate */
   PI_GRID_BADSETNUMCOL = -12103, /* setnumcols failed */
   
   /* Update mannger and upduser */
   /* -12199 - 12150             */
   PI_UPD_NOTREG    = -12150, /* not registered in updmgr */ 
   PI_UPD_NOCONS    = -12151, /* consumer not registered in updmgr */ 
   PI_UPD_NOPROD    = -12152, /* producer not registered in updmgr */ 
   PI_UPD_MISMATCH  = -12153, /* id mismatch */ 
   
   /* License manager errors */
   /* -12200 - -12249 */
   PILIC_NOLICFILE	= -12200, /* no license file */
   PILIC_ERROPENFILE    = -12201, /* error open license file */
   PILIC_BADKEY		= -12202, /* invalid license key */
   PILIC_INVSPECS	= -12203, /* invalid license specs */
   PILIC_NOSUCHLIC	= -12204, /* no such license */
   PILIC_NOTREGISTERED	= -12205, /* user not registered */
   PILIC_LICEXCEDED	= -12206, /* usage exceeded licensed amount */
   PILIC_LICEXPIRED	= -12207, /* license expired */
   PILIC_BADUSERKEY	= -12208, /* user mismatch */
   PILIC_MISMATCH	= -12209, /* no license file */
   PILIC_NOLICFILE10	= -12210, /* no license file */
   PILIC_SERVIDFILEEXISTS = -12211, /* Server ID file creation failure; file already exists. */
   PILIC_SDKCONNECTIONS = -12212,   /* Maximum licensed SDK Application connections exceeded. Connection refused. */
   PILIC_APICONNECTIONS = -12213,   /* Maximum licensed API Application connections exceeded. Connection refused. */
   PILIC_POINTCOUNT = -12214,       /* Maximum licensed Point Count exceeded. */
   PILIC_MODULECOUNT = -12215,       /* Maximum licensed Module Count exceeded. */
   PILIC_POINTMODULECOUNT = -12216,       /* Maximum licensed aggregate Point/Module Count exceeded. */
   PILIC_BDB = -12217,       /* Not licensed to use Batch Database. */
   PILIC_MDB = -12218,       /* Not licensed to use Module Database. */
   PILIC_COMCONNECTORS = -12219, /* Not licensed to use COM Connector-mapped points. */
   PILIC_SERVERAPP = -12220, /* Not licensed to use this server or UDS application. */
   PILIC_CLIENTAPP = -12221, /* Not licensed to use this client application. */
   PILIC_MAXHISTORY = -12222, /* Not licensed to access archive for passed dates. */
   
   /*  PI Batch and related classes  */
   /*  -12299 - -12250               */
   PI_BA_FIRST                  = -12250,
   PI_BA_DUPACTIVETAG		= -12250,		/* Batch active tag is already being used by another unit */
   PI_BA_BADSIZE			= -12251, 		/* Number of arguments in PIarray is incorrect */
   PI_BA_BADACTIVETAGTYPE	= -12252,		/* Batch Active Tag Type is not STEP or PULSE. */
   PI_BA_UNITARCHIVETAGMISSING = -12253, 	/* _piba_N does not exist */
   PI_BA_WAIT				= -12254,		/* a object is in use wait for it */
   PI_BA_BADSYNTAXBATCHID	= -12255,		/* Syntax of unit's Batch ID expression is invalid */
   PI_BA_BADSYNTAXPRODUCTID = -12256,		/* Syntax of unit's Product ID expression is invalid */
   PI_BA_BADPTBATCHID		= -12257,		/* Tag referenced in unit's Batch ID expression doesn't exist */
   PI_BA_BADPTPRODUCTID		= -12258,		/* Tag referenced in unit's Product ID expression doesn't exist */
   PI_BA_UNSUPPORTEDACTIVETAGTYPE = -12259,	/* The type of the batch active tag is not integer, real, or digital */
   PI_BA_NOBATCHINEVENT = -12260, /* pievent.value did not contain a batch record (PIblob len 0 ) */
   PI_BA_PIBANEXISTS        = -12261,       /* Could not create a unique archive tag name for unit. */
   PI_BA_STARTTIMESTATUSINVALID = -12262,   /* Status of start time for a batch is invalid. */
   PI_BA_STOPTIMESTATUSINVALID = -12263,    /* Status of stop time for a batch is invalid. */
   PI_BA_BATCHEND           = -12264,       /* This is an end of batch event. */
   PI_BA_BATCHSTART         = -12265,       /* This is a start of batch event. */
   PI_BA_TIMENOTFOUND       = -12266,       /* Time was outside boundaries of PIbaIndex Directory. */
   PI_BA_INVALIDINDEX       = -12267,       /* Index for PIbaIndex Directory is invalid. */
   PI_BA_INVALIDTIMESPAN    = -12268,       /* Time span within search contains no indexes. */
   PI_BA_NO_UNITS_MATCHED   = -12269,       /* Unit mask matches none of the current units. */
   PI_BA_NO_END_EVENT_EXISTS = -12270,      /* No end event exists for this batch handle. */
   PI_BA_BATCHOREVENT_EXISTS = -12271,
   PI_BA_INVALIDPIBLOB       = -12272,      /* archived blob is invalid or 0 len */
   PI_BA_CANTDELETE          = -12273,
   PI_BA_NOBATCHES           = -12274,
   PI_BA_INVALIDHANDLE       = -12275,
   PI_BA_LAST                  = -12299,
   
   /*  PI PE and related errors  */
   /* -12300 -12399              */
   PI_PE_ERROR                 = -12300,
   PI_PE_PARSEERROR            = -12301,
   PI_PE_DIVIDEBYZERO          = -12302,
   PI_PE_OVERFLOW              = -12303,
   PI_PE_LEX_FILE_NOT_FOUND    = -12304,
   PI_PE_RULE_FILE_NOT_FOUND   = -12305,

   PI_PE_RESOURCES_NOT_FOUND       = -12306,
   PI_PE_INVALID_CONNECTION        = -12307,
   PI_PE_BAD_TREE_AT_GENCODE       = -12308,   /* impossible production numbers in parse tree */
   PI_PE_BAD_TREE_AT_TYPECHECK     = -12309,   /* impossible production numbers in parse tree */
   PI_PE_FUNC_WRONG_NUMBER_OF_ARGS = -12310,
   PI_PE_FUNC_NEEDS_ONE_ARG        = -12311,
   PI_PE_FUNC_TO_MANY_ARGS         = -12312,
   PI_PE_FUNC_ARG1_NOT_DIGITAL     = -12313,
   PI_PE_FUNC_ARG1_NOT_TIME        = -12314,
   PI_PE_FUNC_BAD_ARG_TYPES        = -12315,
   PI_PE_FUNC_NEEDS_MORE_ARGS      = -12316,  /* wrong number of arguments for function */
   PI_PE_FUNC_ARG1_INVALID         = -12317,
   PI_PE_FUNC_ARG2_INVALID         = -12318,
   PI_PE_FUNC_ARG3_INVALID         = -12319,
   PI_PE_FUNC_ARG4_INVALID         = -12320,
   PI_PE_FUNC_ARGS_NOT_SAME_TYPE   = -12321,
   PI_PE_FUNC_ARGS_NOT_NUMBERS     = -12322,
   PI_PE_TOO_MANY_ARGS             = -12323,
   PI_PE_ADD_CONNECT_FAILED        = -12324,
   PI_PE_TOO_MANY_CONNECTS         = -12325,
   PI_PE_PCT_GOOD_TOO_SMALL        = -12330, /* Percent good is below requested threshold */
   PI_PE_E31         = -12331,
   PI_PE_E32         = -12332,
   PI_PE_E33         = -12333,
   PI_PE_E34         = -12334,
   PI_PE_E35         = -12335,
   PI_PE_E36         = -12336,
   PI_PE_E37         = -12337,
   PI_PE_E38         = -12338,
   PI_PE_E39         = -12339,
   PI_PE_E40         = -12340,
   PI_PE_E41         = -12341,
   PI_PE_E42         = -12342,
   PI_PE_E43         = -12343,
   PI_PE_E44         = -12344,
   PI_PE_E45         = -12345,
   PI_PE_E46         = -12346,
   PI_PE_E47         = -12347,
   PI_PE_E48         = -12348,
   PI_PE_E49         = -12349,

   PI_UPD_SAVE_FILENOTFOUND = -12380,

   /* PI Message errors */
   PI_MSG_BADQUERY         = -13000, /* GetMessages query is bad */
   PI_MSG_BADMAXCOUNT      = -13001, /* GetMessages query bad MaxCount */
   PI_MSG_BADSTARTTIME     = -13002, /* GetMessages query bad StartTime */
   PI_MSG_BADENDTIME       = -13003, /* GetMessages query bad EndTime */
   PI_MSG_BADMESSAGEID     = -13004, /* GetMessages query bad MessageID */
   PI_MSG_BADUSER          = -13005, /* GetMessages query bad MaxCount */
   PI_MSG_BADSEARCHSTRING  = -13006, /* GetMessages query bad MessageSearchString */
   PI_MSG_BADOPTION1       = -13007, /* GetMessages query has no MaxCount or EndTime */
   PI_MSG_BADOPTION2       = -13008, /* GetMessages query has no MaxCount or StartTime */
   PI_MSG_BADOPTION3       = -13009, /* GetMessages query has no StartTime or EndTime */
   PI_MSG_NAMEMISMATCH     = -13010, /* GetMessages query has unknown keyword */

   /* Audit Errors */
   PI_AUD_FNF              = -13050, /* Cannot find audit file */
   PI_AUD_CREFAIL          = -13051, /* Cannot create audit file */
   PI_AUD_BCKUPINPROG      = -13052, /* Audit disabled during backup */
   PI_AUD_WRITEERR         = -13053, /* Failed to write audit record */ 

   /* PI Message ( NT Event Log ) errors */
   PI_NTLOG_NOHANDLE          = -13100, /* No NT Application Event Log Handle */
   PI_NTLOG_NOUPDATE          = -13101, /* Unable to get updates from NT App Log */
   PI_NTLOG_NOSENDTOPI        = -13102, /* Unable to send event log msgs to PI */
   PI_NTLOG_BADGETREGVAL      = -13103, /* Unable to get values for pimsgss service registry key */
   PI_NTLOG_NOREGKEY          = -13104, /* Unable to get registry key for service pimsgss */

   /* PI Counter errors */
   PI_CTR_BADPERFINFO         = -13200, /* PIPerfInfo struct is bad */
   PI_CTR_BADGETPERFREG       = -13201, /* Unable to get Perflib registration info */
   PI_CTR_BADSETPERFREG       = -13202, /* Unable to set Perflib registration info */
   PI_CTR_BADGETPIREG         = -13203, /* Unable to get PI performance registration info */
   PI_CTR_BADSETPIREG         = -13204, /* Unable to set PI performance registration info */
   PI_CTR_BADPERFLIBNUM       = -13205, /* Perflib Last Counter larger than Last Help*/
   PI_CTR_ODDPERFLIB          = -13206, /* Perflib Last Counter is odd number or Last Help is even number*/
   PI_CTR_BADPERFLIBSET       = -13207, /* Perflib Last Help is greater than Last Counter by more than one*/

   /* PI Test errors */
   PI_TST_NOTFOUND         = -13250, /* Test not found in DB */
   PI_TST_DBERROR          = -13251, /* Cannot record reuslts in test DB */
   PI_TST_MAILERROR        = -13252, /* Cannot Email test results */
   PI_TST_NOPISYS          = -13253, /* Test requires running PI system */
   PI_TST_INVATR           = -13254, /* Invalid test attribute */
   PI_TST_FAILED           = -13255, /* Test failed */
   
   /* */
   PI_ISTREAMFAIL     = -15000,	/* istream::get failure */
   PI_OSTREAMFAIL     = -15001,	/* ostream:: failure */
   PI_BADOFFSET       = -15002,	/* Generic Out-of-bounds (PIstring, PIarray) */
   PI_NOTFOUND        = -15003,	/* Element not found (PIarray) */
   PI_OVERFLOW        = -15010,	/* PIvalue cannot represent number - too big */
   PI_NOTANUMBER      = -15011,	/* PIvalue type is not numeric */
   PI_INFINITY        = -15012,	/* PIvalue divide by zero */
   PI_NOTAFLOAT       = -15013,	/* PIvalue type or PIstring is not float */
   PI_NOTANINTEGER    = -15014,	/* PIvalue type or PIstring is not an integer */
   PI_BADFLOATFORMAT  = -15015,	/* Number of digits or decimals out of range*/
   PI_NOTASTRING      = -15016,	/* PIvalue type is not a string */
   PI_WRONGVALTYPE    = -15017,	/* PIvalue type not allowed for this call */
   PI_NOTABLOB        = -15018,	/* PIvalue type is not a blob */
   PI_NOTAVALUETYPE   = -15019,	/* value type is not a valid PIvalue type */
   PI_INVALIDPATH     = -15020, /* Invalid Path Specified */
   PI_BADTARGET       = -15021, /* Context Sensitive Bad Target Error */
   PI_BADINITIALIZE   = -15022, /* Context Sensitive Initialization Failure */
   PI_BADVERSION      = -15023, /* Generic Bad Version Failure */
   PI_DUPLICATE       = -15024, /* Generic duplicate name */
   PI_BADEVENTMODE    = -15025, /* Invalid or Unsupported PIevent Mode */
   PI_MAXLENGTHEXCEEDED = -15026, /* Attempt to activate or create a PIstring or PIblob larger than max allowable */
   PI_LOGMESSAGESTHROTTLED = -15027, /* Excessive messages to PImsgss, throttled */
   PI_INVALIDBOOKMARK      = -15028, /* Invalid or corrupt book mark  */
   PI_INVALID          = -15029, /* generic invalid call or argument*/
   PI_MISMATCH         = -15030, /* generic mismatch */
   PI_NOTADIG          = -15031, /* PIvalue type is not digital */
   PI_PATHNOTFOUND     = -15032, /* Requested path not found */
   PI_BCKUPINPROG      = -15033, /* Cannot perform operation during backup */
   PI_INVALIDTZCONFIG  = -15034, /* TimeZone configuration is invalid */
   PI_MAXARRLENGTHEXCEEDED = -15035, /* Attempt to activate or create a PIarray larger than max allowable */
   PI_UNABLETORETRIEVEUNIXERRNO = -15036, /* A call on UNIX fails but errno is zero (which implies good status) */

   /*  PIversion/equipment and batch sdk errors */
   PI_INVALIDEFFECTIVEDATE = -16000,  /* Object not found for passed effective date */
   PI_MODULENOTFOUND = -16001,        /* Module does not exist. */
   PI_INVALIDMODULEVERSION = -16002,   /* Invalid or missing module version. */
   PI_DUPLICATEEFFECTIVEDATE = -16003, /* Value with passed effective date already exists. */
   PI_LASTMODULEVALUE = -16004,        /* Cannot remove last module value; use module remove. */
   PI_ROOTMODULE = -16005,             /* Attempt to remove or edit a Built in module element.*/
   PI_MODULEHIERARCHYBREAK = -16006,   /* Attempt to delete or remove a module that breaks existing hierarchy.*/
   PI_UNEXPECTEDMODULEDBERROR = -16007, /* Unexpected PI Module Database error. */
   PI_MODULEVALUEEXISTS = -16008,  /* Effective date already exists for attempt add or move of a module value. */
   PI_INVALIDPARENT = -16009,      /* Invalid parent specified for the operation. */
   PI_DUPLICATEHIERARCHY = -16010, /* Attempt to create or edit object with duplicate hierarchical level */
   PI_INVALIDHIERARCHY = -16011,   /* Attempt to create or edit object with invalide hierarchical level */
   PI_NOPARENTREFERENCE = -16012, /* Module does not have parent reference to specified module */
   PI_NOCHILDREFERENCE = -16013,  /* Module does not have child reference to specified module */
   PI_INVALIDQUERYDATE = -16014,  /* Invalid or unspecified query date. */
   PI_INVALIDUID = -16015,        /* Invalid or unspecified uid. */
   PI_INVALIDMODE = -16016,        /* Invalid or unspecified module value access mode. */
   PI_MODULEVALUENOTFOUND = -16017, /* Module Value for passed effective date not found.*/
   PI_INVALIDHEADING = -16018, /* Specified heading does not exist or is member of different heading set */
   PI_INVALIDTIMERANGE = -16019, /* Invalid or unspecified time range for batch database search. */
   PI_NOMATCHINGMODULES = -16020, /* No matching PIModules for call. */
   PI_NOMATCHINGBDBRECORDS = -16021, /* No Matching Batch Database records. */
   PI_MDBNOTSUPPORTED = -16022, /* Attempted operation not supported by the specified database.*/
   PI_MDBCIRCULARREFERENCE = -16023, /* Attempt to insert a PIModule that would cause a circular reference. */
   PI_MDBNOMATCHINGVALUES = -16024, /* No module values within the passed time range. */
   PI_MDBLASTVALUE = -16025, /* Attempt to remove the last module value */
   PI_BDBCROSSREFERENCE = -16026, /* Attempt to remove a Batch Database Record with cross reference to another record */
   PI_BDBBSSNOTSUPPORTED = -16027, /* Batch Database does not support this action with Batch Subsystem Batch. */ 
   PI_INVALIDBDBTIME = -16028, /* Invalid start or end time for Batch Database Record. */
   PI_LASTVERSION = -16200,

   /* thread/thread pool errors */
   PI_THREADPOOLDISABLED = -16201, /* Subsystem does not support thread pool or thread pool is disabled. */
   PI_THREADSUSPENDED = -16202, /* Thread is in suspended state */
   PI_INVALIDTHREADID = -16203, /* Passed thread id is invalid. */   
   PI_THREADTIMEOUT = -16204, /* Time out waiting for semaphore or lock. */
   PI_THREADABANDONED = -16205, /* Thread or handle has been abandoned. */
   PI_THREADUNLOCK = -16206, /* Attempt to release exclusive lock from thread that did not acquire the lock. */

#ifdef _DOS /* for 16 bit compiler */
   PI_WARNING         = -30000,        /* -32768 <= code <= -30000 are warnings */
#else
   PI_WARNING         = -30000,        /* -32768 <= code <= -30000 are warnings */
   
   PISTAT_FOR_COMPATIBILITY_WITH_C = -32769,   /* force 4-byte representation */
#endif

   PI_PD_IGNORE      = -30100,	/* Point Attribute not used */
   PI_PD_OPERATIONFAILED = -30101,       /* One or more point sdk operations failed */
   PI_NODATA         = -30200,	 /* generic Nothing Found warning */
   PI_TMPUNAVAIL     = -30201,   /* generic temporary unavailable - try later */
   PI_UPD_OVERFLOW   = -30250,    /* Update queue overflow, some events missing */ 
   PI_CTR_NOINSTALL  = -30300,    /* Perflib registry is bad, will not install counters */
   PI_CTR_DONOTINSTALLCOUNTERS = -30301, /* Performance counters chosen not to be installed */

   PI_AR_DUPTIMESTAMPS = -30400 /* Last event in collection has same timestamp as next event */
};


#define PI_UDS_UNAVAIL   PI_3PHUNAVAIL

#ifdef __cplusplus
private:

   int32 code;

   PIstring lookupPImessage( void ) const;

public:

   PIstatus (int32 newcode = PI_NORMAL) 
   {
      code = newcode;
#ifdef _DEBUG
      _bTrace = false;
      memset ( _strOwner, 0, sizeof(_strOwner) );
#endif
   };
   PIstatus (const PIstatus &newstatus ) 
   {
      code = newstatus.code;
#ifdef _DEBUG
      _bTrace = newstatus._bTrace;
      strncpy ( _strOwner, newstatus._strOwner, sizeof(_strOwner) );
#endif
   };
   ~PIstatus () 
   { 
#ifdef _DEBUG
      if ( _bTrace )
      {
#ifdef WIN32
         std::cout << "~PIstatus " << this << ". Thread " << GetCurrentThreadId() << ". " <<
            _strOwner << std::endl;
#else
         std::cout << "~PIstatus " << this << ". " << _strOwner << std::endl;
#endif
      }
#endif
   };
   PIstring str () const;
   PIstatus &operator= (int32 newcode)
   {
      code = newcode;
      return *this;
   };
   PIstatus &operator= (const PIstatus &newstatus)
   {
      code = newstatus.code;
      return *this;
   };
   inline int operator== (PIstatus stat) { return (code == stat.code); };
   inline int operator== (int32 newcode) { return (code == newcode); };
   inline friend int operator== (int32 acode, PIstatus stat)
       { return (acode == stat.code); };
   inline int operator!= (PIstatus stat) { return (code != stat.code); };
   inline int operator!= (int32 newcode) { return (code != newcode); };
   inline friend int operator!= (int32 acode, PIstatus stat)
       { return (acode != stat.code); };
   inline void setcode (int32 newcode) { code = newcode; };
   inline int32 getcode (void) const { return (code); };
   inline pibool isgood (void) const { return (code == PI_NORMAL); };
   inline pibool isbad (void) const
   {
      return (code != PI_NORMAL && code > PI_WARNING);
   };
   inline pibool iswarning (void) const { return (code <= PI_WARNING); };
   PIstatus activate( uint8 *Buffer, int32 len, int32 &position );
   PIstatus activate( std::istream& is );
   void dump( std::ostream& os, int& depth ) const;
   PIstatus passivate( std::ostream& os ) const;
   void setsyserror ( void );
#ifdef _DEBUG
   pibool _bTrace;
   char  _strOwner[32];  // small buffer for owner info
#endif
};

/* function prototypes for C functions */

extern "C"
{
#endif /* __cplusplus */
typedef int32 pistatus; 
PIUTSHR_API void piut_msg (pistatus stat, TCHAR *pmsg, size_t *plength);
PIUTSHR_API pibool piut_isgood (pistatus stat);
PIUTSHR_API pibool piut_isbad (pistatus stat);
PIUTSHR_API pibool piut_iswarning (pistatus stat);
PIUTSHR_API void piut_ForceCrash ( TCHAR *pmsg );
#ifdef __cplusplus
}
#define PIRETURNIFBAD(x){ if (x.isbad() ) return x; }
#define PIRETURNIFNOTBAD(x){ if (!(x.isbad()) ) return x; }
#define PIRETURNIFGOOD(x){ if (x.isgood() ) return x; }
#define PIRETURNIFNOTGOOD(x){ if ( !(x.isgood()) ) return x; }
#define PIBREAKIFBAD(x){ if (x.isbad() ) break; }
#define PIBREAKIFNOTBAD(x){ if (!(x.isbad()) ) break; }
#define PIBREAKIFGOOD(x){ if (x.isgood() ) break; }
#define PIBREAKIFNOTGOOD(x){ if ( !(x.isgood()) ) break; }
#endif /* __cplusplus */


/*----- Version Support ------------------------------------------------------*/
#ifdef __cplusplus
enum PIvershelpdisplay
{
   justreturn,
   displayandexit,
   displayandreturn
};

enum PIversionconstant
{
   PIvc_PIDATAARCHIVEVERSIONSTR = 0,
   PIvc_PIDATAARCHIVEBUILD,
   PIvc_PIDATAARCHIVEMAJORVER,
   PIvc_PIDATAARCHIVEMINORVER,
   PIvc_PIVersionNumber,
   PIvc_PISystemVersion,
   PIvc_PISystemBuild,
   PIvc_SccsDelta,
   PIvc_SccsCurrent
};

/*----- piut_parseversioninfo --------------------------------------------------*/
PIUTSHR_API const TCHAR* piut_parseversioninfo( int argc, char* argv[], PIvershelpdisplay showit = justreturn );
PIUTSHR_API const pibool piut_IsUsageRequest ( int argc, char** argv, void(*usagefunc)(void)=0, 
                                         PIvershelpdisplay showit = justreturn );
PIUTSHR_API const TCHAR* piut_GetVersionConstant( PIversionconstant vc );
#endif /* __cplusplus */

#endif /* PICL_PISTATUS */
