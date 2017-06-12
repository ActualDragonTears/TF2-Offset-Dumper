#include "ProcMem.h"

using namespace std;

#pragma region Misc Functions

ProcMem::ProcMem() {
	//Constructor For Class, Do Not Remove!
}

ProcMem::~ProcMem() {
	//De-Constructor
	//Clean Up! (Close Handle - Not Needed Anymore)
	CloseHandle(hProcess);
}

#pragma region Memory Functions

/* This Function Will Return A Handle To The Process So We Can Write & Read Memeory From The Process. */
void ProcMem::Process(char* ProcessName) {

	//Variables
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); //Snapshot To View All Active Processes
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry); //Declare Structure Size And Populate It

										  //Loop Through All Running Processes To Find Process
	do
		if (!strcmp(ProcEntry.szExeFile, ProcessName))
		{
			//Store Process ID
			dwPID = ProcEntry.th32ProcessID;
			CloseHandle(hPID);

			//Give Our Handle All Access Rights 
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
			return;
		}
	while (Process32Next(hPID, &ProcEntry));

	cout << "You haven't even started the game yet.";
	system("pause");
	exit(0);
}

bool ProcMem::DataCompare(BYTE* data, BYTE* sign, char* mask)
{
	for (; *mask; mask++, sign++, data++)
	{
		if (*mask == 'x' && *data != *sign)
		{
			return false;
		}
	}
	return true;
}

DWORD ProcMem::FindSignature(DWORD base, DWORD size, BYTE* sign, char* mask)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	DWORD offset = 0;
	while (offset < size)
	{
		VirtualQueryEx(hProcess, (LPCVOID)(base + offset), &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		if (mbi.State != MEM_FREE)
		{
			BYTE* buffer = new BYTE[mbi.RegionSize];
			ReadProcessMemory(hProcess, mbi.BaseAddress, buffer, mbi.RegionSize, NULL);
			for (int i = 0; i < mbi.RegionSize; i++)
			{
				if (DataCompare(buffer + i, sign, mask))
				{
					delete[] buffer;
					return (DWORD)mbi.BaseAddress + i;
				}
			}

			delete[] buffer;
		}
		offset += mbi.RegionSize;
	}
	return 0;
}


/* Returns The Base Address Of The Specified Module Inside The Target Process
/* e.g.[ Module("client.dll"); ]. */
DWORD ProcMem::Module(LPSTR ModuleName) {

	//Variables
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); //Take A Module Snapshot Of The Process (Grab All Loaded Modules)
	MODULEENTRY32 mEntry; //Declare Module Entry Structure
	mEntry.dwSize = sizeof(mEntry); //Declare Structure Size And Populate It With Loaded Modules

									//Scan For Module By Name
	do
		if (!strcmp(mEntry.szModule, ModuleName))
		{
			CloseHandle(hModule);
			return (DWORD)mEntry.modBaseAddr;
		}
	while (Module32Next(hModule, &mEntry));

	//cout << "\nCouldn't find client.dll, retrying...\n";
	return 0;
}

DWORD ProcMem::ModuleSize(char* ModuleName)
{
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	MODULEENTRY32 mEntry;
	mEntry.dwSize = sizeof(mEntry);

	do
		if (!strcmp(mEntry.szModule, ModuleName))
		{
			CloseHandle(hModule);
			return (DWORD)mEntry.modBaseSize;
		}
	while (Module32Next(hModule, &mEntry));

	return 0;
}

DWORD ProcMem::FindAddress(DWORD mod, DWORD modsize, BYTE *sig, char *mask, scandefintions_t def) {
	if (def == scandefintions_t::read)
	{
		DWORD initAddress = FindSignature(mod, modsize, sig, mask);
		DWORD ptrAddress = Read<DWORD>(initAddress);
		return ptrAddress - mod;
	}

	if (def == scandefintions_t::subtract)
		return 	(FindSignature(mod, modsize, sig, mask)) - mod;

	if (def == scandefintions_t::none)
		return FindSignature(mod, modsize, sig, mask);

}

DWORD ProcMem::FindAddress(DWORD mod, DWORD modsize, BYTE *sig, char *mask, scandefintions_t def, int extra) {
	if (def == scandefintions_t::read)
	{
		DWORD initAddress = FindSignature(mod, modsize, sig, mask);
		DWORD ptrAddress = Read<DWORD>(initAddress + extra);
		return ptrAddress - mod;
	}
}
#pragma endregion