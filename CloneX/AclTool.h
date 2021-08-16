#pragma once
#include "CloneX.h"


typedef struct ACE_Object {
	ACE_HEADER  Header;
	ACCESS_MASK Mask;
	DWORD       SidStart;
}*PACE_Object;

class AclTool
{
public:
	void openDacl(wchar_t *objectname);
	void canRead();
	void canWrite();
	void canWriteDAC();
	void listAcl();
	void AddReadWriteACE(wchar_t* name);
	void getOwner(SID* psid);
	DWORD getErrorcode();
	AclTool(SE_OBJECT_TYPE ot);

private:
	DWORD errorCode=0;
	PACL ExistingDacl = NULL;
	PACL NewDacl = NULL;
	PSID ownerSID = NULL;
	PSID groupSID = NULL;
	PSECURITY_DESCRIPTOR psd = NULL;
	SID_NAME_USE siduse = SID_NAME_USE::SidTypeUnknown;
	SE_OBJECT_TYPE sot = SE_UNKNOWN_OBJECT_TYPE;

	

};

