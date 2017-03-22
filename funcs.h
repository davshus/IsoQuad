#ifndef FUNCS
#define FUNCS
#include <chrono>
#include <stdio.h>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <thread>
#include "serial/serial.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "axes.h"
using namespace std;
using namespace glm;
extern int width;
extern int height;
extern mat4 Projection;
extern mat4 Scale;
extern float baseScale;
//Credit to user79758 on Stack Overflow (http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c) for this function
template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
serial::Serial* connect(char *cport, const char *cbaud = "115200") {
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
		serial::Serial *thisSerial = new serial::Serial(port, baud, serial::Timeout::simpleTimeout(1000));
		return thisSerial;
	}
	catch (const exception& e) {
		printf("There was a problem creating a Serial connection.\n");
		throw runtime_error(e.what());
	}
}
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {


	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if (FragmentShaderStream.is_open()) {
		string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
//Why does the cross product give weird results when the vectors are close together?
float relRoll(axes world, axes object) {
	//Will return 0 if the ground is out of range when proximity is implemented.
	//vec3 crss = normalize((rotation(vec3(object.forward.x, 0, object.forward.z), vec3(-1, 0, 0)) * object.forward).z > 0 ? cross(object.forward, world.up) : cross(world.up, object.forward)); //If object.forward is behind world.up, do the opposite cross
	vec3 crss = normalize(cross(object.forward, world.up));
	cout << "(" << crss.x << "\t" << crss.y << "\t" << crss.z << ")\t(" << object.right.x << "\t" << object.right.y << "\t" << object.right.z << ")\t";
	return degrees(orientedAngle(crss, object.right, object.forward));
}
//Callbacks
void resize(GLFWwindow *window, int WIDTH, int HEIGHT) {
	width = WIDTH;
	height = HEIGHT;
	Projection = glm::perspective(glm::radians(70.0f), (float)width / (float)height, 0.1f, 100.0f);
	Scale = scale(vec3(baseScale));
	glViewport(0, 0, width, height);
}
#endif