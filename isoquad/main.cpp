#pragma warning(disable:4996)
#include "SerialClass.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include "stdio.h"
#include <windows.h>
#include <tchar.h>
#include <vector>
#include <conio.h>
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define ARGUMENT_USAGE "\n\nERROR: Incorrect Argument Usage\n\nCorrect Usage: %s <COMPort>\n"
#define PRINT_ARG_USAGE printf(ARGUMENT_USAGE, argv[0])
#define ESC 27
#define F1 59
#define F2 60
#define F3 61
#define F4 62
#define F5 63
#define F6 64
#define F7 65
#define F8 66
#define F9 67
#define F10 68
#define HOME 71
#define UP 72
#define PAGE_UP 73
#define LEFT 75
#define RIGHT 77
#define END 79
#define DOWN 80
#define PAGE_DOWN 81
using namespace std;
int port;
int keyPressed;
int main(int argc, char *argv[]) {
	//Parse Commands
	HKEY hKey = HKEY_LOCAL_MACHINE;
	char *COMPort = "";
	bool greaterThanNine = false;
	bool argsRead = false;
	if (argc != 2) {
		PRINT_ARG_USAGE;
		return 0;
	}
	printf("\n===================================\n=%%=%%=%%==ISOTONIC QUADCOPTER==%%=%%=%%=\n=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=\n=%%=%%=%%==DAVID SHUSTIN, 2016==%%=%%=%%=\n===================================\n\n");
	COMPort = (char*)malloc(strlen(argv[1])+1);
	strcpy(COMPort, argv[1]);
	if (strlen(COMPort) > 4) {
		greaterThanNine = true;
	}
	//Sleep(10000);
	//cout << "Arduino Communication Test" << endl << "What port should this computer connect to?" << endl;
	//cin >> port;
	printf("Connecting to COM Port %s\n", COMPort);
	char *COMFinal;
	if (greaterThanNine) {
		COMFinal = (char*)malloc(strlen("\\\\.\\") + strlen(COMPort) + 1);
		strcpy(COMFinal, "\\\\.\\");
		strcat(COMFinal, COMPort);
	}
	else {
		COMFinal = COMPort;
	}
	Serial* SP = new Serial(COMFinal);
	if (SP->IsConnected()) {
		printf("Connected to COM Port %s!\n", COMFinal);
	}
	else {
		printf("ERROR");
	}
	system("pause");
	/*if (SP->WriteData("abcdefghijklmnopqrstuvwxyz", 27))
		printf("SUCCESS SENDING\n");
	system("pause");
	char incomingData[256] = "";			//don't forget to pre-allocate memory
											//printf("%s\n",incomingData);
	int dataLength = 255;
	int readResult = 0;*/
	printf("Ready to transmit commands!\nPress Esc to exit.\n");
	do {
		keyPressed = getch();
		if (keyPressed == PAGE_UP) {
			SP->WriteData("r", 2);
		}
		else if (keyPressed == PAGE_DOWN) {
			SP->WriteData("g", 2);
		}
		else if (keyPressed == F1) {
			SP->WriteData("o", 2);
		}
	} while (keyPressed != ESC);
	SP->WriteData("o", 2);
	printf("\nClosing COM Port...\n");
	SP->~Serial();
	printf("All Ports Closed! Please wait 3 seconds.");
	Sleep(3000);
	return 0;
}