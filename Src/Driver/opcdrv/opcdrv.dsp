# Microsoft Developer Studio Project File - Name="opcdrv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=opcdrv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "opcdrv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "opcdrv.mak" CFG="opcdrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "opcdrv - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "opcdrv - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "opcdrv - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Objs\ddc_opcdrv_Release"
# PROP Intermediate_Dir "..\..\Objs\ddc_opcdrv_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"../../Bin/DDC2/drivers/opcdrv.dll"

!ELSEIF  "$(CFG)" == "opcdrv - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Objs\ddc_opcdrv_Debug"
# PROP Intermediate_Dir "..\..\Objs\ddc_opcdrv_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../Bin/DDC2/drivers/opcdrv.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "opcdrv - Win32 Release"
# Name "opcdrv - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\advisesink.cpp
# End Source File
# Begin Source File

SOURCE=.\datasink20.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOpcSet.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOpcVar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\FileExplorer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\HashTableUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\interface.cpp
# End Source File
# Begin Source File

SOURCE=.\myopc.cpp
# End Source File
# Begin Source File

SOURCE=.\opccomn_i.c
# End Source File
# Begin Source File

SOURCE=.\opcda_i.c
# End Source File
# Begin Source File

SOURCE=.\opcdrv.cpp
# End Source File
# Begin Source File

SOURCE=.\opcdrv.def
# End Source File
# Begin Source File

SOURCE=.\opcdrv.rc
# End Source File
# Begin Source File

SOURCE=.\OpcEnum_i.c
# End Source File
# Begin Source File

SOURCE=.\safearray.cpp
# End Source File
# Begin Source File

SOURCE=..\sdk\sdktool.cpp
# End Source File
# Begin Source File

SOURCE=.\shutdownsink.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\timestmp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DlgHelp.h
# End Source File
# Begin Source File

SOURCE=.\DlgOpcSet.h
# End Source File
# Begin Source File

SOURCE=.\DlgOpcVar.h
# End Source File
# Begin Source File

SOURCE=..\sdk\drvdef.h
# End Source File
# Begin Source File

SOURCE=..\sdk\drvsdk.h
# End Source File
# Begin Source File

SOURCE=..\sdk\DrvVersion.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\FileExplorer.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\HashTableUtils.h
# End Source File
# Begin Source File

SOURCE=.\myopc.h
# End Source File
# Begin Source File

SOURCE=.\opcdrv.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\sdk\sdktool.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\groupimages.bmp
# End Source File
# Begin Source File

SOURCE=.\itemimages.bmp
# End Source File
# Begin Source File

SOURCE=.\res\opcdrv.rc2
# End Source File
# End Group
# Begin Group "ListCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ItemWinList.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemWinList.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\SortObArray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\SortObArray.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\WinList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\WinList.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\opchelp.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
