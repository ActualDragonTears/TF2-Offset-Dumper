#include "Offsets.h"
#include "ProcMem.h"
#include <fstream>
#include <stdio.h>
#include <ctype.h>

using std::cout;
using std::endl;
using std::uppercase;
using std::hex;

ProcMem Mem;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
bool close = false;

void PrintOffset(char* gname, DWORD offset) {

	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSE_WHITE);
	cout << gname << " : ";
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
	cout << "0x";
	cout << hex << uppercase << offset << endl;
}

void Save() {
	std::ofstream saveF; 

	saveF.open("tf2_offsets.txt"); //Defines the file

	//Formatting things

	saveF << "[TF2 Offset Dump | ";
	saveF << __DATE__;
	saveF << " ";
	saveF << __TIME__;
	saveF << "] \n\n";

	//Formatting the offsets

	saveF << "dwLocalPlayer : 0x" << hex << uppercase << Offset.dwLocalPlayer << "\n";
	saveF << "dwEntityList : 0x" << hex << uppercase << Offset.dwEntityList << "\n\n";

	saveF << "dwJump : 0x" << hex << uppercase << Offset.dwJump << "\n";
	saveF << "dwAttack : 0x" << hex << uppercase << Offset.dwAttack << "\n";
	saveF << "dwAttack2 : 0x" << hex << uppercase << Offset.dwAttack2 << "\n";

	//Closed the files connection

	saveF.close();
}

int main()
{
	//Resizing and formatting the window, it's important.
	SetConsoleTitle("TF2 Offset Dumper by ActualCheats");
	HWND window = GetConsoleWindow();
	MoveWindow(window, 10, 10, 670, 400, true);

	//Checking if the game is open

	if (!FindWindow(NULL, "Team Fortress 2")) {
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		cout << "Please Open TF2" << endl;
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSE_WHITE);
	}

	while (!FindWindow(NULL, "Team Fortress 2")) { Sleep(500); }

	system("cls"); //Clears the console

	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
	cout << "TF2 has been found." << endl << endl;
	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSE_WHITE);

	//Hooking the main game innit
	Mem.Process("hl2.exe");

	//Getting module addresses
	while (!Offset.dwClient && !Offset.dwEngine) {
		Offset.dwClient = Mem.Module("client.dll"); // Get the module
		Offset.dwEngine = Mem.Module("engine.dll");
		Offset.dwClientSize = Mem.ModuleSize("client.dll"); // Get the module size
		Offset.dwEngineSize = Mem.ModuleSize("engine.dll");
	}

	//Pattern Scanning for Local Player then printing.
	Offset.dwLocalPlayer = Mem.FindAddress(Offset.dwClient , Offset.dwClientSize, (PBYTE)"\xC7\x00\x00\x01\x00\x00\x00\x4C\xE9", "x??x???xx") + 0x9F; //Sig scanning for localplayer address.
	PrintOffset("dwLocalPlayer", Offset.dwLocalPlayer); // Printing the offset for formatting

	Offset.dwEntityList = Mem.FindAddress(Offset.dwClient, Offset.dwClientSize, (PBYTE)"\x00\x00\x00\x00\x00\x00\xF4\x00\x00\x00\x14\xD3", "??????x???xx") + 0x12E; //Sig scanning for entitylist address.
	PrintOffset("dwEntityList", Offset.dwEntityList); // Printing the offset for formatting

	cout << endl;

	Offset.dwJump = Mem.FindAddress(Offset.dwClient, Offset.dwClientSize, (PBYTE)"\x98\xC6\00\x00\xF8\xEC", "xx??xx") - 0xB8; // Sig scanning for jump
	PrintOffset("dwJump", Offset.dwJump); // Printing the offset for formatting

	Offset.dwAttack = Mem.FindAddress(Offset.dwClient, Offset.dwClientSize, (PBYTE)"\x98\xC6\00\x00\xF8\xEC", "xx??xx") - 0xAC; // Sig scanning for attack
	PrintOffset("dwAttack", Offset.dwAttack); // Printing the offset for formatting

	Offset.dwAttack2 = Mem.FindAddress(Offset.dwClient, Offset.dwClientSize, (PBYTE)"\x98\xC6\00\x00\xF8\xEC", "xx??xx") - 0xA0; // Sig scanning for attack2
	PrintOffset("dwAttack2", Offset.dwAttack2); // Printing the offset for formatting

	cout << endl;

	char yn;

	SetConsoleTextAttribute(hConsole, FOREGROUND_CYAN);

	cout << "Do you want to save to a file [Y] Yes | [N] No : ";
	std::cin >> yn;

	if (yn == 'Y' || yn == 'y')
		Save();

	cout << "Bye!";
	Sleep(300);

}
