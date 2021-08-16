#include "RegTool.h"

void RegTool::getType(wchar_t* keyname, DWORD* type)
{
	this->errorcode=RegQueryValueEx(hkey, NULL, NULL, type, NULL, NULL);
}

void RegTool::getValue(wchar_t* keyname, wchar_t* data, DWORD* fsize,DWORD *type)
{
	this->errorcode=RegQueryValueEx(this->hkey, keyname, 0, type, (LPBYTE)data, fsize);
}

void RegTool::writeValue(wchar_t* keyname,  wchar_t* data, DWORD size,DWORD type)
{
	this->errorcode = RegSetValueEx(this->hkey,keyname,0, type, (BYTE *)data,size);
}

void RegTool::openReadkey(HKEY rootkey,wchar_t* pathname)
{
	this->errorcode=RegOpenKeyEx(rootkey,pathname,0, KEY_READ,&(this->hkey));
	
}

void RegTool::openWriteKey(HKEY rootkey,wchar_t* pathname)
{
	this->errorcode = RegOpenKeyEx(rootkey, pathname, 0, KEY_WRITE, &(this->hkey));
}

void RegTool::CreateReg(HKEY rootkey, wchar_t* pathname)
{	
	HKEY tmpkey = NULL;
	this->errorcode=RegCreateKey(rootkey,pathname, &tmpkey);
	if (tmpkey!=NULL)
	{
		RegCloseKey(tmpkey);
	}
}

void RegTool::closeKey()
{
	if (this->hkey != NULL)
	{
		RegCloseKey(this->hkey);
	}
}

DWORD RegTool::getErrorCode()
{
	DWORD code = this->errorcode;
	this->errorcode = 0;
	return code;
}

RegTool::~RegTool()
{
	if (this->hkey!=NULL)
	{
		RegCloseKey(this->hkey);
	}
	
}

RegTool::RegTool() {

	this->hkey = NULL;
	this->errorcode = 0;

}