#include "ElevateTool.h"


bool ElevateTool::isadminuser()
{
	bool isadmin = false;
	wchar_t username[MAX_NAME];
	DWORD namesize = MAX_NAME;
	GetUserName(username, &namesize);

	LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
	DWORD dwLevel = 0;
	DWORD dwFlags = LG_INCLUDE_INDIRECT;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	NET_API_STATUS nStatus;

	nStatus = NetUserGetLocalGroups(NULL,username,dwLevel,dwFlags,(LPBYTE*)& pBuf,dwPrefMaxLen,&dwEntriesRead,&dwTotalEntries);

	if (nStatus == NERR_Success)
	{
		LPLOCALGROUP_USERS_INFO_0 pTmpBuf;
		DWORD i;
		DWORD dwTotalCount = 0;

		if ((pTmpBuf = pBuf) != NULL)
		{

			for (i = 0; i < dwEntriesRead; i++)
			{

				if (pTmpBuf == NULL)
				{
					fprintf(stderr, "An access violation has occurred\n");
					break;
				}

				if (wcscmp(pTmpBuf->lgrui0_name, L"Administrators") == 0)
				{
					isadmin = true;
				}
				pTmpBuf++;
				dwTotalCount++;
			}
		}
	}
	else {
		fprintf(stderr, "A system error has occurred: %d\n", nStatus);
	}
		
	if (pBuf != NULL) {
		NetApiBufferFree(pBuf);
	}	
	return isadmin==true;
}

bool ElevateTool::istokenele()
{
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;
	BOOL isele=false;
	if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup))
	{
		CheckTokenMembership(NULL, AdministratorsGroup, &isele);
		FreeSid(AdministratorsGroup);
	}
	return  isele == true;
}

//Ì«ÈßÔÓ£¬ÏÈ²»¼Ó
bool ElevateTool::bypassuac()
{
	return false;
}

bool ElevateTool::getsystem()
{
	return false;
}

