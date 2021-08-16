#pragma once
#include "CloneX.h"

class RegTool
{
public:
	void getType(wchar_t *keyname, DWORD *type);
	void getValue(wchar_t *keyname, wchar_t* data, DWORD *fsize,DWORD *type);
	void writeValue(wchar_t* keyname,  wchar_t* data, DWORD size, DWORD type);
	void openReadkey(HKEY rootkey,wchar_t* pathname);
	void openWriteKey(HKEY rootkey,wchar_t* pathname);
	void CreateReg(HKEY rootkey, wchar_t* pathname);
	void closeKey();
	DWORD getErrorCode();
	RegTool();
private:
	HKEY hkey;
	DWORD errorcode;
	~RegTool();
	
};

