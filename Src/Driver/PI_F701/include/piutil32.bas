Attribute VB_Name = "Module2"
' @(#)piutil.bas        1.1 09/20/95
'
'Copyright (c) 1995 Oil Systems, Inc. All rights reserved. Unpublished - rights
'reserved under the copyright law of the United States.
'
'USE OF A COPYRIGHT NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
'OR DISCLOSURE.
'
'THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE SECRETS OF
'OIL SYSTEMS, INC.  USE, DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT
'THE PRIOR EXPRESS WRITTEN PERMISSION OF OIL SYSTEMS, INC.
'
'RESTRICTED RIGHTS LEGEND
'Use, duplication, or disclosure by the Government is subject to restrictions
'as set forth in subparagraph (c)(1)(ii) of the Rights in Technical Data and
'Computer Software clause at DFARS 252.227.7013
'
'Oil Systems, Inc.
'545 Estudillo Ave., San Leandro CA 94577
'
'
Option Explicit

Type PIVAL
   rval As Single
   istat As Long
   timedate As Long
End Type

'----- declarations used to make a window "always on top" -----
'courtesy of Microsoft
Declare Function SetWindowPos Lib "user32" (ByVal h&, ByVal hb%, ByVal x%, ByVal y%, ByVal cx%, ByVal cy%, ByVal f%) As Integer
Global Const SWP_NOMOVE = 2
Global Const SWP_NOSIZE = 1
Global Const FLAGS = SWP_NOMOVE Or SWP_NOSIZE
Global Const HWND_TOPMOST = -1
Global Const HWND_NOTOPMOST = -2

'------------------------------------- handle errors in PI
Function ChkPIstatus(piname As String, apistatus As Long) As Long
   If apistatus <> 0 Then
      MsgBox "Error in " & piname & ": status = " & apistatus
   End If
   ChkPIstatus = apistatus
End Function

'------------------------------------- connect to PI server if server has changed
Sub Connect(servername As String)
Dim status As Long
   status = ChkPIstatus("piut_setservernode", piut_setservernode(servername))
End Sub

Function FormatTime(pitime As Long) As String
Dim timestring As String * 19
   pitm_formtime pitime, timestring, Len(timestring)
   FormatTime = timestring
End Function

'----- Get digital state code, given a string
Function getDigCode(theString As String) As Long
Dim theCode As Long
   If pipt_digcode(theCode, theString) = -12 Then
      getDigCode = 0         ' return 0 if string not found
   Else
      getDigCode = -theCode   'return a negative value, it's more useful
   End If
End Function

'------------------------------------- get state string from code
Function getDigState(code As Long) As String
Dim state As String * 12
   If ChkPIstatus("pipt_digstate", pipt_digstate(code, state, 12)) <> 0 Then
      state = ""              'return empty string on error
   End If
   getDigState = state
End Function

'------------------------------------- get a snapshot event
Sub getEVMexception(ptno As Long, valu As PIVAL)
Dim count As Long
   count = 1
   If ChkPIstatus("pisn_evmexceptions", pisn_evmexceptions(count, ptno, valu.rval, valu.istat, valu.timedate)) <> 0 Or count = 0 Then
      ptno = 0        'return ptno = 0 if there is no exception at this time
   End If
End Sub

'----- get point number from point name
Function getptno(itsName As String) As Long
Dim ptno As Long
Dim apistatus As Long
Dim tagname As String * 80 'must be fixed-length string because API can modify it
   getptno = 0             'return zero on error
   tagname = itsName
   apistatus = pipt_findpoint(tagname, ptno)
   If apistatus = -5 Then
      MsgBox "Point """ & itsName & """ not found."
   ElseIf ChkPIstatus("pipt_findpoint", apistatus) = 0 Then
      getptno = ptno
   End If
End Function

'----- get point source character
Function getptsource(ptno As Long) As String
Dim ptsrc As String * 1
   If ChkPIstatus("pipt_pointsource", pipt_pointsource(ptno, ptsrc)) <> 0 Then
      ptsrc = ""             'return empty string on error
   End If
   getptsource = ptsrc
End Function

'----- get point type character
Function getPtType(ptno As Long) As String
Dim pttype As String * 1
   If ChkPIstatus("pipt_pointtype", pipt_pointtype(ptno, pttype)) <> 0 Then
      pttype = ""             'return empty string on error
   End If
   getPtType = pttype
End Function

'------------------------------------- get current value from point number
Sub GetSnapshot(ptno As Long, valu As PIVAL)
Dim status As Long
   status = ChkPIstatus("pisn_getsnapshot", pisn_getsnapshot(ptno, valu.rval, valu.istat, valu.timedate))
End Sub

'------------------------------------- sign up for snapshot events on a point
Sub ListenTo(ptno As Long)
Dim count As Long, status As Long
   count = 1
   status = ChkPIstatus("pisn_evmestablish", pisn_evmestablish(count, ptno))
End Sub

'----- Make this an ordinary (not topmost) window
Sub makenontopmost(theForm As Form)
Dim success As Integer
    success = SetWindowPos(theForm.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, FLAGS)
    'success is nonzero when successful
End Sub

'----- Make this window "always on top"
Sub maketopmost(theForm As Form)
Dim success As Integer
Dim winhandle As Long
winhandle = theForm.hWnd
    success = SetWindowPos(winhandle, HWND_TOPMOST, 0, 0, 0, 0, FLAGS)
    'success is nonzero when successful
End Sub

'----- process rval/istat, return result as string
Function parsePIval(valu As PIVAL, pttype As String) As String
   If valu.istat < 0 Then
      parsePIval = getDigState(-valu.istat)
   ElseIf pttype = "I" Then                'integer tag
      parsePIval = Str$(valu.istat)
   Else                                    'real tag
      parsePIval = Str$(valu.rval)
   End If
End Function

'------------------------------------- parse a PI time string
Function parsetime(s As String, rel As Long) As Long
Dim timestr As String * 19              'must be fixed-length string because API can modify it
Dim result As Long
   timestr = s
   If ChkPIstatus("pitm_parsetime", pitm_parsetime(timestr, rel, result)) <> 0 Then
      result = 0              'return zero on error
   End If
   parsetime = result
End Function

'-------------------- convert PI time to Excel time
' 1-jan-1970 is 25569 on MS Windows, and there are 86400 seconds in a day.
Function pitime2xl(pitime As Long) As Double
   pitime2xl = CDbl(pitime) / 86400 + 25569
End Function

'------------------------------------- stop listening for snapshot events on a point
Sub StopListening(ptno As Long)
Dim count As Long, status As Long
   count = 1
   status = ChkPIstatus("pisn_evmdisestablish", pisn_evmdisestablish(count, ptno))
End Sub

'----- Change a string into a PI value.
' return 0 if OK, 1 if error
Function StringToPIVAL(theString As String, pttype As String, valu As PIVAL) As Long
Dim theval As Variant
Dim theInt As Integer
   StringToPIVAL = 0             'be optimistic, set for OK return
   valu.istat = getDigCode(theString)
   If valu.istat <> 0 Then       'it is a digital state
      valu.rval = 0
   ElseIf pttype = "D" Then
      MsgBox "Digital value needed"
      StringToPIVAL = 1          'error
   Else    ' point is not digital, value is not digital
      theval = CVar(theString)    'see if it's a number
      If Not IsNumeric(theval) Then
    MsgBox "Numeric value needed"
   StringToPIVAL = 1       'error
      Else     'point is numeric, value is numeric
        valu.rval = theval      'setup for a real point, the usual case
         valu.istat = 0
  If pttype = "I" Then    'we need an integer
        theInt = Int(valu.rval)
         If theInt <> valu.rval Or theInt > 32767 Or theInt < 0 Then
        MsgBox "Valid integer value needed"
             StringToPIVAL = 1 'error
     Else  'point is integer, value is integer
          valu.istat = valu.rval
          valu.rval = 0
        End If  'value is integer
    End If   'point is integer
      End If   'value is numeric
   End If   'point is numeric
End Function

