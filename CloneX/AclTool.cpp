#include "AclTool.h"



void AclTool::openDacl(wchar_t* objectname)
{
	this->errorCode=GetNamedSecurityInfo(objectname, this->sot, DACL_SECURITY_INFORMATION, &(this->ownerSID), &(this->groupSID), &(this->ExistingDacl), NULL, &(this->psd));
}

void AclTool::canRead()
{

}

void AclTool::canWrite()
{
}

void AclTool::canWriteDAC()
{

}

void AclTool::listAcl()
{
	int aceindex = 0;
	SID* psid;
	wchar_t AcctName[MAX_NAME];
	wchar_t DomainName[MAX_NAME];
	DWORD dwAcctName = MAX_NAME, dwDomainName = MAX_NAME;
	SID_NAME_USE siduse = SID_NAME_USE::SidTypeUnknown;
	PACE_Object ace = NULL;
	wchar_t *sidstr=NULL;
	wprintf(L"[!]List access contrl list:\n");
	if (this->sot==SE_REGISTRY_KEY)
	{
		while (aceindex < this->ExistingDacl->AceCount)
		{
			GetAce(this->ExistingDacl, aceindex, (LPVOID*)& ace);
			psid = (SID*) & (ace->SidStart);
			switch (ace->Header.AceType)
			{
				
			case ACCESS_ALLOWED_ACE_TYPE:
				// Lookup the account name and print it.          
				// http://msdn2.microsoft.com/en-us/library/aa379554.aspx
				// http://www.netid.washington.edu/documentation/domains/sddl.aspx

				LookupAccountSid(NULL, psid, AcctName, &dwAcctName, DomainName, &dwDomainName, &siduse);
				ConvertSidToStringSid(psid, (LPWSTR *)&sidstr);

				fwprintf(stdout, L"  [*] %ws\\%ws -> SID : %ws [+]", DomainName, AcctName, sidstr);
				LocalFree(sidstr);
				ZeroMemory(DomainName, MAX_NAME);
				ZeroMemory(AcctName, MAX_NAME);
				//https://docs.microsoft.com/en-us/windows/win32/sysinfo/registry-key-security-and-access-rights
				if ((ace->Mask & KEY_ALL_ACCESS) == KEY_ALL_ACCESS)
				{
					fprintf(stdout, " KEY_ALL_ACCESS ");
				}
				else {

					if ((ace->Mask & KEY_READ) == KEY_READ)
					{
						fprintf(stdout, " KEY_READ ");
					}
					if ((ace->Mask & KEY_WRITE) == KEY_WRITE)
					{
						fprintf(stdout, " KEY_WRITE ");
					}

				}
				if ((ace->Mask & WRITE_DAC) == WRITE_DAC) {
					fprintf(stdout, " WRITE_DAC ");
				}
				if ((ace->Mask & READ_CONTROL) == READ_CONTROL) {
					fprintf(stdout, " READ_CONTROL ");
				}
				fprintf(stdout, "   \n");
				break;
				// Denied ACE
			case ACCESS_DENIED_ACE_TYPE:
				break;
				// Uh oh
			default:
				break;
			}
			aceindex = aceindex + 1;

		}
	}
	else if (this->sot==SE_KERNEL_OBJECT) {
	
	}
}

//添加当前用户的ALL_ACCESS权限
//debug
void AclTool::AddReadWriteACE(wchar_t *name)
{
	if (this->sot==SE_REGISTRY_KEY)
	{
		EXPLICIT_ACCESS ea;
		wchar_t username[MAX_NAME];
		DWORD usernamesize = MAX_NAME;
		PACL pNewDacl = NULL;
		GetUserName(username, &usernamesize);
		//LPUSER_INFO_1 userinfo = NULL;
		//DWORD errorcode = NetUserGetInfo(NULL, username, 1, (LPBYTE*)& userinfo);
		ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
		BuildExplicitAccessWithName(&ea, username, KEY_ALL_ACCESS, GRANT_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);
		SetEntriesInAcl(1, &ea, this->ExistingDacl, &pNewDacl);
		this->errorCode=SetNamedSecurityInfo(name, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDacl, NULL);
	}
	
}



void AclTool::getOwner(SID* psid)
{
}

DWORD AclTool::getErrorcode()
{
	DWORD code = this->errorCode;
	this->errorCode = 0;
	return code;
}


AclTool::AclTool(SE_OBJECT_TYPE ot) {

	this->sot = ot;


}