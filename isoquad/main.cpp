#pragma warning(disable:4996)
#include <stdio.h>
#include <string>
//#include <conio.h>
#include <iostream>
#include <serial.h>
#include "glm.hpp"
#include "gtc/quaternion.hpp"
//typedef float tempquat[4];

//#define PIPE
using namespace std;
using namespace glm;
void printArgUsage(char* arg0) {
	printf("Incorrect argument usage!\n Correct usage: %s <port> <baud rate>\n", arg0);
}
serial::Serial* connect(char *cport, char *cbaud = "115200") {
	string port(cport);
	string sbaud(cbaud);
	//Validate baud
	int baud;
	try {
		baud = stoi(sbaud);
	}
	catch (out_of_range) {
		throw out_of_range("Baud rate is out of range.");
	}
	catch (invalid_argument) {
		throw invalid_argument("Baud rate is not a number.");
	}
	if (baud < 0) {
		throw out_of_range("Baud rate is a negative number.");
	}
	try {
		serial::Serial *thisSerial = new serial::Serial(port, baud, serial::Timeout(1000));
		return thisSerial;
	}
	catch (const exception& e) {
		printf("There was a problem creating a Serial connection.\n");
		throw exception(e.what());
	}
}
int main(int argc, char *argv[]) {
	printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n####### ISOTONIC QUADCOPTER #######\n#######    PROGRAMMED BY    #######\n####### DAVID SHUSTIN, 2016 #######\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");
	if (argc < 2 || argc > 3) {
		printArgUsage(argv[0]);
		return -2;
	}
	serial::Serial *SPO;
	if (argc == 2) {
		try {
			SPO = connect(argv[1]);
		}
		catch (const exception& e) {
			cout << e.what() << endl;
			return -1;
		}
	}
	else {
		try {
			SPO = connect(argv[1], argv[2]);
		}
		catch (const exception& e) {
			cout << e.what() << endl;
			return -1;
		}
	}
	printf("Ready to start visualization!\n");
	int key; //keyboard command
	uint8_t buf[1];
	unsigned char ch;
	SPO->write("l"); //begin transmission
	int synced = 0;
	int serialCount = 0;
	unsigned char teapotPacket[14];
	//short teapotShort[8];
	//float q[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
	quat q;
	quat qi;
	quat qa;
	/*vec3 axis;
	float theta;
	vec3 vecForward(0, 0, 1);
	vec3 euler;*/
	do {
		//Parse through glove input
		while (SPO->available()) {
			//Credit to Jeff Rowberg for writing the original Java/Processing code that the following code is ported from.
			//https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_DMP6/Processing/MPUTeapot/MPUTeapot.pde
			if (SPO->read(buf, 1)) {
				ch = (unsigned char)buf[0];
				if (synced == 0 && ch != '$') continue;
				synced = 1;
				if ((serialCount == 1 && ch != 0x02)
					|| (serialCount == 12 && ch != '\r')
					|| (serialCount == 13 && ch != '\n')) {
					serialCount = 0;
					synced = 0;
					continue;
				}
				if (serialCount > 0 || ch == '$') {
					teapotPacket[serialCount++] = (unsigned char)ch;
					if (serialCount == 14) {
						serialCount = 0;
						q[0] = ((teapotPacket[2] << 8) | teapotPacket[3]) / 16384.0f;
						q[1] = ((teapotPacket[4] << 8) | teapotPacket[5]) / 16384.0f;
						q[2] = ((teapotPacket[6] << 8) | teapotPacket[7]) / 16384.0f;
						q[3] = ((teapotPacket[8] << 8) | teapotPacket[9]) / 16384.0f;
						for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i];
						cout << "\n";

					}
				}
			}
		}
		//memset(buffer, '\0', 1024);
	} while (key != 27);
	if (SPO->isOpen())
		SPO->close();
	delete SPO;
	return 0;
}