#pragma warning(disable:4996)
#include <stdio.h>
#include <string>
#include <conio.h>
#include <iostream>
#include <serial.h>
#include <mathfu/quaternion.h>
//typedef float tempquat[4];
#define	EXIT pause(); return 0
#define PIPE
//#define QUATERNION
using namespace std;
void pause() {
	printf("Press any key to continue.");
	_getch();
	printf("\n");
}
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
#ifndef PIPE
	printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n####### ISOTONIC QUADCOPTER #######\n#######    PROGRAMMED BY    #######\n####### DAVID SHUSTIN, 2016 #######\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");
#endif
	if (argc < 2 || argc > 3) {
		printArgUsage(argv[0]);
		EXIT;
	}
	serial::Serial *SPO;
	if (argc == 2) {
		try {
			SPO = connect(argv[1]);
		}
		catch (const exception& e) {
			cout << e.what() << endl;
			EXIT;
		}
	}
	else {
		try {
			SPO = connect(argv[1], argv[2]);
		}
		catch (const exception& e) {
			cout << e.what() << endl;
			EXIT;
		}
	}
#ifndef PIPE
	printf("Ready to start transmitting data!\n");
	pause();
	printf("Press Escape to exit...\n");
#endif
	int key; //keyboard command
	uint8_t buf[1];
	char ch;
	SPO->write("l"); //begin transmission
	int synced = 0;
	int serialCount = 0;
	char teapotPacket[14];
	short teapotShort[8];
	//float q[4] = { 1, 0, 0, 1 };
	mathfu::Quaternion<float> q(mathfu::Quaternion<float>::identity);
	mathfu::Quaternion<float> qi(mathfu::Quaternion<float>::identity);
	mathfu::Quaternion<float> qa(mathfu::Quaternion<float>::identity);
	mathfu::Vector<float, 3> axis;
	float theta;
	mathfu::Vector<float, 3> vecForward(0, 0, 1);
	mathfu::Vector<float, 3> euler;
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
		/*switch (key) {
		case (int)'w':
			qi = q.Inverse();
			break;
		}*/
		//Parse through glove input
		while (SPO->available()) {
			//Credit to Jeff Rowberg for writing the original Java/Processing code that the following code is ported from.
			//https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_DMP6/Processing/MPUTeapot/MPUTeapot.pde
			if (SPO->read(buf, 1)) {
				ch = (char)buf[0];
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
					teapotPacket[serialCount++] = (char)ch;
					if (serialCount == 14) {
						serialCount = 0;
						for (int x = 0; x < 8; x++)
							teapotShort[x] = teapotPacket[x + 2];
						q[0] = ((teapotShort[0] << 8) | teapotShort[1]) / 16384.0f;
						q[1] = ((teapotShort[2] << 8) | teapotShort[3]) / 16384.0f;
						q[2] = ((teapotShort[4] << 8) | teapotShort[5]) / 16384.0f;
						q[3] = ((teapotShort[6] << 8) | teapotShort[7]) / 16384.0f;
						for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i];
						q.Normalize();
						//vec = q.ToMatrix() * vecForward;
						//vec.Normalize();
						//cout << "Vector:\t" << vec[0] << "\t" << vec[1] << "\t" << vec[2] << endl;
						//q = q * qi;

//#define DELAYED
#ifdef EULER_ANGLES
						euler = q.ToEulerAngles();
						printf("Eulers:");
						for (int i = 0; i < 3; i++) {
							euler[i] *= 180;
							euler[i] /= M_PI;
							euler[i] += 180;
							printf("\t%f", euler[i]);
						}
						printf("\n");
#endif
#ifdef AXIS_ANGLE
						q.ToAngleAxis(&theta, &axis);
						printf("Axis-Angle:\t%f\t%f\t%f\t%f\n", theta, axis[0], axis[1], axis[2]);
#endif
#ifdef QUATERNION
						cout << "Quaternion:\t" << q[0] << "\t" << q[1] << "\t" << q[2] << "\t" << q[3] << endl;
#endif
#ifdef PIPE
						printf("%f\t%f\t%f\t%f\n", q[0], q[1], q[2], q[3]);
#endif
#ifdef DELAYED
						_sleep(500);
						SPO->flush();
#endif
					}
				}
			}
		}
		//memset(buffer, '\0', 1024);
	} while (key != 27);
	if (SPO->isOpen())
		SPO->close();
	delete SPO;
	EXIT;
}
