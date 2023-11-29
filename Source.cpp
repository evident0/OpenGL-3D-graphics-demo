#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION

// Include standard headers
//#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>


#include <algorithm>

#include "Texture.h"
#include "Object.h"

// Include GLEW
#include <glad/glad.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define MAX_CRATERS 1000

using namespace glm;
using namespace std;

glm::vec3 cameraCoords = glm::vec3(3.0f, -10.0f, 10.0f);
//glm::vec3 cameraCoords = glm::vec3(10.0f, 0.0f, 0.0f);
glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

void inputKeys() {
	float zoom = 0;
	float xMove = 0;
	float zMove = 0;
	bool inputGiven = false;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		inputGiven = true;
		xMove = 0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		inputGiven = true;
		xMove = -0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		inputGiven = true;
		zMove = 0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		inputGiven = true;
		zMove = -0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
		inputGiven = true;
		zoom = 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
		inputGiven = true;
		zoom = -0.1f;
	}

	if (inputGiven) {
		upVector = glm::rotate(upVector, glm::radians(xMove), glm::vec3(1.0f, 0.0f, 0.0f));
		upVector = glm::rotate(upVector, glm::radians(zMove), glm::vec3(0.0f, 0.0f, 1.0f));
		cameraCoords = glm::rotate(cameraCoords, glm::radians(xMove), glm::vec3(1.0f, 0.0f, 0.0f));
		cameraCoords = glm::rotate(cameraCoords, glm::radians(zMove), glm::vec3(0.0f, 0.0f, 1.0f));
		cameraCoords += zoom * glm::normalize(glm::vec3(-cameraCoords.x, -cameraCoords.y, -cameraCoords.z));
	}

}

//************************************
// Η LoadShaders είναι black box για σας
//************************************
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
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
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}



int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1000, 1000, u8"Εργασία 1Γ - Καταστροφή", NULL, NULL);


	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	/*
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}*/



	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark green background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("ProjCVertexShader.vertexshader", "ProjCFragmentShader.fragmentshader");

	Texture groundTexture("./textures/ground3.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Texture ballTexture("./textures/fire.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Texture craterTexture("./textures/crater1.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

	Object ball("./objects/ball.obj");
	Object ground("./objects/grid20x20f100x100s.obj");
	Object crater("./objects/crater.obj");

	double prevTime = glfwGetTime();
	double crntTime = glfwGetTime();

	glm::vec3 craters[MAX_CRATERS]; //= {glm::vec3(0.0f,0.0f,0.0f)};
	vector<glm::vec3> ballPositionArray;
	int craterNum = 0;

	glm::vec3 ballPos = glm::vec3(0.0f, 0.0f, 5.0f);
	bool ballFalling = false;
	bool hasPressed = false; // wait for key to be lifted (if we don't use this the key will just spam fireballs)
	bool hasPressedUorP = false; // same here
	float randx;
	float randy;
	float ballSpeed = 0.1f;

	do {
		crntTime = glfwGetTime();
		
		//light settings
		glm::vec4 lightColor = glm::vec4(1.0f, 0.6f, 0.0f, 1.0f);
		glUniform4f(glGetUniformLocation(programID, "lightColor"), lightColor.x, lightColor.y, lightColor.z,
			lightColor.w);
		glUniform3f(glGetUniformLocation(programID, "camPos"), cameraCoords.x, cameraCoords.y, cameraCoords.z);
		glUniform1i(glGetUniformLocation(programID, "pointLightsSize"), 0);

		if (crntTime - prevTime >= 0.01f)
		{
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {//&& !hasPressed
				float randx = -45.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (80.2f)));
				float randy = -45.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (80.2f)));
				ballPos = glm::vec3(randx, randy, 20.0f);
				ballPositionArray.push_back(ballPos);
				glUniform1i(glGetUniformLocation(programID, "arraySize"), ballPositionArray.size());

				ballFalling = true;
				hasPressed = true;
			}
			else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
				hasPressed = false;
			}
			for (int i = 0; i < ballPositionArray.size(); i++) {
			
				if (ballPositionArray[i].z-0.1 < 0.0f) {
					
					craters[craterNum] = glm::vec3(ballPositionArray[i].x, ballPositionArray[i].y, 0.05f);
					ballPositionArray.erase(ballPositionArray.begin() + i);
					glUniform1i(glGetUniformLocation(programID, "arraySize"), ballPositionArray.size());
					
					if (craterNum < MAX_CRATERS - 1) {
						craterNum++;
					}
				
				}
				else {
					
					std::string uniformArray = "pointLights[" + std::to_string(i) + "].position";
					glUniform3f(glGetUniformLocation(programID, uniformArray.c_str()), ballPositionArray[i].x, ballPositionArray[i].y, ballPositionArray[i].z);
					
					if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !hasPressedUorP) {
					
						ballSpeed += 0.02f;

						hasPressedUorP = true;
					}
					else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !hasPressedUorP ) {

						
						ballSpeed += -0.02f;

						hasPressedUorP = true;

					}
					else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE) {
						
						hasPressedUorP = false;
					
					}
								
					ballPositionArray[i].z -= ballSpeed;
					
				}
			
			}
			prevTime = crntTime;
		}
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 1000.0f);

		inputKeys();

		glm::mat4 ViewMatrix = glm::lookAt(
			cameraCoords,
			glm::vec3(0.0f, 0.0f, 0.0f),
			upVector
		);

		glm::mat4 VP = ProjectionMatrix * ViewMatrix;

		GLuint VPID = glGetUniformLocation(programID, "VP");

		glUniformMatrix4fv(VPID, 1, GL_FALSE, &VP[0][0]);

		ground.draw(groundTexture,glm::vec3(0.0f,0.0f, 0.0f), programID);

		for (int i = 0; i < ballPositionArray.size(); i++) {
		
			ball.draw(ballTexture, ballPositionArray[i], programID);
		
		}

		glDepthMask(GL_FALSE);// this should fix z fighting
		for (int i = 0; i < craterNum+1; i++) {
			//cout << "I placed a crater in position: " << craters[i].x << ", " << craters[i].y << ", " << craters[i].z << endl;
			crater.draw(craterTexture, craters[i], programID);
		}
		glDepthMask(GL_TRUE);
		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteProgram(programID);
	groundTexture.del();
	ballTexture.del();
	ball.del();
	ground.del();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;

}

