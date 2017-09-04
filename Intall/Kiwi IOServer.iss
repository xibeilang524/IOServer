[Setup]
OutputBaseFilename=Kiwi IOServer V2.3
VersionInfoVersion=2.3.0.1
VersionInfoCompany=双人鱼科技
VersionInfoDescription=Kiwi IOServer V2.3
VersionInfoCopyright=双人鱼科技,保留所有权利
AppCopyright=双人鱼科技,保留所有权利
AppName=Kiwi IOServer V2.3
AppVerName=Kiwi IOServer V2.3.0.1
DefaultDirName={pf}\Kiwi IOServer
DefaultGroupName=\Kiwi IOServer
OutputDir=Setup      
LicenseFile=License.rtf

[Dirs]
Name: {app}\drivers
Name: {app}\logs
Name: {app}\project
Name: {app}\system
Name: {app}\pdb
Name: {app}\Package

[Files]          
Source: License.rtf; DestDir: {app}\; Flags: ignoreversion

Source: ..\bin\Kiwi IOServer.exe; DestDir: {app}\; Flags: ignoreversion 
Source: ..\bin\LogModule.dll; DestDir: {app}\; Flags: ignoreversion
;Source: ..\bin\GeneralD.dll; DestDir: {app}\; Flags: ignoreversion
Source: ..\bin\General.dll; DestDir: {app}\; Flags: ignoreversion     
Source: ..\bin\DriverMgr.dll; DestDir: {app}\; Flags: ignoreversion
;Source: ..\bin\CLicenseD.dll; DestDir: {app}\; Flags: ignoreversion 
Source: ..\bin\CLicense.dll; DestDir: {app}\; Flags: ignoreversion
;Source: ..\bin\CGeneral.dll; DestDir: {app}\; Flags: ignoreversion
Source: ..\bin\AuthorityMgr.dll; DestDir: {app}\; Flags: ignoreversion   
Source: ..\bin\CCrashWatch.dll; DestDir: {app}\; Flags: ignoreversion                                                                     ;Source: ..\bin\PDB_Release\dbpapi.pdb; DestDir: {app}\pdb; Flags: ignoreversion 

Source: ..\bin\drivers\SimDrv.dll; DestDir: {app}\drivers; Flags: ignoreversion  Source: ..\bin\drivers\modbus.dll; DestDir: {app}\drivers; Flags: ignoreversion   
Source: ..\bin\drivers\OPCDrv.dll; DestDir: {app}\drivers; Flags: ignoreversion 
Source: ..\bin\drivers\102Drv.dll; DestDir: {app}\drivers; Flags: ignoreversion   
Source: ..\bin\drivers\104Drv.dll; DestDir: {app}\drivers; Flags: ignoreversion 

Source: ..\bin\system\Operator.dll; DestDir: {app}\system; Flags: ignoreversion   
Source: ..\bin\system\opt.opt; DestDir: {app}\system; Flags: ignoreversion
Source: ..\bin\system\Server.ini; DestDir: {app}\system; Flags: ignoreversion
;config file           
Source: ..\bin\config\config.ini; DestDir: {app}\config; Flags: ignoreversion

Source: vcredist_x86.exe; DestDir: {app}\Package  

[Run]
Filename: {app}\Package\vcredist_x86.exe; WorkingDir: {app}\Package; StatusMsg: 正在安装运行库.; Flags: runminimized runhidden; Check: VCRCheck

[UninstallRun]

[LangOptions]
LanguageName=Chinese
LanguageID=$0804

[Tasks]
Name: desktopicon; Description: 创建桌面快捷方式; GroupDescription: 添加快捷方式:; Flags: checkedonce

[Icons]
Name: {group}\Kiwi IOServer V2.3; Filename: {app}\Kiwi IOServer.exe; WorkingDir: {app}\; IconFilename: {app}\Kiwi IOServer; IconIndex: 0
Name: {group}\{cm:UninstallProgram, Kiwi IOServer V2.0}; Filename: {uninstallexe}  
Name: {userdesktop}\Kiwi IOServer; Filename: {app}\Kiwi IOServer.exe; Tasks: desktopicon

;[Languages]
;Name: "ChineseSimp"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"

[Code]
function VCRCheck(): Boolean;
begin
  Result := not RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{710f4c1c-cc18-4c59-8cbf-51240c89a1b3}');
end;