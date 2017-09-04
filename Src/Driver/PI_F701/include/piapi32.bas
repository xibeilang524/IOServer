Attribute VB_Name = "Module1"
' $Archive: /PI/clients/piapi/piapi32.bas $
'Copyright (c) 1993-98 OSI Software, Inc. All rights reserved.
'Unpublished - rights reserved under the copyright law of the United States.
'
'USE OF A COPYRIGHT NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
'OR DISCLOSURE.
'
'THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE SECRETS OF
'OSI SOFTWARE, INC.  USE, DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT
'THE PRIOR EXPRESS WRITTEN PERMISSION OF OSI SOFTWARE, INC.
'
'RESTRICTED RIGHTS LEGEND
'Use, duplication, or disclosure by the Government is subject to restrictions
'as set forth in subparagraph (c)(1)(ii) of the Rights in Technical Data and
'Computer Software clause at DFARS 252.227.7013
'
'OSI Software, Inc.
'777 Davis Street, Suite 250, San Leandro CA 94577
'
' $Log: /PI/clients/piapi/piapi32.bas $
' 
' 17    2/26/03 9:31a Matzen
' Added function prototype for get_utctime
' 
' 16    2/19/03 2:48p Matzen
' The prototype for piut_getloginuser was changed to return a long
' instead of a string.  Although visual basic strings can be returned as
' parameters in function calls (as long as they are declared as Byval
' String), visual basic strings cannot directly be returned by a function
' writen in C or C++ unless the C or C++ function returns a BSTR.   Once
' must treat function returns of char*  as long in visual basic and then
' manipulate the long using win32 calls.  See clientele call 52201GMBH.
'
' 15    4/16/02 11:34a Harry
' Changed case of PITime in method definitions.  Screws up VB projects
' that include it and want to use PISDK PITime.
Rem
Rem 14    8/10/99 2:27p Cahenze
Rem Added piut_getloginuser
Rem
Rem 13    7/27/99 5:29p Cahenze
Rem drVals were #, not Any.
Rem
Rem 12    5/13/99 11:19a Cahenze
Rem Added piut_strerror
Rem
Rem 11    4/28/99 8:08a Charlie
Rem Added archive retrieval flags which were missing.  Added missing $ type
Rem in pilg_addnewserver.  Removed Byval declaration in piut_errormsg for
Rem msglen.
Rem
Rem 10    2/15/99 5:45p Charlie
Rem pisn_evmexceptx: first argument changed to byref.
Rem
Rem 9     2/08/99 1:09p Charlie
Rem Added pilg_checklog, pilg_formputlog, pisn_evmexceptx,
Rem pisn_sendexcepstrucq, piut_fastserverversion.
Rem
Rem 8     12/03/98 5:07p Charlie
Rem Removed unusable PIvaluetypes.  Modified PITIMESTAMP.reserved to
Rem PITIMESTAMP.tzinfo.  Added piar_putarcvaluesx and mode definitions,
Rem pitm_isdst, pitm_setdst and piut_getserverversion.
Rem
Rem 7     5/15/98 11:35a Harry
Rem Removed line
Rem Attribute VB_Name = "Module1"
Rem Conflicted with other modules when loading into VB projects and not
Rem required.  Leaving it out causes the module to take the next available
Rem name.
Rem
Rem 6     4/30/98 10:19a Charlie
Rem
Rem 4     10/29/97 7:45a Harry
Rem Added defined constants for extended API, GETSAME, PI_NOMOREVALUES, and
Rem BITFLAGS
'
' History
' 11/22/96 hs  Added and fixed extended api declarations and types
Option Explicit

'------------------------------- piar_summary definitions
Global Const ARCTOTAL = 0
Global Const ARCMINIMUM = 1
Global Const ARCMAXIMUM = 2
Global Const ARCSTDEV = 3
Global Const ARCRANGE = 4
Global Const ARCAVERAGE = 5
Global Const ARCMEAN = 6

'------------------------------- piar_value definitions
Global Const ARCVALUEBEFORE = 1
Global Const ARCVALUEAFTER = 2
Global Const ARCVALUEINTERP = 3
Global Const ARCVALUECODE = 4

'------------------------------- piel_evntactn definitions
Global Const ELEQSEARCH = 1
Global Const ELGESEARCH = 2
Global Const ELLESEARCH = 3
Global Const ELGTSEARCH = 4
Global Const ELLTSEARCH = 5
Global Const ELEDIT = 6
Global Const ELDELETE = 7

'------------------------------- pilg_putoutput definitions
Global Const MSGLOG = 1
Global Const STDOUTPUT = 2

'------------------------------- pipt_updates definitions
Global Const NOTAGS = 0
Global Const POINTCREATE = 1
Global Const POINTEDIT = 2
Global Const POINTDELETE = 3

'------------------------------- piut_login definitions
Global Const PILOGINOK = 1
Global Const PIREADACCESS = 2
Global Const PIWRITEACCESS = 4
Global Const PILOGINREAD = 8
Global Const PILOGINWRITE = 16
Global Const PILOGINPIUSER = 256
Global Const PILOGINVMS = 512
Global Const PILOGINUNIX = 1024
Global Const PINO = 0
Global Const PIREAD = 1
Global Const PIREADWRITE = 2
'-------------------------------- pilg definitions
Global Const GETFIRST = 0
Global Const GETNEXT = 1
Global Const GETSAME = 2
Global Const PI_NOMOREVALUES = 100
Global Const PI_M_AFLAG = 1
Global Const PI_M_QFLAG = 2
Global Const PI_M_SFLAG = 4
Global Const MAX_POINT_NUM_LEN = 16
Global Const MAX_NODENAME_LEN = 30
Global Const MAX_TAGNAME_LEN = 256
Global Const MAX_USERNAME_LEN = 30
Global Const PILOGIN_SERVER_TYPE_PI = 1000
'-------------------------------- extended API definitions
Global Const ARCflag_time = 4
Global Const ARCFLAG_EVEN = 8
Global Const ARCflag_filter = 16
Global Const ARCflag_mark = 32
Global Const ARCFLAG_COMP = 64

Global Const PI_Type_null = 0
Global Const PI_Type_bool = 1
Global Const PI_Type_uint8 = 2
Global Const PI_Type_int8 = 3
Global Const PI_Type_char = 4
Global Const PI_Type_uint16 = 5
Global Const PI_Type_int16 = 6
Global Const PI_Type_uint32 = 7
Global Const PI_Type_int32 = 8
Global Const PI_Type_uint64 = 9
Global Const PI_Type_int64 = 10
Global Const PI_Type_float16 = 11
Global Const PI_Type_float32 = 12
Global Const PI_Type_float64 = 13
Global Const PI_Type_PI2 = 14
Global Const PI_Type_digital = 101
Global Const PI_Type_blob = 102
Global Const PI_Type_PIstring = 105
Global Const PI_Type_bad = 255

' piar_putarcvaluesx defines
Global Const ARCNOREPLACE = 3   ' add unless event(s) exist at same time (PI 2.x)
Global Const ARCAPPEND = 4      ' add event regardless of existing events
Global Const ARCREPLACE = 5     '  add event, replace if event at same time
Global Const ARCREPLACEX = 6    '  replace existing event (fail if no event at time)
Global Const ARCDELETE = 7      '  remove existing event
Global Const ARCAPPENDX = 8     '  add event regardless of existing events, no compression

'---------------------------- Structure declarations
Type QERROR
   Point As Long
   piapierror As Long
End Type

Global Const MAXPI3PUTSNAP = 255 'For queue call return values.
Type QERRORS
   syserror As Long
   numpterrs As Long
   qerr(MAXPI3PUTSNAP) As QERROR
End Type

Type PI_EXCEPT
   NewVal As Single
   newstat As Long
   newTime As Long
   oldVal As Single
   oldstat As Long
   oldTime As Long
   prevVal As Single
   prevstat As Long
   prevTime As Long
   ExcDevEng As Single
   ExcMin As Long
   ExcMax As Long
End Type

Type PI_VAL
   bSize As Long
   iStat As Long
   Flags As Integer
End Type

Type TagList
   server As String * MAX_NODENAME_LEN
   NodeID As Long
   TagName As String * MAX_TAGNAME_LEN
   Point As Long
   reserved As Long
End Type

Type PITimeStamp
   Month As Long '    /* 1-12 */
   Year As Long    ' /* four digit */
   Day As Long    '      /* 1-31 */
   Hour As Long    ' /* 0-23 */
   Minute As Long  ' /* 0-59 */
   tzinfo As Long
   Second As Double ' /* 0-59.99999999.... */
End Type

'PI batch function declarations -- New with PI-API v1.1.0
Declare Function piba_getunit Lib "piapi32.dll" (ByVal unit$, ByVal slen&, ByVal index&, number&) As Long
Declare Function piba_getaliaswunit Lib "piapi32.dll" (ByVal unit$, ByVal balias$, ByVal slen&, ByVal index&, number&) As Long
Declare Function piba_getunitswalias Lib "piapi32.dll" (ByVal balias$, ByVal unit$, ByVal slen&, ByVal index&, number&) As Long
Declare Function piba_findaliaspoint Lib "piapi32.dll" (ByVal unit$, ByVal balias$, ptno&, ByVal TagName$, ByVal slen&) As Long
Declare Function piba_search Lib "piapi32.dll" (ByVal batchid$, ByVal blen&, ByVal unit$, ByVal ulen&, ByVal product$, ByVal plen&, stime&, etime&, ByVal sf&, ByVal timeout&) As Long

'PI Login Services function declarations -- new with PI-API v1.1.0
Declare Function pilg_addnewserver Lib "pilog32.dll" (ByVal ServerName$, ByVal servertype&, ByVal username$, ByVal portnum&) As Long
Declare Function pilg_connectdlg Lib "pilog32.dll" (ByVal hwndparent&) As Long
Declare Function pilg_disconnect Lib "pilog32.dll" () As Long
Declare Function pilg_disconnectnode Lib "pilog32.dll" (ByVal ServerName$) As Long
Declare Function pilg_getconnectedserver Lib "pilog32.dll" (ByVal servernamebuf$, bufsize&, NodeID&, port&, ByVal seq&) As Long
Declare Function pilg_getdefserverinfo Lib "pilog32.dll" (ByVal servernamebuf$, bufsize&, NodeID&, port&) As Long
Declare Function pilg_getnodeid Lib "pilog32.dll" (ByVal servernamebuf$, NodeID&) As Long
Declare Function pilg_getselectedtag Lib "pilog32.dll" (taglst As TagList, ByVal seq&) As Long
Declare Function pilg_getservername Lib "pilog32.dll" (ByVal NodeID&, ByVal servernamebuf$, bufsize&) As Long
Declare Function pilg_login Lib "pilog32.dll" (ByVal hwndparent&, ByVal username$, ByVal ServerName$, ByVal password$, valid&) As Long
Declare Function pilg_pointattdlg Lib "pilog32.dll" (ByVal hwndparent&, taglst As TagList) As Long
Declare Function pilg_registerapp Lib "pilog32.dll" (ByVal dllname$) As Long
' The function below requires allocating a linked list which is not supported in VB '
'Declare Function pilg_registerhelp Lib "pilog32.dll" (ByVal helpfile$, linklist&) As Long
Declare Function pilg_setservernode Lib "pilog32.dll" (ByVal ServerName$) As Long
Declare Function pilg_tagsearchdlg Lib "pilog32.dll" (ByVal hwndparent&) As Long
Declare Function pilg_unregisterapp Lib "pilog32.dll" () As Long


'Function declarations
Declare Function piar_calculation Lib "piapi32.dll" (Count&, times&, rvals!, istats&, ByVal Expr$) As Long
Declare Function piar_compvalues Lib "piapi32.dll" (ByVal pt&, Count&, times&, rvals!, istats&, ByVal rev&) As Long
Declare Function piar_compvaluesfil Lib "piapi32.dll" (ByVal pt&, Count&, times&, rvals!, istats&, ByVal Expr$, ByVal rev&, ByVal fil&) As Long
Declare Function piar_deletevalue Lib "piapi32.dll" (ByVal pt&, ByVal timedate&) As Long
Declare Function piar_interpvalues Lib "piapi32.dll" (ByVal pt&, Count&, times&, rvals!, istats&) As Long
Declare Function piar_interpvaluesfil Lib "piapi32.dll" (ByVal pt&, Count&, times&, rvals!, istats&, ByVal Expr$) As Long
Declare Function piar_panvalues Lib "piapi32.dll" (ByVal pt&, Count&, timedate&) As Long
Declare Function piar_plotvalues Lib "piapi32.dll" (ByVal pt&, ByVal intervals&, Count&, times&, rvals!, istats&) As Long
Declare Function piar_putvalue Lib "piapi32.dll" (ByVal pt&, ByVal rval!, ByVal iStat&, ByVal timedate&, ByVal wait&) As Long
Declare Function piar_replacevalue Lib "piapi32.dll" (ByVal pt&, ByVal timedate&, ByVal rval!, ByVal iStat&) As Long
Declare Function piar_summary Lib "piapi32.dll" (ByVal pt&, time1&, time2&, rval!, pctgood!, ByVal code&) As Long
Declare Function piar_timedvalues Lib "piapi32.dll" (ByVal pt&, Count&, times&, rvals!, istats&, ByVal prev&) As Long
Declare Function piar_timedvaluesfil Lib "piapi32.dll" (ByVal pt&, Count&, times&, rvals!, istats&, ByVal Expr$) As Long
Declare Function piar_timefilter Lib "piapi32.dll" (ByVal StartTime&, ByVal EndTime&, ByVal Expr$, tottime&, passtime&) As Long
Declare Function piar_value Lib "piapi32.dll" (ByVal pt&, timedate&, ByVal Mode&, rval!, iStat&) As Long
Declare Function piel_addevnt Lib "piapi32.dll" (PITime&, number&, ByVal group&, ByVal etype&, ByVal Msg$, ByVal timeout&) As Long
Declare Function piel_evntactn Lib "piapi32.dll" (PITime&, number&, group&, etype&, ByVal slen&, ByVal Msg$, ByVal action&, ByVal timeout&) As Long
Declare Function pilg_checklogfile Lib "piapi32.dll" (ByVal action&, ByVal LogFile$) As Long
Declare Function pilg_formputlog Lib "piapi32.dll" (ByVal Msg$, ByVal IdString$) As Long
Declare Function pilg_puthomelog Lib "piapi32.dll" (ByVal Msg$) As Long
Declare Function pilg_putlog Lib "piapi32.dll" (ByVal Msg$) As Long
Declare Function pilg_putoutput Lib "piapi32.dll" (ByVal Msg$, ByVal Flags&) As Long
Declare Function pipt_compspecs Lib "piapi32.dll" (ByVal pt&, compdev&, compmin&, compmax&) As Long
Declare Function pipt_compspecseng Lib "piapi32.dll" (ByVal pt&, compdeveng!, compmin&, compmax&) As Long
Declare Function pipt_dates Lib "piapi32.dll" (ByVal pt&, creationdate&, ByVal creator$, ByVal crlen&, changedate&, ByVal changer$, ByVal chlen&) As Long
Declare Function pipt_descriptor Lib "piapi32.dll" (ByVal pt&, ByVal desc$, ByVal slen&) As Long
Declare Function pipt_digcode Lib "piapi32.dll" (digcode&, ByVal digstring$) As Long
Declare Function pipt_digcodefortag Lib "piapi32.dll" (ByVal pt&, digcode&, ByVal digstring$) As Long
Declare Function pipt_digpointers Lib "piapi32.dll" (ByVal pt&, digcode&, dignumb&) As Long
Declare Function pipt_digstate Lib "piapi32.dll" (ByVal digcode&, ByVal digstate$, ByVal slen&) As Long
Declare Function pipt_displaydigits Lib "piapi32.dll" (ByVal pt&, displaydigits&) As Long
Declare Function pipt_engunitstring Lib "piapi32.dll" (ByVal pt&, ByVal engunitstring$, ByVal slen&) As Long
Declare Function pipt_engunstring Lib "piapi32.dll" (ByVal engunitcode&, ByVal engunitstring$, ByVal slen&) As Long
Declare Function pipt_excspecseng Lib "piapi32.dll" (ByVal pt&, ExcDevEng!, ExcMin&, ExcMax&) As Long
Declare Function pipt_exdesc Lib "piapi32.dll" (ByVal pt&, ByVal exdesc$, ByVal slen&) As Long
Declare Function pipt_findpoint Lib "piapi32.dll" (ByVal TagName$, pt&) As Long
Declare Function pipt_inprocbits Lib "piapi32.dll" (ByVal pt&, larchiving&, lcompressing&, filtercode&) As Long
Declare Function pipt_instrumenttag Lib "piapi32.dll" (ByVal pt&, ByVal instrumenttag$, ByVal slen&) As Long
Declare Function pipt_location Lib "piapi32.dll" (ByVal pt&, location&) As Long
Declare Function pipt_nextptwsource Lib "piapi32.dll" (ByVal source%, pt&) As Long
Declare Function pipt_pointid Lib "piapi32.dll" (ByVal pt&, ipt&) As Long
Declare Function pipt_pointsource Lib "piapi32.dll" (ByVal pt&, ByVal source$) As Long
Declare Function pipt_pointtype Lib "piapi32.dll" (ByVal pt&, ByVal PtType$) As Long
Declare Function pipt_ptexist Lib "piapi32.dll" (ByVal pt&) As Long
Declare Function pipt_recordtype Lib "piapi32.dll" (ByVal pt&, steps&) As Long
Declare Function pipt_rescode Lib "piapi32.dll" (ByVal pt&, rescode&) As Long
Declare Function pipt_scale Lib "piapi32.dll" (ByVal pt&, zero!, span!) As Long
Declare Function pipt_scan Lib "piapi32.dll" (ByVal pt&, lscan&) As Long
Declare Function pipt_signupforupdates Lib "piapi32.dll" () As Long
Declare Function pipt_sourcept Lib "piapi32.dll" (ByVal pt&, sourcept&) As Long
Declare Function pipt_squareroot Lib "piapi32.dll" (ByVal pt&, squareroot&) As Long
Declare Function pipt_tag Lib "piapi32.dll" (ByVal pt&, ByVal tag$, ByVal slen&) As Long
Declare Function pipt_taglong Lib "piapi32.dll" (ByVal pt&, ByVal tag$, ByVal slen&) As Long
Declare Function pipt_tagpreferred Lib "piapi32.dll" (ByVal pt&, ByVal tag$, ByVal slen&) As Long
Declare Function pipt_totalspecs Lib "piapi32.dll" (ByVal pt&, totalcode&, convers!) As Long
Declare Function pipt_typicalvalue Lib "piapi32.dll" (ByVal pt&, typicalvalue!) As Long
Declare Function pipt_updates Lib "piapi32.dll" (pt&, ByVal TagName$, ByVal slen&, Mode&) As Long
Declare Function pipt_userattribs Lib "piapi32.dll" (ByVal pt&, userint1&, userint2&, userreal1!, userreal2!) As Long
Declare Function pipt_wildcardsearch Lib "piapi32.dll" (ByVal tagmask$, ByVal direction&, found&, ByVal TagName$, ByVal slen&, pt&, numfound&) As Long
Declare Function pisn_evmdisestablish Lib "piapi32.dll" (Count&, pts&) As Long
Declare Function pisn_evmestablish Lib "piapi32.dll" (Count&, pts&) As Long
Declare Function pisn_evmexceptions Lib "piapi32.dll" (Count&, pt&, rval!, iStat&, timedate&) As Long
Declare Function pisn_getsnapshot Lib "piapi32.dll" (ByVal pt&, rval!, iStat&, timedate&) As Long
Declare Function pisn_getsnapshots Lib "piapi32.dll" (pt&, rval!, iStat&, timedate&, piapierror&, ByVal Count&) As Long
Declare Function pisn_putsnapshot Lib "piapi32.dll" (ByVal pt&, ByVal rval!, ByVal iStat&, ByVal timedate&) As Long
Declare Function pisn_putsnapshots Lib "piapi32.dll" (pt&, rval!, iStat&, timedate&, piapierror&, ByVal Count&) As Long
Declare Function pisn_sendexcepstruc Lib "piapi32.dll" (ByVal pt&, ByVal PtType%, except As PI_EXCEPT, Count&) As Long
Declare Function pisn_sendexceptions Lib "piapi32.dll" (ByVal pt&, ByVal PtType%, oldVal!, oldstat%, oldTime&, prevVal!, prevstat%, prevTime&, ByVal NewVal!, ByVal newstat%, ByVal newTime&, ByVal ExcDevEng!, ByVal ExcMin%, ByVal ExcMax%, Count&) As Long
Declare Function pisn_sendexcepstrucq Lib "piapi32.dll" (ByVal pt&, ByVal PtType%, except As PI_EXCEPT, Count&, ByVal Queueing%, numbptErrs&, qErrs As QERROR) As Long
'----- these functions have return types incompatible with Visual Basic
'Declare Function pisn_sendexceptionq Lib "piapi32.dll" (ByVal pt&, ByVal pttype%, oldval!, oldstat%, oldtime&, prevval!, prevstat%, prevtime&, ByVal newval!, ByVal newstat%, ByVal newtime&, ByVal excdeveng!, ByVal excmin%, ByVal excmax%, count&) As QERRORS
'Declare Function pisn_putsnapshotq Lib "piapi32.dll" (ByVal pt&, ByVal rval!, ByVal istat&, ByVal timedate&) As QERRORS
'Declare Function pisn_flushputsnapq Lib "piapi32.dll" () As QERRORS
Declare Function pitm_delay Lib "piapi32.dll" (ByVal mseconds&) As Long
Declare Function pitm_fastservertime Lib "piapi32.dll" () As Long
Declare Sub pitm_formtime Lib "piapi32.dll" (ByVal timedate&, ByVal timestring$, ByVal slen&)
Declare Sub pitm_intsec Lib "piapi32.dll" (timedate&, timearray&)
Declare Function pitm_parsetime Lib "piapi32.dll" (ByVal timestr$, ByVal reltime&, timedate&) As Long
Declare Sub pitm_secint Lib "piapi32.dll" (ByVal timedate&, timearray&)
Declare Function pitm_servertime Lib "piapi32.dll" (servertime&) As Long
Declare Function pitm_syncwithservertime Lib "piapi32.dll" () As Long
Declare Function pitm_systime Lib "piapi32.dll" () As Long
Declare Function piut_connect Lib "piapi32.dll" (ByVal ProcName$) As Long
Declare Function piut_disconnect Lib "piapi32.dll" () As Long
Declare Function piut_disconnectnode Lib "piapi32.dll" (ByVal SrvName$) As Long
Declare Function piut_fastserverversion Lib "piapi32.dll" (MinorVer&, ByVal buildid$, ByVal BuildLen&) As Long
Declare Function piut_getapiversion Lib "piapi32.dll" (ByVal version$, ByVal slen&) As Long
Declare Function piut_getloginuser Lib "piapi32.dll" () As Long
Declare Sub piut_getprotocolvers Lib "piapi32.dll" (ByVal Vers$, ByVal slen&)
Declare Function piut_getserverversion Lib "piapi32.dll" (NodeID&, ByVal ServerName$, ByVal servernamelen&, ByVal version$, ByVal versionlen&, ByVal buildid$, ByVal buildidlen&) As Long
Declare Sub piut_inceventcounter Lib "piapi32.dll" (ByVal counter&, ByVal Count&)
Declare Function piut_ishome Lib "piapi32.dll" () As Long
Declare Function piut_login Lib "piapi32.dll" (ByVal username$, ByVal password$, valid&) As Long
Declare Function piut_netinfo Lib "piapi32.dll" (ByVal namestr$, ByVal NameLen&, ByVal addressstr$, ByVal addresslen&, ByVal typestr$, ByVal typelen&) As Long
Declare Function piut_netnodeinfo Lib "piapi32.dll" (ByVal namestr$, ByVal NameLen&, ByVal addressstr$, ByVal addresslen&, connected&) As Long
Declare Function piut_netserverinfo Lib "piapi32.dll" (ByVal namestr$, ByVal NameLen&, ByVal addressstr$, ByVal addresslen&, connected&) As Long
Declare Function piut_setdefaultservernode Lib "piapi32.dll" (ByVal ServerName$) As Long
Declare Sub piut_setprocname Lib "piapi32.dll" (ByVal ProcName$)
Declare Function piut_setservernode Lib "piapi32.dll" (ByVal ServerName$) As Long
Declare Sub piut_zeroeventcounter Lib "piapi32.dll" (ByVal counter&)

Declare Function piar_getarcvaluesx Lib "piapi32.dll" (ByVal PtNum&, ByVal arcMode&, Count&, drVal As Any, iVal&, bVal As Any, bSize&, iStat&, Flags%, time0 As PITimeStamp, time1 As PITimeStamp, ByVal FuncCode&) As Long
Declare Function piar_getarcvaluesfilterx Lib "piapi32.dll" (ByVal PtNum&, ByVal Mode&, Count&, drVal As Any, iVal&, bVal As Any, bSize&, iStat&, Flags%, time0 As PITimeStamp, time1 As PITimeStamp, ByVal Expr$, ByVal FuncCode&) As Long
Declare Function piar_getarcvaluex Lib "piapi32.dll" (ByVal PtNum&, ByVal arcMode&, drVal As Any, iVal&, bVal As Any, bSize&, iStat&, Flags%, time0 As PITimeStamp) As Long
Declare Function piar_putarcvaluesx Lib "piapi32.dll" (ByVal Count&, ByVal Mode&, PtNum&, drVal As Any, iVal&, bVal As Any, bSize&, iStat&, Flags%, time0 As PITimeStamp, Errors&) As Long
Declare Function piar_putarcvaluex Lib "piapi32.dll" (ByVal PtNum&, ByVal Mode&, drVal As Any, iVal&, bVal As Any, bSize&, iStat&, Flags%, time0 As PITimeStamp) As Long
Declare Function pipt_pointtypex Lib "piapi32.dll" (ByVal PtNum&, typeX&) As Long
'Declare Function pisn_evmexceptionsx Lib "piapi32.dll" (ByVal Count&, PTnum&, event As PI_EVENT, ByVal FuncCode&) As Long
Declare Function pisn_evmexceptx Lib "piapi32.dll" (Count&, PtNum&, typeX&, Val As Variant, iStat&, Flags%, Timeval As PITimeStamp, ByVal FuncCode&) As Long
Declare Function pisn_flushputsnapqx Lib "piapi32.dll" (numbptErrs&, qErrs As QERROR) As Long
Declare Function pisn_getsnapshotsx Lib "piapi32.dll" (PtNum&, cntptnum&, drVal As Any, iVal&, bVal As Any, bSize&, iStat&, Flags%, time0 As PITimeStamp, lerror&, ByVal FuncCode&) As Long
Declare Function pisn_getsnapshotx Lib "piapi32.dll" (ByVal PtNum&, drVal As Any, iVal&, bVal As Any, bSize&, iStat&, Flags%, time0 As PITimeStamp) As Long
Declare Function pisn_putsnapshotqx Lib "piapi32.dll" (ByVal PtNum&, drVal As Any, iVal&, bVal As Any, bSize As Long, iStat As Long, Flags%, TimeStamp As PITimeStamp, ByVal Queueing&, numbptErrs&, qErrs As QERROR) As Long
'pisn_putsnapshotsx takes a pointer to an array of string pointers which can't be generated in VB
Declare Function pisn_putsnapshotsx Lib "piapi32.dll" (ByVal Count&, PtNum&, drVal As Any, iVal&, bVal As Any, bSize&, iStat&, Flags%, time0 As Any, Errors&) As Long
Declare Function pisn_putsnapshotx Lib "piapi32.dll" (ByVal PtNum&, drVal As Any, iVal&, bVal As Any, bSize&, iStat&, Flags%, time0 As Any) As Long
'Declare Function pisn_sendexceptionqx Lib "piapi32.dll" (ByVal pt&, ByVal typeX&, oldValue As PI_EVENT, prevvalue As PI_EVENT, newvalue As PI_EVENT, ByVal ExcDevEng#, ByVal ExcMin#, ByVal ExcMax#, Count&, ByVal Queueing&, numbptErrs&, qErrs As QERROR) As Long
'Declare Function pisn_sendexceptionsx Lib "piapi32.dll" (ByVal num_pts&, ptnum&, typeX&, except As PI_EXCEPTSTRU, Errors&, Count&) As Long
Declare Function pisn_sendexceptqx Lib "piapi32.dll" (ByVal pt&, ByVal typeX&, oldVal As Variant, oldistat&, oldFlags%, oldTime As PITimeStamp, prevVal As Variant, previstat&, prevFlags%, prevTime As PITimeStamp, Val As Variant, iStat&, Flags%, newTime As PITimeStamp, ByVal ExcDevEng#, ByVal ExcMin#, ByVal ExcMax#, Count&, ByVal Queueing&, numbptErrs&, qErrs As QERROR) As Long
Declare Function pitm_getpitime Lib "piapi32.dll" (time0 As PITimeStamp, frac#) As Long
Declare Function pitm_getutctime Lib "piapi32.dll" (time0 As PITimeStamp) As Double
Declare Function pitm_isdst Lib "piapi32.dll" (time0 As PITimeStamp) As Long
Declare Sub pitm_setcurtime Lib "piapi32.dll" (time0 As PITimeStamp, ByVal incl_subsec%)
Declare Sub pitm_setdst Lib "piapi32.dll" (time0 As PITimeStamp, ByVal tm_isdst&)
Declare Function pitm_setpitime Lib "piapi32.dll" (time0 As PITimeStamp, ByVal PITime&, ByVal frac#) As Long
Declare Function pitm_settime Lib "piapi32.dll" (time0 As PITimeStamp, ByVal xyear&, ByVal xmonth&, ByVal xday&, ByVal xhour&, ByVal xminute&, ByVal xsecond#) As Long
Declare Function piut_errormsg Lib "piapi32.dll" (ByVal stat&, ByVal Msg$, MsgLen&) As Long
Declare Function piut_setpassword Lib "piapi32.dll" (ByVal username$, ByVal oldpw$, ByVal newpw$) As Long
Declare Function piut_strerror Lib "piapi32.dll" (ByVal stat&, ByVal Msg$, MsgLen&, ByVal SrcStr$) As Long

