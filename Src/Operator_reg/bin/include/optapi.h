#if !defined _OPTAPI_H_I
#define _OPTAPI_H_I

#define OPT_OK			0
#define OPT_ERRHANDLE	1 
#define OPT_ERRPASS		2	//密码错误
#define OPT_ERRNAME		3	//无此账号
#define OPT_CANCEL		4	//取消
#define OPT_NOPOWER		5	//无此权限

typedef DWORD	OPTHANDLE;	
extern "C"{

OPTHANDLE __stdcall OPT_Create(); //创建
DWORD __stdcall OPT_Destroy(OPTHANDLE dwHandle); //销毁
DWORD __stdcall OPT_MkDefault(OPTHANDLE dwHandle); //创建默认管理员
DWORD __stdcall OPT_SetOptFile(OPTHANDLE dwHandle,LPCTSTR lpszFile); //设置操作员文件
DWORD __stdcall OPT_LoadOptFile(OPTHANDLE dwHandle,LPCTSTR lpszFile); //读入文件
DWORD __stdcall OPT_Login(OPTHANDLE dwHandle,char sName[64],char sGroup[64],char sPass[64],DWORD *pdwPower,BOOL bLic = FALSE); //登录
DWORD __stdcall OPT_ModifyPass(OPTHANDLE dwHandle,LPCTSTR lpszName); //修改密码
DWORD __stdcall OPT_OptMan(OPTHANDLE dwHandle,LPCTSTR lpszName,LPCTSTR lpszPass); //管理账号
DWORD __stdcall OPT_LoginNoWin(DWORD dwHandle,const char* suser,const char* spass,DWORD *pdwPower);
}
#endif //_OPTAPI_H_I