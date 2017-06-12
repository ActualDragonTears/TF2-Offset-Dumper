#pragma once
#define WIN32_LEAN_AND_MEAN //Excludes Headers We Wont Use (Increase Compile Time)

#include <windows.h> //Standard Windows Functions/Data Types
#include <iostream> //Constains Input/Output Functions (cin/cout etc..)
#include <TlHelp32.h> //Contains Read/Write Functions
#include <string> //Support For Strings
#include <sstream> //Supports Data Conversion

enum scandefintions_t
{
	none,
	read,
	subtract
};

class ProcMem {
protected:

	//STORAGE
	HANDLE hProcess;
	DWORD dwPID, dwProtection, dwCaveAddress;

	//MISC
	BOOL bPOn, bIOn, bProt;

public:

	//MISC FUNCTIONS
	ProcMem();
	~ProcMem();

	//READ MEMORY 
	template <class cData>
	cData Read(DWORD dwAddress)
	{
		cData cRead; //Generic Variable To Store Data
		ReadProcessMemory(hProcess, (LPVOID)dwAddress, &cRead, sizeof(cData), NULL); //Win API - Reads Data At Specified Location 
		return cRead; //Returns Value At Specified dwAddress
	}
	template <class cData>
	cData ReadString(DWORD dwAddress)
	{
		cData csRead; //Generic Variable To Store Data
		ReadProcessMemory(hProcess, (LPVOID)dwAddress, &csRead, 32 * sizeof(cData), NULL); //Win API - Reads Data At Specified Location 
		return csRead; //Returns Value At Specified dwAddress
	}

	//READ MEMORY - Pointer
	template <class cData>
	cData Read(DWORD dwAddress, char *Offset, BOOL Type)
	{
		//Variables
		int iSize = iSizeOfArray(Offset) - 1; //Size Of *Array Of Offsets 
		dwAddress = Read<DWORD>(dwAddress); //HEX VAL

											//Loop Through Each Offset & Store Hex Value (Address)
		for (int i = 0; i < iSize; i++)
			dwAddress = Read<DWORD>(dwAddress + Offset[i]);

		if (!Type)
			return dwAddress + Offset[iSize]; //FALSE - Return Address
		else
			return Read<cData>(dwAddress + Offset[iSize]); //TRUE - Return Value
	}

	//MEMORY FUNCTION PROTOTYPES
	virtual void Process(char* ProcessName); //Return Handle To The Process
	DWORD FindAddress(DWORD mod, DWORD modsize, BYTE * sig, char * mask, scandefintions_t def);
	DWORD FindAddress(DWORD mod, DWORD modsize, BYTE * sig, char * mask, scandefintions_t def, int extra);
	virtual bool DataCompare(BYTE * data, BYTE * sign, char * mask);
	virtual DWORD FindSignature(DWORD base, DWORD size, BYTE* sign, char* mask);
	virtual DWORD Module(LPSTR ModuleName); //Return Module Base Address
	virtual DWORD ModuleSize(LPSTR ModuleName); //Return Module Base Address

#pragma endregion	

};