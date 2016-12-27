#pragma warning(disable:4996)
//YES, I KNOW THAT MY COORDINATE SYSTEM IS A MONSTROSITY. I STARTED THIS WITHOUT KNOWING ANYTHING ABOUT WHAT TO DO AND NOW I'M TOO SCARED TO CHANGE IT UNTIL A REWRITE. PLEASE DON'T KILL ME.
//AND YES, I KNOW, NOTHING HERE IS SAFE AGAINST vec3(0, 0, 0). PLEASE JUST DON'T JUDGE ME. MAYBE I'LL FIX IT IN THE REWRITE.
#include "funcs.h"
#include "axes.h"
int width = 1920;
int height = 1080;
float baseScale = 2.0f;
mat4 Scale = scale(vec3(baseScale));
mat4 Projection;
void printArgUsage(char* arg0) {
	printf("Incorrect argument usage!\n Correct usage: %s <port> <baud rate>\n", arg0);
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
	//Start OpenGL
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window;
	const char* windowTitle = "IsoQuad";
	window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW Window.\nIf you have an Intel GPU, you can't play No Man's Sky, though why would you want to?.\nAlso, you should use v2.1 instead.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, resize);
	glfwSetWindowPos(window, 100, 100);
	//glfwSetWindowAspectRatio(window, 16, 9);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW.\n");
		return -1;
	}
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	static const GLfloat g_vertex_buffer_data[] = {
		0.433f, 0.0f, 0.25f,
		-0.433f, 0.0f, 0.25f,
		0.0f,  0.0f, -0.5f,
		0.433f, 0.0f, -0.5f,
		-0.433f, 0.0f, -0.5f,
		0.0f, 0.5f, -0.5f
	};
	/*static const GLfloat g_color_buffer_data[] = {
		0.4f, 0.6f, 1.0f,
		0.4f, 0.6f, 1.0f,
		1.0f, 1.0f, 1.0f
	};*/
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	/*GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);*/
	Projection = glm::perspective(glm::radians(85.0f), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	GLuint programID = LoadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");
	GLuint programID2 = LoadShaders("SimpleVertexShader2.vert", "SimpleFragmentShader2.frag");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	int key; //keyboard command
	uint8_t buf[1];
	unsigned char ch;
	SPO ->write("l"); //begin transmission
	int synced = 0;
	int serialCount = 0;
	unsigned char teapotPacket[14];
	//short teapotShort[8];
	//float q[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
	quat mpu;
	quat ogl;
	quat ogli;
	quat oglf;
	//quat qa;
	quat quatIdentity;
	//chrono::time_point<chrono::steady_clock> pastTime;
	//chrono::milliseconds deltaTime;
	chrono::time_point<chrono::steady_clock> atSerial = chrono::steady_clock::now();
	chrono::milliseconds sinceSerial;
	bool hanging = false;
	bool perspective = true;
	//chrono::nanoseconds nanoTime;
	/*vec3 axis;
	float theta;
	vec3 vecForward(0, 0, 1);
	vec3 euler;*/
	//bool fullscreen = false;
	const vec3 up = vec3(0, 1, 0);
	const vec3 down = vec3(0, -1, 0);
	const vec3 right = vec3(1, 0, 0);
	const vec3 left = vec3(-1, 0, 0);
	const vec3 back = vec3(0, 0, 1);
	const vec3 forward = vec3(0, 0, -1);
	axes reference, object;
	do {
		//pastTime = chrono::steady_clock::now();
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
						mpu.w = ((teapotPacket[2] << 8) | teapotPacket[3]) / 16384.0f;
						mpu.x = ((teapotPacket[4] << 8) | teapotPacket[5]) / 16384.0f;
						mpu.y = ((teapotPacket[6] << 8) | teapotPacket[7]) / 16384.0f;
						mpu.z = ((teapotPacket[8] << 8) | teapotPacket[9]) / 16384.0f;
						for (int i = 0; i < 4; i++) if (mpu[i] >= 2) mpu[i] = -4 + mpu[i];

					}
				}
			}
			atSerial = chrono::steady_clock::now();
			if (hanging)
				hanging = false;
		}
		//memset(buffer, '\0', 1024);
		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
		mpu = normalize(mpu);
		ogl.w = mpu.w;
		ogl.x = mpu.x;
		ogl.y = mpu.z;
		ogl.z = -mpu.y;
		if (glfwGetKey(window, GLFW_KEY_F8) == GLFW_PRESS) {
			ogl.w = 0.707;
			ogl.x = 0.707;
			ogl.y = 0;
			ogl.z = 0;
		}
		ogl = normalize(ogl);
		if (glfwGetKey(window, GLFW_KEY_F9) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			ogli = inverse(ogl);
		oglf = normalize(ogli * ogl);
		object.calculate(oglf);
		reference.calculate(quat());
		cout << relRoll(reference, object) << '\r';
		Model = toMat4(oglf) * Scale;
		MVP = Projection * View * Model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);
		glUseProgram(programID2);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 3, 3);
		glDisableVertexAttribArray(0);
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		//deltaTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - pastTime);
		sinceSerial = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - atSerial);
		if (sinceSerial.count() > 1000) {
			if (!hanging) {
				printf("A problem occurred, causing data transfer to hang.  Please wait while the system is revived.\n");
				hanging = true;
			}
			printf("Sending REVIVE message...\n");
			try {
				SPO->write("l");
			}
			catch (serial::PortNotOpenedException &e) {
				printf("PortNotOpenedException: %s\n", e.what());
			}
			catch (serial::SerialException &e) {
				printf("SerialException: %s\n", e.what());
			}
			catch (serial::IOException &e) {
				printf("IOException: %s\n", e.what());
			}
			this_thread::sleep_for(chrono::milliseconds(500));
		}
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
			Projection = glm::ortho(-width/1000.0f, width/1000.0f, -height/1000.0f, height/1000.0f, 0.0f, 50.0f);
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
			Projection = glm::perspective(glm::radians(85.0f), (float)width / (float)height, 0.1f, 100.0f);
		/*if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
			this_thread::sleep_for(chrono::milliseconds(500));
			fullscreen = !fullscreen;
			const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwSetWindowMonitor(window, //Window
				fullscreen ? glfwGetPrimaryMonitor() : nullptr, //Fullscreen if currently windowed, windowed if currently fullscreen
				0, 0, //Origin
				vidmode->width, vidmode->height, //Height and Width
				GLFW_DONT_CARE //Refresh Rate
			);
			//glViewport(0, 0, vidmode->width, vidmode->height);
		}*/
	} while (SPO->isOpen() && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
	if (!SPO->isOpen())
		cout << "Program terminated because the serial port was closed.";
	else
		SPO->close();
	delete SPO;
	return 0;
}
