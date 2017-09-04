; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=14
Class1=COdbcdrvApp
LastClass=COracleCfgDlg
NewFileInclude2=#include "odbcdrv.h"
ResourceCount=10
NewFileInclude1=#include "stdafx.h"
Class2=CDlgConfig
LastTemplate=CDialog
Resource1=IDD_DIALOG_CONFIG
Class3=CDlgDbSet
Resource2=IDD_DIALOG_DB2
Class4=CDlgSelDb
Resource3=IDD_DIALOG_MSSQL
Class5=CDlgSetSql2K
Resource4=IDD_DIALOG_MYSQL
Resource5=IDD_DIALOG_SELDBDRV
Resource6=IDD_DIALOG_DBCONFIG
Resource7=IDD_DIALOG_CONNSTR
Resource8=IDD_DIALOG_SYBASE
Resource9=IDD_DIALOG_ORACLE
Class6=CDB2CfgDlg
Class7=CDbConnStrDlg
Class8=CDbCfgDlg
Class9=CSQLServerCfgDlg
Class10=CMySQLCfgDlg
Class11=CSelDBDrvDlg
Class12=CSybaseCfgDlg
Class13=COracleCfgDlg
Class14=CTableCfgDlg
Resource10=IDD_DIALOG_TABLE

[CLS:COdbcdrvApp]
Type=0
HeaderFile=odbcdrv.h
ImplementationFile=odbcdrv.cpp
Filter=N
LastObject=COdbcdrvApp

[DLG:IDD_DIALOG_CONFIG]
Type=1
Class=CDlgConfig
ControlCount=7
Control1=IDC_STATIC_FRM,static,1342177287
Control2=IDOK,button,1342242817
Control3=IDC_BUTTON_EXSAMP,button,1342242816
Control4=IDC_BUTTON_INPORT,button,1342242816
Control5=IDC_BUTTON_EXPORT,button,1342242816
Control6=IDC_BUTTON_DELSEL,button,1342242816
Control7=IDC_BUTTON_UP,button,1073807360

[CLS:CDlgConfig]
Type=0
HeaderFile=DlgConfig.h
ImplementationFile=DlgConfig.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgConfig

[CLS:CDlgDbSet]
Type=0
HeaderFile=DlgDbSet.h
ImplementationFile=DlgDbSet.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgDbSet

[CLS:CDlgSelDb]
Type=0
HeaderFile=DlgSelDb.h
ImplementationFile=DlgSelDb.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgSelDb

[CLS:CDlgSetSql2K]
Type=0
HeaderFile=DlgSetSql2K.h
ImplementationFile=DlgSetSql2K.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgSetSql2K

[DLG:IDD_DIALOG_CONNSTR]
Type=1
Class=CDbConnStrDlg
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342177296
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_CONNSTR,edit,1350631556

[DLG:IDD_DIALOG_DB2]
Type=1
Class=CDB2CfgDlg
ControlCount=13
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT_SERVER,edit,1350631552
Control4=IDC_EDIT_PORT,edit,1350639744
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_DATABASE,edit,1350631552
Control9=IDC_EDIT_USER,edit,1350631552
Control10=IDC_EDIT_PASSWORD,edit,1350631584
Control11=IDC_STATIC,static,1342308352
Control12=IDC_BUTTON_CONNSTR,button,1342242816
Control13=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG_DBCONFIG]
Type=1
Class=CDbCfgDlg
ControlCount=7
Control1=IDC_BUTTON_NEXT,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_BUTTON_BACK,button,1476460544
Control4=IDC_TAB_LIST,SysTabControl32,1342177536
Control5=IDC_STATIC,static,1342177296
Control6=IDC_BUTTON_TEST,button,1342242816
Control7=IDC_BUTTON_TABCFG,button,1342242816

[DLG:IDD_DIALOG_MSSQL]
Type=1
Class=CSQLServerCfgDlg
ControlCount=16
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_SERVER,edit,1350631552
Control6=IDC_EDIT_PORT,edit,1350639744
Control7=IDC_EDIT_INST,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_EDIT_DATABASE,edit,1350631552
Control12=IDC_EDIT_USER,edit,1350631552
Control13=IDC_EDIT_PASSWORD,edit,1350631584
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_BUTTON_CONNSTR,button,1342242816

[DLG:IDD_DIALOG_MYSQL]
Type=1
Class=CMySQLCfgDlg
ControlCount=13
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT_SERVER,edit,1350631552
Control4=IDC_EDIT_PORT,edit,1350639744
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_DATABASE,edit,1350631552
Control9=IDC_EDIT_USER,edit,1350631552
Control10=IDC_EDIT_PASSWORD,edit,1350631584
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_BUTTON_CONNSTR,button,1342242816

[DLG:IDD_DIALOG_ORACLE]
Type=1
Class=COracleCfgDlg
ControlCount=7
Control1=IDC_STATIC,static,1342308352
Control2=IDC_COMBO_SERVER,combobox,1344340226
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_USER,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_PASSWORD,edit,1350631584
Control7=IDC_BUTTON_CONNSTR,button,1342242816

[DLG:IDD_DIALOG_SELDBDRV]
Type=1
Class=CSelDBDrvDlg
ControlCount=3
Control1=IDC_STATIC,static,1342181390
Control2=IDC_STATIC,static,1342308352
Control3=IDC_LIST_DBTYPE,listbox,1352728833

[DLG:IDD_DIALOG_SYBASE]
Type=1
Class=CSybaseCfgDlg
ControlCount=7
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDIT_SERVER,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_USER,edit,1350631552
Control6=IDC_EDIT_PASSWORD,edit,1350631584
Control7=IDC_BUTTON_CONNSTR,button,1342242816

[CLS:CDB2CfgDlg]
Type=0
HeaderFile=DB2CfgDlg.h
ImplementationFile=DB2CfgDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDB2CfgDlg

[CLS:CDbConnStrDlg]
Type=0
HeaderFile=DbConnStrDlg.h
ImplementationFile=DbConnStrDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDbConnStrDlg

[CLS:CDbCfgDlg]
Type=0
HeaderFile=DbCfgDlg.h
ImplementationFile=DbCfgDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDbCfgDlg

[CLS:CSQLServerCfgDlg]
Type=0
HeaderFile=SQLServerCfgDlg.h
ImplementationFile=SQLServerCfgDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSQLServerCfgDlg

[CLS:CMySQLCfgDlg]
Type=0
HeaderFile=MySQLCfgDlg.h
ImplementationFile=MySQLCfgDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CMySQLCfgDlg

[CLS:CSelDBDrvDlg]
Type=0
HeaderFile=SelDBDrvDlg.h
ImplementationFile=SelDBDrvDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSelDBDrvDlg

[CLS:CSybaseCfgDlg]
Type=0
HeaderFile=SybaseCfgDlg.h
ImplementationFile=SybaseCfgDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSybaseCfgDlg

[CLS:COracleCfgDlg]
Type=0
HeaderFile=OracleCfgDlg.h
ImplementationFile=OracleCfgDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COracleCfgDlg

[DLG:IDD_DIALOG_TABLE]
Type=1
Class=CTableCfgDlg
ControlCount=14
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT_TABNAME,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_FNAME,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_FTIME,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_EDIT_FQA,edit,1350631552
Control10=IDC_STATIC,static,1342308352
Control11=IDC_EDIT_FVAL,edit,1350631552
Control12=IDC_STATIC,button,1342308359
Control13=IDC_RADIO_SL,button,1342308361
Control14=IDC_RADIO_ML,button,1342177289

[CLS:CTableCfgDlg]
Type=0
HeaderFile=TableCfgDlg.h
ImplementationFile=TableCfgDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_EDIT_FQA

