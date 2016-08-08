#pragma warning(disable:4996)
#include "SerialClass.h"
//#include <iostream>
#include <stdio.h>
#include <string>
#include <windows.h>
#include <tchar.h>
#include <vector>
#include <conio.h>
//#include <chrono>
#include <atlstr.h>
#include <boost/shared_ptr.hpp>
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define ARGUMENT_USAGE "\n\nERROR: Incorrect Argument Usage\n\nCorrect Usage: %s <Output COM> [Input COM]\n"
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
//using namespace std::chrono;
//int port;
int keyPressed;
/*int message = 0;
CString tempMsg;
char *msgStr;
vector<int> msgs;
vector<milliseconds> timestamps;
milliseconds currentTime;*/

int test;
int main(int argc, char *argv[]) {
	printf("\n===================================\n=%%=%%=%%==ISOTONIC QUADCOPTER==%%=%%=%%=\n=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=%%=\n=%%=%%=%%==DAVID SHUSTIN, 2016==%%=%%=%%=\n===================================\n\n");
	//Parse Commands
	//HKEY hKey = HKEY_LOCAL_MACHINE;
	char *COMPort = NULL;
	char *COMFinal = NULL;
	//bool greaterThanNine = false;
	//bool argsRead = false;
	if (argc > 3) {
		PRINT_ARG_USAGE;
		return 0;
	}
	delete[] COMPort;
	COMPort = (char*)malloc(strlen(argv[1]) + 1);
	strcpy(COMPort, argv[1]);
	if (strlen(COMPort) < 4 || !(COMPort[0] == 'C' && COMPort[1] == 'O' && COMPort[2] == 'M') || atoi(COMPort+3) == 0) {
		printf("\n%s is not a valid COM Port.\n", COMPort);
		PRINT_ARG_USAGE;
		return 0;
	}
	
	//int test;
	/*if (strlen(COMPort) > 4) {
		greaterThanNine = true;
	}*/
	//Sleep(10000);
	//cout << "Arduino Communication Test" << endl << "What port should this computer connect to?" << endl;
	//cin >> port;
	printf("Connecting to COM Port %s\n", COMPort);
	
	if (atoi(COMPort+3) > 9) {
		free(COMFinal);
		COMFinal = (char*)malloc(strlen("\\\\.\\") + strlen(COMPort) + 1);
		strcpy(COMFinal, "\\\\.\\");
		strcat(COMFinal, COMPort);
	}
	else {
		COMFinal = COMPort;
	}
	Serial* SP1 = new Serial(COMFinal);
	if (SP1->IsConnected()) {
		printf("Connected to COM Port %s!\n", COMFinal);
	}
	else {
		printf("ERROR\n");
		system("pause");
		return 0;
	}
	system("pause");
	if (argc > 2) {
		printf("Attempting secondary COM connection to %s...\n", argv[2]);
		//Second COM port initializer
		//TODO: Unify COM initialization to single function?
		//bool greaterThanNine = false;
		//bool argsRead = false;

		realloc(COMPort, strlen(argv[2]) + 1);
		strcpy(COMPort, argv[2]);
		if (strlen(COMPort) < 4 || !(COMPort[0] == 'C' && COMPort[1] == 'O' && COMPort[2] == 'M') || atoi(COMPort + 3) == 0) {
			printf("\n%s is not a valid COM Port.\n", COMPort);
			PRINT_ARG_USAGE;
			return 0;
		}

		//int test;
		/*if (strlen(COMPort) > 4) {
		greaterThanNine = true;
		}*/
		//Sleep(10000);
		//cout << "Arduino Communication Test" << endl << "What port should this computer connect to?" << endl;
		//cin >> port;
		printf("Connecting to COM Port %s\n", COMPort);

		if (atoi(COMPort + 3) > 9) {
			//free(COMFinal);
			COMFinal = (char*)malloc(strlen("\\\\.\\") + strlen(COMPort) + 1);
			strcpy(COMFinal, "\\\\.\\");
			strcat(COMFinal, COMPort);
		}
		else {
			COMFinal = COMPort;
		}
		Serial* SP2 = new Serial(COMFinal);
		if (SP2->IsConnected()) {
			printf("Connected to COM Port %s!\n", COMFinal);
		}
		else {
			printf("ERROR\n");
			system("pause");
			return 0;
		}
		//free(COMFinal);
		//free(COMPort);
		system("pause");
	}
	//char incomingData[255];
	printf("Ready to transmit commands!\nPress Esc to exit.\n");
	do {
		keyPressed = 0;
		if (kbhit())
			keyPressed = getch();
		switch (keyPressed) {
		case UP:
			if (SP1->WriteData("0", 1))
				puts("Increment successful!");
			else
				puts("Increment failed...");
			puts("\n");
			break;
		case DOWN:
			if (SP1->WriteData("9", 1))
				printf("Decrement successful!");
			else
				printf("Decrement failed...");
			puts("\n");
			break;
		case PAGE_UP:
		case PAGE_DOWN:
		case HOME:
		case END:
			if (SP1->WriteData("8", 1))
				puts("Motor power cut!");
			else
				puts("Motor power cut unsuccessful...");
			puts("\n");
			break;
		}
	} while (keyPressed != ESC);
	//SP1->WriteData("o", 2);
	printf("\nClosing COM Port...\n");
	SP1->~Serial();
	printf("All Ports Closed!");
	return 0;
}