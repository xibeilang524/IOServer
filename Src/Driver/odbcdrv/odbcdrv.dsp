# Microsoft Developer Studio Project File - Name="odbcdrv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=odbcdrv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "odbcdrv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "odbcdrv.mak" CFG="odbcdrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "odbcdrv - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbcdrv - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "odbcdrv - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Objs\ddc_odbcdrv_Release"
# PROP Intermediate_Dir "..\..\Objs\ddc_odbcdrv_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"../../Bin/DDC2/drivers/odbcdrv.dll"

!ELSEIF  "$(CFG)" == "odbcdrv - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Objs\ddc_odbcdrv_Debug"
# PROP Intermediate_Dir "..\..\Objs\ddc_odbcdrv_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../Bin/DDC2/drivers/odbcdrv.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "odbcdrv - Win32 Release"
# Name "odbcdrv - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ConnStringParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\CSVFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DB2CfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DbCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DbConnStrDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\sdk\drvsdk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\FileExplorer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\HashTableUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\MySQLCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\odbcdrv.cpp
# End Source File
# Begin Source File

SOURCE=.\odbcdrv.def
# End Source File
# Begin Source File

SOURCE=.\odbcdrv.rc
# End Source File
# Begin Source File

SOURCE=.\odbcobj.cpp
# End Source File
# Begin Source File

SOURCE=.\OracleCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\sdk\sdktool.cpp
# End Source File
# Begin Source File

SOURCE=.\SelDBDrvDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\SortObArray.cpp
# End Source File
# Begin Source File

SOURCE=.\SQLServerCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SybaseCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TableCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSheet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\WinList.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ConnStringParser.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\CSVFile.h
# End Source File
# Begin Source File

SOURCE=.\DB2CfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\DbCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\DbConnStrDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\DebugUtils.h
# End Source File
# Begin Source File

SOURCE=.\DlgConfig.h
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

SOURCE=.\MySQLCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\odbcdrv.h
# End Source File
# Begin Source File

SOURCE=.\odbcobj.h
# End Source File
# Begin Source File

SOURCE=.\OracleCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\sdk\sdktool.h
# End Source File
# Begin Source File

SOURCE=.\SelDBDrvDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\SortObArray.h
# End Source File
# Begin Source File

SOURCE=.\SQLServerCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SybaseCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\TableCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabSheet.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\TimeUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\WinList.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\dbdrv_list.bmp
# End Source File
# Begin Source File

SOURCE=.\res\odbcdrv.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
