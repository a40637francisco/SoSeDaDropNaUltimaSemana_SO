// ConsoleExercicio3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <psapi.h>

#define ANY_KEY() do {								\
		printf("Press Enter to continue...\n");	\
		getchar();									\
	} while(0)

#define THRESHOLD 10000000
#define TIMES 1
#define SLEEP 5000

int exit(char * m, int r)
{
	printf(m);
	ANY_KEY();
	return r;
}


long long GetTotalSpace(PSAPI_WORKING_SET_INFORMATION *wsi)
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	long long ret = 0;
	ULONG_PTR nPages = wsi->NumberOfEntries;

	for (DWORD i = 0; i <= nPages; i++)
	{
		if (wsi->WorkingSetInfo[i].Shared == 0)
		{
			ret++;
		}
		
	}

	return ret * si.dwPageSize;
}


int checkMemLeaks(int pid)
{

	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (process == NULL)
	{
		return exit("Could not find desired process ID\n", 1);
	}

	long long totalSize;
	DWORD wsSize;
	int count = 1;
	do
	{
		PSAPI_WORKING_SET_INFORMATION info, *wsi;
		
		PSAPI_WORKING_SET_INFORMATION* wsit = (PSAPI_WORKING_SET_INFORMATION*)malloc(sizeof(PSAPI_WORKING_SET_INFORMATION));

		QueryWorkingSet(process, (LPVOID)&info, sizeof(&wsi));

		wsSize = sizeof(PSAPI_WORKING_SET_INFORMATION)
			+ sizeof(PSAPI_WORKING_SET_BLOCK) * info.NumberOfEntries;

		wsi = (PSAPI_WORKING_SET_INFORMATION*)malloc(wsSize);

		if (!QueryWorkingSet(process, (LPVOID)wsi, wsSize)) {
			printf("%u", GetLastError());
			return exit("it failed\n", 1);
		}

		totalSize = GetTotalSpace(wsi);

		printf("%u\n", totalSize);
		if (totalSize > THRESHOLD)
		{
			return exit("memory leak in process\n", 1);
		}

		free(wsi);
		Sleep(SLEEP);
	} while (++count < TIMES);

	
	return exit("closing\n", 0);
}



int main(int argc, char * argv[])
{

	return checkMemLeaks(atoi(argv[1]));
}
