//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#define TINYOBJLOADER_IMPLEMENTATION
#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp" 
#include "Camera.hpp"
#include "Model3D.hpp"
#include "Mesh.hpp"
#include "Skybox.hpp"
 

int glWindowWidth = 1240;
int glWindowHeight = 620;
int retina_width, retina_height;
const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
GLFWwindow* glWindow = NULL;
GLfloat lastX = glWindowWidth/2, lastY = glWindowHeight/2;
GLfloat pitch=0, yaw=0;
bool firstMouse=true;
GLfloat fov;
GLdouble viewRotAngle;

glm::mat4 model, model_lamps, model_reflect;
glm::mat4 view, view_lamps, view_reflect;
glm::mat4 projection, projection_lamps, projection_reflect;
glm::mat3 normalMatrix, normalMatrix_reflect;
glm::mat3 lightDirMatrix;
glm::mat4 view_spinCam;

GLuint modelLoc, model_lamps_Loc, model_reflect_Loc;
GLuint viewLoc, view_lamps_Loc, view_reflect_Loc;
GLuint projectionLoc, projection_lamps_Loc, projection_reflect_Loc;
GLuint normalMatrixLoc, normalMatrix_reflect_Loc;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
glm::vec3 lightColor;
glm::vec3 lightPoint;
glm::vec3 lightPoint2;
glm::vec4 lampColor;
glm::vec3 yellow = glm::vec3(1.0f, 0.843137f, 0.0f);
glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
float colorStrength = 0.0f; // lamp color constant
float cubePointLight_x_moved = 0.0f, cubePointLight_y_moved = 0.0f, cubePointLight_z_moved = 0.0f;
bool generateRedCube = false;

GLuint lightDirLoc;
GLuint lightColorLoc;
GLuint lightPointLoc;
GLuint lightPointLoc2;
GLuint lampColorLoc;

// activate/deactivate lights
GLint activeDirL;
GLint activeDirLLoc;
GLint activePointL;
GLint activePointLLoc;

GLfloat ambientStrength_Point=0.0;
GLuint ambientStrength_Loc;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool demo=false;

//                      - Position-                 -Target-
gps::Camera myCamera(glm::vec3(0.0f, 5.0f, 15.0f), glm::vec3(0.0f, 2.0f, -10.0f));
float cameraSpeed; // = 0.18f;
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


bool pressedKeys[1024];
float angle = 0.0f;
int renderMode = 0;
bool increase = true;

gps::Model3D teapot, ground, grass, tree, fairy_house, stone, fairy, lightCube, sphere, mushroom_amanita, mushroom_chant, mushroom_boletus;
gps::Shader myCustomShader, lampShader, skyboxShader, depthMapShader, reflectShader;
gps::SkyBox mySkyBox;
std::vector<const GLchar*> faces;


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to custom shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lampShader.useShaderProgram();
	
	
	//send matrix data to shader-lamps
	projection_lamps = projection;
	GLint projLoc2 = glGetUniformLocation(lampShader.shaderProgram, "projection_lamps");
	glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection_lamps));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;

			if (pressedKeys[GLFW_KEY_ENTER]) {
				demo = true;
			}

			if (pressedKeys[GLFW_KEY_O]) {
				if (activeDirL == 1)
					activeDirL = 0;
				else 
					activeDirL = 1;
			}
			if (pressedKeys[GLFW_KEY_P]) {
				if (activePointL == 1)
					activePointL = 0;
				else activePointL = 1;
			}
			if (pressedKeys[GLFW_KEY_M]) {
				if (renderMode < 3)
					renderMode++;
				else renderMode = 0;
			}
		}
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)  //fov-field of view
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // bool var initially true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	
	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	myCamera.rotate(pitch, yaw);
}

void processMovement()
{
	if (!pressedKeys[GLFW_KEY_Q] && !pressedKeys[GLFW_KEY_E])
		view = myCamera.getViewMatrix();

	if (pressedKeys[GLFW_KEY_Q]) {
		viewRotAngle+= 0.009f;
		view = myCamera.spinOnCircle(viewRotAngle);
	}

	if (pressedKeys[GLFW_KEY_E]) {
		viewRotAngle-= 0.009f;
		view = myCamera.spinOnCircle(viewRotAngle);
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}
	//directional light moving
	if (pressedKeys[GLFW_KEY_C] && pressedKeys[GLFW_KEY_1]) {
		lightDir.x += 0.08f;
	}
	if (pressedKeys[GLFW_KEY_X] && pressedKeys[GLFW_KEY_1]) {
		lightDir.x -= 0.08f;
	}
	if (pressedKeys[GLFW_KEY_Y] && pressedKeys[GLFW_KEY_1]) {
		lightDir.y += 0.08f;
	}
	if (pressedKeys[GLFW_KEY_V] && pressedKeys[GLFW_KEY_1]) {
		lightDir.y -= 0.08f;
	}
	if (pressedKeys[GLFW_KEY_Z] && pressedKeys[GLFW_KEY_1]) {
		lightDir.z += 0.08f;
	}
	if (pressedKeys[GLFW_KEY_B] && pressedKeys[GLFW_KEY_1]) {
		lightDir.z -= 0.08f;
	}
	//point light moving
	if (pressedKeys[GLFW_KEY_C] && pressedKeys[GLFW_KEY_2]) {
		lightPoint.x += 0.08f;
		cubePointLight_x_moved += 0.3f;
	}
	if (pressedKeys[GLFW_KEY_X] && pressedKeys[GLFW_KEY_2]) {
		lightPoint.x -= 0.08f;
		cubePointLight_x_moved -= 0.3f;
	}
	if (pressedKeys[GLFW_KEY_Y] && pressedKeys[GLFW_KEY_2]) {
		lightPoint.y += 0.08f;
		cubePointLight_y_moved += 0.3f;
	}
	if (pressedKeys[GLFW_KEY_V] && pressedKeys[GLFW_KEY_2]) {
		lightPoint.y -= 0.08f;
		cubePointLight_y_moved -= 0.3f;
	}
	if (pressedKeys[GLFW_KEY_Z] && pressedKeys[GLFW_KEY_2]) {
		lightPoint.z += 0.08f;
		cubePointLight_z_moved += 0.3f;
	}
	if (pressedKeys[GLFW_KEY_B] && pressedKeys[GLFW_KEY_2]) {
		lightPoint.z -= 0.08f;
		cubePointLight_z_moved -= 0.3f;
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetScrollCallback(glWindow, scroll_callback);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	//glClearColor(0.5, 0.5, 0.5, 1.0); //FOG
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initModels()
{
	//teapot = gps::Model3D("objects/teapot/teapot4segU.obj", "objects/teapot/");
	ground = gps::Model3D("objects/ground/ground.obj", "objects/ground/");
	grass = gps::Model3D("objects/Grass/Grass_02.obj", "objects/grass/");
	tree = gps::Model3D("objects/tree/tree2.obj", "objects/tree/");
	fairy_house = gps::Model3D("objects/fairy_house/House_new.obj", "objects/fairy_house/");
	stone = gps::Model3D("objects/Stone_Forest/Stone_F_3_scaled.obj", "objects/Stone_Forest/");
	fairy = gps::Model3D("objects/fairy/fairy.obj", "objects/fairy/");
	lightCube = gps::Model3D("objects/cube/cube.obj", "objects/cube/");
	mushroom_amanita = gps::Model3D("objects/Mushrooms/amanita/exported/shroom.obj", "objects/Mushrooms/amanita/exported/");
	mushroom_chant = gps::Model3D("objects/Mushrooms/chanterelles/exported/chant.obj", "objects/Mushrooms/chanterelles/exported/");
	mushroom_boletus = gps::Model3D("objects/Mushrooms/boletus/exported/boletus.obj", "objects/Mushrooms/boletus/exported/");
	//sphere = gps::Model3D("objects/sphere/sphere.obj", "objects/sphere/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lampShader.loadShader("shaders/lampShader.vert","shaders/lampShader.frag");
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	depthMapShader.loadShader("shaders/shadowMap/shadowMap.vert","shaders/shadowMap/shadowMap.frag");
	reflectShader.loadShader("shaders/reflectShader/reflectShader.vert", "shaders/reflectShader/reflectShader.frag");
}

void initFBOs() {
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);
	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	//unbind till ready to use
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initUniforms()
{
	// LAMPS shader
	lampShader.useShaderProgram();

	model_lamps = glm::mat4(1.0f);
	model_lamps_Loc = glGetUniformLocation(lampShader.shaderProgram, "model_lamps");
	glUniformMatrix4fv(model_lamps_Loc, 1, GL_FALSE, glm::value_ptr(model_lamps));

	view_lamps = myCamera.getViewMatrix();
	view_lamps_Loc = glGetUniformLocation(lampShader.shaderProgram, "view_lamps");
	glUniformMatrix4fv(view_lamps_Loc, 1, GL_FALSE, glm::value_ptr(view_lamps));

	projection_lamps = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projection_lamps_Loc = glGetUniformLocation(lampShader.shaderProgram, "projection_lamps");
	glUniformMatrix4fv(projection_lamps_Loc, 1, GL_FALSE, glm::value_ptr(projection_lamps));

	lampColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lampColorLoc = glGetUniformLocation(lampShader.shaderProgram, "lampColor");
	glUniform4fv(lampColorLoc, 1, glm::value_ptr(lampColor));

	//-----------------------------------------
	// CUSTOM shader
	myCustomShader.useShaderProgram();
	
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	//dir light
	lightDir = glm::vec3(4.0f, 7.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//point light
	lightPoint = glm::vec3(6.0, 2.0, -2.0);
	lightPointLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPoint");
	glUniform3fv(lightPointLoc, 1, glm::value_ptr(lightPoint));

	//second-far away point light
	lightPoint2 = glm::vec3(-8.0, 2.0, -5.0);
	lightPointLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPoint2");
	glUniform3fv(lightPointLoc2, 1, glm::value_ptr(lightPoint2));

	///ambient component
	ambientStrength_Loc = glGetUniformLocation(myCustomShader.shaderProgram, "ambientStrength_Point");
	glUniform1f(ambientStrength_Loc, ambientStrength_Point);

	activeDirL = 1;
	activeDirLLoc = glGetUniformLocation(myCustomShader.shaderProgram, "activeDirL");
	glUniform1i(activeDirLLoc, activeDirL);

	activePointL = 1;
	activePointLLoc = glGetUniformLocation(myCustomShader.shaderProgram, "activePointL");
	glUniform1i(activePointLLoc, activePointL);

	//-- reflectShader --------------------------------------------------------------
	reflectShader.useShaderProgram();
	model_reflect_Loc = glGetUniformLocation(reflectShader.shaderProgram, "model");
	model_reflect = glm::mat4(1.0f);
	glUniformMatrix4fv(model_reflect_Loc, 1, GL_FALSE, glm::value_ptr(model_reflect));

	view_reflect = myCamera.getViewMatrix();
	view_reflect_Loc = glGetUniformLocation(reflectShader.shaderProgram, "view");
	glUniformMatrix4fv(view_reflect_Loc, 1, GL_FALSE, glm::value_ptr(view_reflect));

	projection_reflect = projection;
	projection_reflect_Loc = glGetUniformLocation(reflectShader.shaderProgram, "projection");
	glUniformMatrix4fv(projection_reflect_Loc, 1, GL_FALSE, glm::value_ptr(projection_reflect));

	normalMatrix_reflect = glm::mat3(glm::inverseTranspose(view_reflect*model_reflect));
	normalMatrix_reflect_Loc = glGetUniformLocation(reflectShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrix_reflect_Loc, 1, GL_FALSE, glm::value_ptr(normalMatrix_reflect));

}


void skyBoxMapping()
{
	/*faces.push_back("textures/skybox/forest/Fae_R.JPG");
	faces.push_back("textures/skybox/forest/Fae_L.JPG");
	faces.push_back("textures/skybox/forest/Fae_U.JPG");
	faces.push_back("textures/skybox/forest/Fae_D.JPG");
	faces.push_back("textures/skybox/forest/Fae_B.JPG");
	faces.push_back("textures/skybox/forest/Fae_F.JPG");
	*/
	faces.push_back("textures/skybox/rainforest/rainforest_rt.tga");
	faces.push_back("textures/skybox/rainforest/rainforest_lf.tga");
	faces.push_back("textures/skybox/rainforest/rainforest_up.tga");
	faces.push_back("textures/skybox/rainforest/rainforest_dn.tga");
	faces.push_back("textures/skybox/rainforest/rainforest_bk.tga");
	faces.push_back("textures/skybox/rainforest/rainforest_ft.tga");
	
	mySkyBox.Load(faces);
}
void activateLights() 
{
	//activate-deactivate lights
	glUniform1i(activeDirLLoc, activeDirL);
	glUniform1i(activePointLLoc, activePointL);
}
void moveLights()
{
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
	glUniform3fv(lightPointLoc, 1, glm::value_ptr(lightPoint));
}
void chooseRenderMode()
{
	switch (renderMode) {
		case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
		case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case 2: 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				glPointSize(1.2f);
				break; 
			}
		default: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
	}
}

glm::vec3 lampColorFadeOut(glm::vec3 color) {
	glm::vec3 lampColor;

	if (increase) 
	{
		lampColor = colorStrength * color;
		colorStrength += 0.02f;
	}
	else 
	{
		lampColor = colorStrength * color;
		colorStrength -= 0.02f;
	}
	return lampColor;
}


void renderLamps(glm::mat4 view_lamps) {
	//view
	glUniformMatrix4fv(view_lamps_Loc, 1, GL_FALSE, glm::value_ptr(view_lamps));


	//draw light cube - point light
	model_lamps = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
	model_lamps = glm::translate(model_lamps, glm::vec3(30.0, 10.0, -10.0));
	model_lamps = glm::translate(model_lamps, glm::vec3(cubePointLight_x_moved, cubePointLight_y_moved, cubePointLight_z_moved));
	glUniformMatrix4fv(model_lamps_Loc, 1, GL_FALSE, glm::value_ptr(model_lamps));
	//white color
	glm::vec3 whiteLampColor;
	if (activePointL)
		whiteLampColor = lampColorFadeOut(yellow);
	else
		glm::vec3 whiteLampColor = glm::vec3(0.0f, 0.0f, 0.0f); // if we turn off the light, the cube is black
	glm::vec4 finalWhiteColor = glm::vec4(whiteLampColor, 1.0f); // add the transparency factor
	glUniform4fv(lampColorLoc, 1, glm::value_ptr(finalWhiteColor));
	lightCube.Draw(lampShader);

	
	//draw second cube - point light
	glm::mat4 model_lamps2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
	model_lamps2 = glm::translate(model_lamps2, glm::vec3(-60.0f, 15.0f, -40.0f));
	glUniformMatrix4fv(model_lamps_Loc, 1, GL_FALSE, glm::value_ptr(model_lamps2));
	//yellow color
	glm::vec3 yellowLampColor = lampColorFadeOut(yellow);
	glm::vec4 finalYellowColor = glm::vec4(yellowLampColor, 1.0f);
	glUniform4fv(lampColorLoc, 1, glm::value_ptr(finalYellowColor));
	lightCube.Draw(lampShader);

	if (generateRedCube) {
		glm::mat4 model_redCube = glm::translate(glm::mat4(1.0f), glm::vec3(-5.4f, 3.1f, 1.5f));
		model_redCube = glm::scale(model_redCube, glm::vec3(1.1f,1.4f,1.0f));
		glUniformMatrix4fv(model_lamps_Loc, 1, GL_FALSE, glm::value_ptr(model_redCube));
		glm::vec4 redCubeColor = glm::vec4(0.86f, 0.07f, 0.23f, 0.1f); // (deep red)-purpuriu
		glUniform4fv(lampColorLoc, 1, glm::value_ptr(redCubeColor));
		lightCube.Draw(lampShader);
	}
}

glm::mat4 computeLightSpaceTrMatrix(glm::vec3 lightDirOrPoint) {
	///changing to light's point of view
	glm::mat4 lightView = glm::lookAt(lightDirOrPoint, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 1.0f, far_plane = 30.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	///final light's transf matrix
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void modifyLightIntensity() {
	if (increase) {
		if (ambientStrength_Point < 1.0f) 
			ambientStrength_Point += 0.02f;
		else 
			increase = false;
	}
	else 
	{
		if (ambientStrength_Point > 0.0f)
			ambientStrength_Point -= 0.02;
		else
			increase = true;
	}
}
bool movedAlongX = false;
bool movedAlongY = false;
bool movedAlongZ = false;
bool riseAndRotate = false;
bool stand = false;
bool rotCamera = false;
bool backCamera = false;
float fairy_angle = 0.0f;
float fairy_x_move = 0.0f, fairy_y_move = 0.0f, fairy_z_move = 0.0f;
float standingTime = 0.0f;
float rotationTime = 0.0f;
float moveBackCam = 0.0f;
float moveLeftCam = 0.0f;
float moveFrontCam = 0.0f;

void renderObjects(gps::Shader shaderUsed, glm::mat4 view) {

	//view
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//model matrix
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderUsed.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	//normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	//lightSpaceTrMatrix
	GLuint lightSpaceTrMatrix_Loc = glGetUniformLocation(shaderUsed.shaderProgram, "lightSpaceTrMatrix");
	glUniformMatrix4fv(lightSpaceTrMatrix_Loc,1,GL_FALSE,glm::value_ptr(computeLightSpaceTrMatrix(lightDir)));

	//light dirmatrix - to decouple the light from the camera movement
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	///ambient component
	glUniform1f(ambientStrength_Loc, ambientStrength_Point);
	//----------------------------------------------------------------------------------------


	//draw Ground
	ground.Draw(shaderUsed);

	//draw fairy

	glm::mat4 matrix_fairy = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	matrix_fairy = glm::translate(matrix_fairy, glm::vec3(0.0f, 2.0f, 0.0f));

	if(demo){
		float movingSpeed = deltaTime;//0.04f;

		if (!backCamera) {
			if (moveBackCam < 8.0f) {
				moveBackCam += 0.2f;
				pressedKeys[GLFW_KEY_S] = true;
			}
			else {
				pressedKeys[GLFW_KEY_S] = false;
				backCamera = true;
			}
		}

		if (backCamera & !rotCamera) {
			if (rotationTime < 60.0f) {
				pressedKeys[GLFW_KEY_Q] = true;
				rotationTime += 0.1f;
			}
			else {
				pressedKeys[GLFW_KEY_Q] = false;
				rotCamera = true;
			}
		}


		if (rotCamera & !movedAlongX) {
			if (fairy_x_move > -3.6f) {
				fairy_x_move -= movingSpeed;
				matrix_fairy = glm::translate(matrix_fairy, glm::vec3(fairy_x_move, 0.0f, 0.0f));
				
			}
			else {
				movedAlongX = true;
			}
		}

		if (movedAlongX & !movedAlongZ){
			if (fairy_z_move < 0.9f) {
				fairy_z_move += movingSpeed;
				matrix_fairy = glm::translate(matrix_fairy, glm::vec3(-3.6f, 0.0f, fairy_z_move));
			}
			else {
				movedAlongZ = true;
			}
		}

		if (movedAlongZ & !movedAlongY){
			if (fairy_y_move < 2.6f) {
				fairy_y_move += movingSpeed;
				matrix_fairy = glm::translate(matrix_fairy, glm::vec3(-3.6f, -fairy_y_move, 0.9f));
				//move camera left
				pressedKeys[GLFW_KEY_A] = true;
			}
			else {
				movedAlongY = true;
				pressedKeys[GLFW_KEY_A] = false;
			}
		}

		if (movedAlongY & !riseAndRotate) {
			
			if (fairy_y_move > -1.0f) { // fairy_y_move starts from 2.6f (y init: -2.6f)
				fairy_y_move -= (movingSpeed - 0.01f);
				matrix_fairy = glm::translate(matrix_fairy, glm::vec3(-3.6f, -fairy_y_move, 0.9f));

				if (fairy_y_move < 1.4f) { //first go up a bit, then rotate

					matrix_fairy = glm::rotate(matrix_fairy, glm::radians(fairy_angle), glm::vec3(0, 1, 0));
					fairy_angle += 30.0f;
				}
				//move camera front
				pressedKeys[GLFW_KEY_W] = true;
			}
			else {
				riseAndRotate = true;
				pressedKeys[GLFW_KEY_W] = false;
			}
		}
		
		if (riseAndRotate & !stand) {
			if (standingTime < 50.0f) {
				standingTime += 0.2f;
				matrix_fairy = glm::translate(matrix_fairy, glm::vec3(-3.6f, 1.0f, 0.9f));
				
				generateRedCube = true;
			}
			else {
				stand = true;
				generateRedCube = false;
			}
		}

		if (stand) {
			demo = false;
			fairy_x_move = 0.0f;
			fairy_y_move = 0.0f;
			fairy_z_move = 0.0f;
			movedAlongX = false;
			movedAlongY = false;
			movedAlongZ = false;
			standingTime = 0.0f;
			riseAndRotate = false;
			stand = false;
			rotCamera = false;
			backCamera = false;
			rotationTime = 0.0f;
			moveBackCam = 0.0f;
			moveLeftCam = 0.0f;
			moveFrontCam = 0.0f;
		}
		
	}

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_fairy));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_fairy));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	fairy.Draw(shaderUsed);

	//draw house
	glm::mat4 matrix_house = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
	matrix_house = glm::translate(matrix_house, glm::vec3(-3.5f, 0.0f, 0.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_house));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_house));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	fairy_house.Draw(shaderUsed);

	//draw mushroom_boletus
	glm::mat4 matrix_boletus = glm::translate(model, glm::vec3(8.3f, 0.0f, -2.2f));
	matrix_boletus = glm::scale(matrix_boletus, glm::vec3(0.6, 0.8, 0.8));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_boletus));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_boletus));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_boletus.Draw(shaderUsed);

	//draw mushroom_chanterelles
	glm::mat4 matrix_chant = glm::translate(model, glm::vec3(8.5f, 0.0f, -3.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_chant));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_chant));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_chant.Draw(shaderUsed);

	//draw mushroom_chanterelles 2
	glm::mat4 matrix_chant2 = glm::translate(model, glm::vec3(9.3f, 0.0f, -2.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_chant2));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_chant2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_chant.Draw(shaderUsed);

	//first group
	//draw mushroom_amanita
	glm::mat4 matrix_amanita = glm::translate(model, glm::vec3(9.0f, 0.0f, -3.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_amanita));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_amanita));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_amanita.Draw(shaderUsed);

	//draw mushroom_amanita
	glm::mat4 matrix_amanita_s = glm::translate(model, glm::vec3(9.0f, 0.0f, -2.5f));
	matrix_amanita_s = glm::scale(matrix_amanita_s, glm::vec3(0.6f, 0.6f, 0.6f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_amanita_s));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_amanita_s));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_amanita.Draw(shaderUsed);

	//second group
	//draw mushroom_amanita 2
	glm::mat4 matrix_amanita2 = glm::translate(model, glm::vec3(-17.0f, 0.0f, 4.0f));
	matrix_amanita2 = glm::scale(matrix_amanita2, glm::vec3(0.9f, 0.9f, 0.9f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_amanita2));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_amanita2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_amanita.Draw(shaderUsed);

	//draw mushroom_amanita 3
	glm::mat4 matrix_amanita3 = glm::translate(model, glm::vec3(-16.0f, 0.0f, 4.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_amanita3));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_amanita3));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_amanita.Draw(shaderUsed);

	//draw mushroom_amanita 4
	glm::mat4 matrix_amanita4 = glm::translate(model, glm::vec3(-16.5f, 0.0f, 5.0f));
	matrix_amanita4 = glm::scale(matrix_amanita4, glm::vec3(0.7f, 0.7f, 0.7f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_amanita4));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_amanita4));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_amanita.Draw(shaderUsed);

	//draw mushroom_amanita 5
	glm::mat4 matrix_amanita5 = glm::translate(model, glm::vec3(-15.5f, 0.0f, 4.1f));
	matrix_amanita5 = glm::scale(matrix_amanita5, glm::vec3(0.7f, 0.7f, 0.7f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_amanita5));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_amanita5));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_amanita.Draw(shaderUsed);

	//draw mushroom_amanita 6
	glm::mat4 matrix_amanita6 = glm::translate(model, glm::vec3(-15.85f, 0.0f, 5.0f));
	matrix_amanita6 = glm::scale(matrix_amanita6, glm::vec3(0.5f, 0.8f, 0.8f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_amanita6));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_amanita6));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_amanita.Draw(shaderUsed);

	//draw mushroom_amanita 7
	glm::mat4 matrix_amanita7 = glm::translate(model, glm::vec3(-16.7f, 0.0f, 5.5f));
	matrix_amanita3 = glm::scale(matrix_amanita3, glm::vec3(0.6f, 0.6f, 0.6f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_amanita7));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_amanita7));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_amanita.Draw(shaderUsed);

	//draw mushroom_amanita 8
	glm::mat4 matrix_amanita8 = glm::translate(model, glm::vec3(-17.5f, 0.0f, 4.4f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_amanita8));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_amanita8));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mushroom_amanita.Draw(shaderUsed);



	//draw Stone
	glm::mat4 matrix_stone = glm::translate(model, glm::vec3(7.0f, 0.5f, -6.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_stone));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_stone));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	stone.Draw(shaderUsed);
	//draw Stone
	glm::mat4 matrix_stone2 = glm::translate(model, glm::vec3(9.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_stone2));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_stone2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	stone.Draw(shaderUsed);

	//draw Grass
	glm::mat4 matrix_grass = glm::translate(model, glm::vec3(8.0f, 0.0f, -10.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_grass));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_grass));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	grass.Draw(shaderUsed);
	//draw Grass
	glm::mat4 matrix_grass2 = glm::translate(model, glm::vec3(-12.0f, 0.0f, -10.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_grass2));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_grass2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	grass.Draw(shaderUsed);

	//draw trees
	glm::mat4 matrix_tree1 = glm::translate(model, glm::vec3(5.0f, 0.0f, -11.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_tree1));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_tree1));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tree.Draw(shaderUsed);

	glm::mat4 matrix_tree2 = glm::translate(model, glm::vec3(10.0f, 0.0f, -6.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_tree2));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_tree2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tree.Draw(shaderUsed);

	glm::mat4 matrix_tree3 = glm::translate(model, glm::vec3(-15.0f, 0.0f, -6.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix_tree3));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*matrix_tree3));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	tree.Draw(shaderUsed);

}

void renderSkyBox(glm::mat4 view){

	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	mySkyBox.Draw(skyboxShader, view, projection);
}
void renderReflecting(glm::mat4 view_reflect) {
	//view matrix

	glUniformMatrix4fv(view_reflect_Loc, 1, GL_FALSE, glm::value_ptr(view_reflect));

	//model matrix
	model_reflect = glm::mat4(1.0f);
	model_reflect = glm::scale(model_reflect, glm::vec3(0.8f,0.8f,0.8f));
	model_reflect = glm::translate(model_reflect, glm::vec3(-7.0f, 0.0f, 2.0f));
	glUniformMatrix4fv(model_reflect_Loc, 1, GL_FALSE, glm::value_ptr(model_reflect));

	//normalMatrix
	normalMatrix_reflect = glm::mat3(glm::inverseTranspose(view_reflect*model_reflect));
	glUniformMatrix3fv(normalMatrix_reflect_Loc, 1, GL_FALSE, glm::value_ptr(normalMatrix_reflect));

	lightCube.Draw(reflectShader);
}

void calcCameraSpeed()
{
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	cameraSpeed = 5.0f * deltaTime;
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	calcCameraSpeed();
	processMovement();

	//Shadow Mapping
	//pass1
	depthMapShader.useShaderProgram();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	///render Scene to depth buffer
	renderObjects(depthMapShader, view);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	//pass2
	chooseRenderMode();

	// CUSTOM shader----------------------------------------------------------------
	myCustomShader.useShaderProgram();
	
	activateLights();
	moveLights();
	modifyLightIntensity();

	glViewport(0, 0, retina_width, retina_height);
	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	renderObjects(myCustomShader, view);



	// LIGHTS shader--------------------------------------------------------------------------
	lampShader.useShaderProgram();
	renderLamps(view);
	// Skybox---------------------------------------------------------------------------------
	skyboxShader.useShaderProgram();
	renderSkyBox(view);
	// reflectShader--------------------------------------------------------------------------
	reflectShader.useShaderProgram();
	renderReflecting(view);
}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();	
	skyBoxMapping();
	
	
	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
