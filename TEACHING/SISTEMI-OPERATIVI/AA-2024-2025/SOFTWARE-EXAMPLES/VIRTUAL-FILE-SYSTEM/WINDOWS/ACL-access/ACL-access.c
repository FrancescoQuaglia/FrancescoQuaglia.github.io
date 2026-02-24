
// ACL-access.c : Defines the entry point for the console application.
// you can compile with either ASCII or UNICODE
//

#
#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <Sddl.h>
#include "accctrl.h"
#include "aclapi.h"
#pragma comment(lib, "advapi32.lib")

int main(void)
{
	DWORD dwRtnCode = 0;
	PSID pSidOwner = NULL;
	BOOL bRtnBool = TRUE;
	LPTSTR AcctName = NULL;
	LPTSTR DomainName = NULL;
	DWORD dwAcctName = 1, dwDomainName = 1;
	SID_NAME_USE eUse = SidTypeUnknown;
	HANDLE hFile;
	PSECURITY_DESCRIPTOR pSD = NULL;
	TCHAR *stringSID;


	// Get the handle of the file object.
	hFile = CreateFile(
		TEXT("myfile.txt"),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS, //|OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// Check GetLastError for CreateFile error code.
	if (hFile == INVALID_HANDLE_VALUE) {
		DWORD dwErrorCode = 0;

		dwErrorCode = GetLastError();
		_tprintf(TEXT("CreateFile error = %d\n"), dwErrorCode);
		return -1;
	}



	// Get the owner SID of the file.
	dwRtnCode = GetSecurityInfo(
		hFile,
		SE_FILE_OBJECT,
		OWNER_SECURITY_INFORMATION,
		&pSidOwner,
		NULL,
		NULL,
		NULL,
		&pSD);

	printf("this is the owner address %p\nthis is the structure address %p\n", pSidOwner, pSD);
	// Check GetLastError for GetSecurityInfo error condition.
	if (dwRtnCode != ERROR_SUCCESS) {
		DWORD dwErrorCode = 0;

		dwErrorCode = GetLastError();
		_tprintf(TEXT("GetSecurityInfo error = %d\n"), dwErrorCode);
		return -1;
	}

	ConvertSidToStringSid(pSidOwner, &stringSID);

	_tprintf(TEXT("this is the owner SID %s\n"), stringSID);


	AcctName = (LPTSTR)malloc(4096);
	DomainName = (LPTSTR)malloc(4096);

	dwAcctName = 4096;
	dwDomainName = 4096;

	bRtnBool = LookupAccountSid(
		NULL,                   // name of local or remote computer
		pSidOwner,              // security identifier
		AcctName,               // account name buffer
		(LPDWORD)&dwAcctName,   // size of account name buffer 
		DomainName,             // domain name
		(LPDWORD)&dwDomainName, // size of domain name buffer
		&eUse);                 // SID type

	// Print the account name.
	_tprintf(TEXT("Account owner = %s\n"), AcctName);

	return 0;
}


