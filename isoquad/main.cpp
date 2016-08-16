#pragma warning(disable:4996)
#include "SerialClass.h"
#include <stdio.h>
#include <string>
#include <conio.h>
#include <iostream>
#define ARGUMENT_USAGE "\n\nERROR: Incorrect Argument Usage\n\nCorrect Usage: isoquad <Output COM> [Input COM]\n"
#define PRINT_ARG_USAGE printf(ARGUMENT_USAGE)
#define PAUSE printf("\nPress any key to continue...\n"); getch()
typedef union {
	float floatingPoint;
	char binary[4];
} binaryFloat;
using namespace std;
bool COMConnect(char *arg, Serial **SP) {
	char *COMPort = new char[strlen(arg) + 1];
	for (unsigned int i = 0; i < strlen(arg) + 1; ++i) {
		COMPort[i] = arg[i];
	}
	///////////////////////////////////////////////////////////
	//The next two functions are separate because the program//
	//should check if it is attempting to access a value that//
	//   is not within the array.  Just trust me. I think.   //
	///////////////////////////////////////////////////////////
	printf("Verifying COM Port...\n");
	if (strlen(COMPort) < 4) {
		printf("\n%s is not a valid COM Port.\n", COMPort);
		PRINT_ARG_USAGE;
		return 0;
	}
	if (!(COMPort[0] == 'C' && COMPort[1] == 'O' && COMPort[2] == 'M') || atoi(COMPort + 3) == 0) {
		printf("\n%s is not a valid COM Port.\n", COMPort);
		PRINT_ARG_USAGE;
		return 0;
	}
	printf("Verified!\nFormatting COM request...\n");
	string COMFinal = atoi(COMPort + 3) > 9 ? "\\\\.\\" + string(COMPort) : string(COMPort);
	printf("Formatting complete. %s was converted to %s (No change may have occured.  This is normal.)\n", COMPort, COMFinal.c_str());
	*SP = new Serial(const_cast<char*>(COMFinal.c_str()));
	if ((*SP)->IsConnected())
		printf("Connected to COM Port %s!\n", COMFinal.c_str());
	else
		return false;
	delete[] COMPort;
	//PAUSE;
	printf("\n\n");
	return true;
}
Serial *SPO, *SPI = NULL;
int main(int argc, char *argv[]) {
	printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n####### ISOTONIC QUADCOPTER #######\n#######    PROGRAMMED BY    #######\n####### DAVID SHUSTIN, 2016 #######\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");
	if (argc > 3) {
		PRINT_ARG_USAGE;
		return 0;
	}
	if (!COMConnect(argv[1], &SPO)) {
		PAUSE;
		return 0;
	}
	if (argc > 2) {
		if (!COMConnect(argv[2], &SPI)) {
			PAUSE;
			return 0;
		}
	}
	printf("Ready to start transmitting data!");
	PAUSE;
	printf("Press Escape to exit...\n");
	binaryFloat qw, qx, qy, qz;
	qw.floatingPoint, qx.floatingPoint, qy.floatingPoint, qz.floatingPoint = 0.0;
	bool floatsReady = false; //safeguard against using floats before they are defined
	int key; //keyboard command
	char buffer[1024]; //input buffer
	char FlTemp[4]; //float buffer
	int index = 0; //bookmark for FlTemp
	memset(buffer, '\0', 1024); //initialize input buffer
	memset(FlTemp, '\0', 4); //initialize float buffer
	SPO->WriteData("l", 1); //begin transmission
	bool ready = false; //safeguard against split buffer
	char mode; //current float
	do {
		key = 0;
		if (kbhit())
			key = getch();
		/*switch (key) {
		case 59:
			printf("F1 detected.\n");
			break;
		case 60:
			printf("F2 detected.\n");
			break;
		case (int)'w':
			printf("w detected.\n");
			break;
		}*/
		//Parse through glove input
		//TODO: Remove outdated data if program hangs, wait for current data
		if (SPO->ReadData(buffer, 1023)) {
			for (char *cp = buffer; *cp; ++cp) {
				switch (*cp) {
				case 'n':
					ready = true;
					break;
				case 'w':
				case 'x':
				case 'y':
				case 'z':
					if (ready)
						mode = *cp;
					break;
				case 'a':
					if (ready)
						cout << "Quaternion - \t" << qw.floatingPoint << "\t" << qx.floatingPoint << "\t" << qy.floatingPoint << "\t" << qz.floatingPoint << endl;
					ready = false;
					break;
				default: 
					if (ready) {
						if (index == 4) {
							//Flush data
							index = 0;
							switch (mode) {
							case 'w':
								for (int i = 0; i < 4; i++) {
									qw.binary[i] = FlTemp[i];
								}
								break;
							case 'x':
								for (int i = 0; i < 4; i++) {
									qx.binary[i] = FlTemp[i];
								}
								break;
							case 'y':
								for (int i = 0; i < 4; i++) {
									qy.binary[i] = FlTemp[i];
								}
								break;
							case 'z':
								for (int i = 0; i < 4; i++) {
									qz.binary[i] = FlTemp[i];
								}
								break;
							}
						}
						else {
							FlTemp[index] = *cp;
						}
					}
					break;
				}
			}
		}
		memset(buffer, '\0', 1024);
	} while (key != 27);
	delete SPO;
	delete SPI;
	return 0;
}