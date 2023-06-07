#include <gl.h>
#include <GLFW\glfw3.h>

#include <iostream>
#include <windows.h>

#include "stb_image.h"

#include "Shader.h"
#include "CamFPS.h"
#include "Materials.h"
#include "Lights.h"
#include "Timer.h"
#include "Model.h"


#define SCR_WIDTH 1000
#define SCR_HEIGHT 800

void framebuffer_size_callback(GLFWwindow*, int, int);
void cursor_pos_callback(GLFWwindow*, double, double);
void key_callback(GLFWwindow*, int, int, int, int);
void processInput(GLFWwindow*);

unsigned int loadTex(const char*);

using std::cout;

/////////////////////////////////////////
// Camera

CamFPS cam(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 2.0f, 0.0f));

//delta time
float lastFrame = 0.0f;
float deltaTime = 0.0f;

//mouse view
float xLast = 0.0f, yLast = 0.0f;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstCursor = true;

/////////////////////////////////////////
// Lights

glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

//flashlight
bool flActive = true;
bool flFlicker = false;
float flTimer = 0.f;

/////////////////////////////////////////
// MAIN

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(
		SCR_WIDTH, SCR_HEIGHT,
		"...",
		NULL, NULL
	);
	if (!window) {
		cout << "Error: Window creation FAILED\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress)) {
		cout << "Error: GLAD loadGL FAILED\n";
		glfwTerminate();
		return -1;
	};

	auto sysWidth = GetSystemMetrics(SM_CXSCREEN);
	auto sysHeight = GetSystemMetrics(SM_CYSCREEN);
	glfwSetWindowMonitor(window, NULL, (sysWidth - SCR_WIDTH) / 2, (sysHeight - SCR_HEIGHT) / 2, SCR_WIDTH, SCR_HEIGHT, GLFW_DONT_CARE);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	////////////////////////////////////////
	// Shaders

	//object shader
	//Shader shader0("shaders\\vLightObj.txt", "shaders\\fLightObj.txt");

	//lightning shader
	Shader shaderLight0("shaders\\vLightSrc.txt", "shaders\\fLightSrc.txt");

	//model shader
	Shader shaderModel0("shaders\\vModel.txt","shaders\\fModel.txt");

	////////////////////////////////////////
	// Textures

	//unsigned int tex0 = loadTex("..\\img\\container1.png");
	//unsigned int tex0Spec = loadTex("..\\img\\container1_spec.png");
	//unsigned int tex0Emiss = loadTex("..\\img\\container1_emiss.png");
	//unsigned int flCookie = loadTex("..\\img\\cookie.png");

	////////////////////////////////////////
	// Data configuring

	//plate
	/*float vertices0[] = {
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f
	};*/

	//cube
	float vertices1[] = {
	//vertex			  //normal			   //tex
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	//rotated cubes
	/*const int n = 10;
	glm::vec3 cubes[n] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(-1.5f, -1.5f, -1.0f),
		glm::vec3(3.0f, 0.5f, -1.0f),
		glm::vec3(-2.0f, 1.5f, -3.0f),
		glm::vec3(1.5f, -1.5f, 0.2f),
		glm::vec3(0.5f, 2.0f, 1.0f),
		glm::vec3(-3.5f, -0.5f, -2.0f),
		glm::vec3(2.0f, 2.0f, -3.0f),
		glm::vec3(-1.0f, 3.0f, -6.0f),
		glm::vec3(3.5f, -1.5f, 0.4f)
	};*/

	//random up-down movement
	/*srand(time(NULL));
	float random[n];
	for (int i = 0; i < n; i++) {
		random[i] = (rand() % 3 + 1);
	}*/

	//point lights
	const int m = 1;
	glm::vec3 plPoss[m] = {
		glm::vec3(1.f, 1.4f, 0.4f),
		//glm::vec3(-1.5f, -0.5f, 0.5f),
		//glm::vec3(0.3f, 1.2f, -2.f),
	};
	//if only one

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

	////////////////////////////////////////
	// Light source

	unsigned int VAOl;
	glGenVertexArrays(1, &VAOl);
	glBindVertexArray(VAOl);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

	////////////////////////////////////////
	// Pre-Render

	glEnable(GL_DEPTH_TEST);
	
	glm::mat4 projection = glm::perspective(
		glm::radians(45.0f),
		static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
		0.1f,
		100.0f
	);

	Model asset("..\\assets\\backpack\\backpack.obj", false);
	

	unsigned int tex1 = loadTex("..\\img\\frame.png");

	//shaderModel0
	//-----------------------------------

	shaderModel0.use();

	shaderModel0.setMat4("vuProjection", projection);

	
	//shader0
	//-----------------------------------
	//shader0.use();
	//shader0.setMat4("vuPerspective", perspective);

	////material
	//shader0.setInt("material.diffuse", 0);
	//shader0.setInt("material.specular", 1);
	//shader0.setInt("material.emission", 2);
	//shader0.setVec3("material.specular", 0.8f, 0.8f, 0.8f);
	//shader0.setFloat("material.shininess", SilverShininess);

	////directional light
	////shader0.setVec3("dirLight.ambient", LampAmbient);
	////shader0.setVec3("dirLight.diffuse", LampDiffuse);
	////shader0.setVec3("dirLight.specular", LampSpecular);
	////shader0.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

	//point lights init
	Shader& sh = shaderModel0; //specify required shader unit
	sh.use();
	if(m > 1)
	{
		for (int i = 0; i < m; i++) {
			std::string name = "pointLights[" + std::to_string(i) + "]";
			sh.setVec3(name + ".position", plPoss[i]);
			sh.setVec3(name + ".ambient", LampAmbient);
			if (i == 0) sh.setVec3(name + ".diffuse", LampDiffuse);
			if (i == 1) sh.setVec3(name + ".diffuse", LampDiffuse);
			if (i == 2) sh.setVec3(name + ".diffuse", LampDiffuse);
			sh.setVec3(name + ".specular", LampSpecular);
			sh.setFloat(name + ".constant", 1.0f);
			sh.setFloat(name + ".linear", 0.09f);
			sh.setFloat(name + ".quadr", 0.032f);
		}
	} else
		if (m == 1)
		{
			std::string name = "pointLight";
			sh.setVec3(name + ".position", plPoss[0]);
			sh.setVec3(name + ".ambient", LampAmbient);
			sh.setVec3(name + ".diffuse", LampDiffuse);
			sh.setVec3(name + ".specular", LampSpecular);
			sh.setFloat(name + ".constant", 1.0f);
			sh.setFloat(name + ".linear", 0.09f);
			sh.setFloat(name + ".quadr", 0.032f);
		}

	////flashlight
	//shader0.setVec3("flashLight.ambient", LampAmbient);
	//shader0.setVec3("flashLight.diffuse", LampDiffuse);
	//shader0.setVec3("flashLight.specular", LampSpecular);
	//shader0.setFloat("flashLight.constant", 1.0f);
	//shader0.setFloat("flashLight.linear", 0.09f);
	//shader0.setFloat("flashLight.quadratic", 0.032f);
	//shader0.setInt("flashLight.cookie", 3);
	//shader0.setFloat("flashLight.inAngle", glm::cos(glm::radians(12.f)));
	//shader0.setFloat("flashLight.outAngle", glm::cos(glm::radians(16.f)));

	//shader0.setVec2("viewPort", SCR_WIDTH, SCR_HEIGHT);

	//Timer t(true);
	//Timer flT(0);

	//shaderLight0
	//------------------------------------
	shaderLight0.use();
	shaderLight0.setMat4("vuPerspective", projection);


	////////////////////////////////////////
	// RENDER

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		////////////////////////////////////
		glClearColor(SKY, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		////////////////////////////////////
		float time = glfwGetTime();
		float currentFrame = time;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		cam.setDeltaTime(deltaTime);
		////////////////////////////////////
		
		////////////////////////////////////
		// Camera

		glm::mat4 camView = cam.getView();
		glm::vec3 camPos = cam.getPos();
		glm::vec3 camDir = cam.getDir();

		////////////////////////////////////
		// Light settings

		//rotated pointlight
		float e = rand() % 10 / 10;
		lightPos.x = cos((time + 4 + e) * 2) * 3.f;
		lightPos.y = sin(time + 4 + e) / 2 * 2;
		//lightPos.z = sin((time + 4 + e) * 2) * 3.f;
		lightPos.z = 2.f;
		
		////////////////////////////////////
		// ShaderModel0

		shaderModel0.use();

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
		shaderModel0.setMat4("vuModel", model);
		shaderModel0.setMat4("vuView", camView);

		shaderModel0.setVec3("pointLight.position", lightPos);
		shaderModel0.setVec3("fuViewDir", camDir);
		//shaderModel0.setVec3("fuViewPos", camPos);

		asset.draw(shaderModel0);

		////////////////////////////////////
		// Shader0 (Scene)

		//shader0.use();

		//light updading
		//----------------------------------

		//flashlight
		/*shader0.setVec3("flashLight.position", camPos);
		shader0.setVec3("flashLight.direction", camDir);

		shader0.setBool("flashLight.isActive", flActive);

		float r1 = rand() % 100 + 50, r2 = rand() % 200 + 100;
		if (t.Elapsed() > (rand() % 3000 + 8000)) {
			if (!flFlicker) {
				flFlicker = true;
				flT.start();
			}
		}
		if (!flFlicker)
			flT.Reset();

		if (flFlicker) {
			if (flT.Elapsed() == 600 + r1) {
				shader0.setBool("flashLight.isActive", false);
				flActive = false;
			}
			if (flT.Elapsed() == 900) {
				shader0.setBool("flashLight.isActive", true);
				flActive = true;
			}
			if (flT.Elapsed() == 1400) {
				shader0.setBool("flashLight.isActive", false);
				flActive = false;
			}
			if (flT.Elapsed() == 1470 + r1) {
				shader0.setBool("flashLight.isActive", true);
				flActive = true;
			}
			if (flT.Elapsed() == 1500 + r2) {
				shader0.setBool("flashLight.isActive", false);
				flActive = false;
			}
			if (flT.Elapsed() == 1650 + r1) {
				shader0.setBool("flashLight.isActive", true);
				flActive = true;
			}
			if (flT.Elapsed() > 1900) {
				shader0.setBool("flashLight.isActive", false);
				if (rand() % 2 == 0)
					flActive = false;
				else
					flActive = true;
				flFlicker = false;
				flT.end();
				t.end();
			}
		}*/

		//----------------------------------
	
		//shader0.setVec3("fuViewPos", cam.getPos());

		//shader0.setMat4("vuView", camView);

		//glm::mat4 model = glm::mat4(1.0f);
		////model = glm::rotate(model, glm::radians((float)glfwGetTime()) * 50, glm::vec3(1.0f, 0.5f, 0.0f));
		////shader0.setMat4("vuModel", model);
		//
		//shader0.setFloat("fuTime", time * 1.5f);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, tex0);

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, tex0Spec);

		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, tex0Emiss);

		//glActiveTexture(GL_TEXTURE3);
		//glBindTexture(GL_TEXTURE_2D, flCookie);

		//glBindVertexArray(VAO);
		////glDrawArrays(GL_TRIANGLES, 0, 36);

		//rotated cubes
		/*for (int i = 0; i < n; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubes[i]);
			model = glm::translate(model, glm::vec3(0.0f, static_cast<float>(sin(glfwGetTime() * random[i] * 0.25f) * 0.25f), 0.0f));
			model = glm::rotate(model, glm::radians((float)time * (i + 1)) * 8, glm::vec3(1.0f, 0.5f, 0.2f));
			shader0.setMat4("vuModel", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}*/

		////////////////////////////////////
		// ShaderLight0 (Light source)

		shaderLight0.use();
		shaderLight0.setMat4("vuView", camView);
		
		//1 moving light source
		model = glm::mat4(1.0);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.8f));
		shaderLight0.setMat4("vuModel", model);

		//few light sources
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);

		//shaderLight0.setInt("frame", 3);
		glBindVertexArray(VAOl);

		/*for (int i = 0; i < m; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, points[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			shaderLight0.setMat4("vuModel", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}*/

		shaderLight0.setMat4("vuModel", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		////////////////////////////////////
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	////////////////////////////////////////
	// Memory dealloc

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VAOl);

	//shader0.suicide();
	shaderLight0.suicide();
	shaderModel0.suicide();

	glfwTerminate();
	system("pause");
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int x, int y) {
	glViewport(0, 0, x, y);
}

void cursor_pos_callback(GLFWwindow* window, double xPos, double yPos) {

	if (firstCursor) {
		xLast = xPos;
		yLast = yPos;
		firstCursor = false;
	}
	float xOffset = xPos - xLast;
	float yOffset = yLast - yPos;
	xLast = xPos;
	yLast = yPos;
	float sensitivity = 0.02f;
	yaw += xOffset * sensitivity;
	pitch += yOffset * sensitivity;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 dir;
	dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cam.setDir(glm::normalize(dir));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int modes) {
	
	//flashlight
	if ((key == GLFW_KEY_L) && (action == GLFW_PRESS)) {
		if (flActive)
			flActive = false;
		else
			flActive = true;
	}
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	//camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam.moveFront();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cam.moveBack();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam.moveLeft();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cam.moveRight();
	}

}

unsigned int loadTex(const char* path) {
	unsigned int id;
	glGenTextures(1, &id);
	int imgW, imgH, imgCh;
	unsigned char* imgData = stbi_load(path, &imgW, &imgH, &imgCh, 0);
	stbi_set_flip_vertically_on_load(true);
	if (imgData) {
		GLenum format = 0;
		if (imgCh == 1) format = GL_RED;
		if (imgCh == 3) format = GL_RGB;
		if (imgCh == 4) format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, imgW, imgH, 0, format, GL_UNSIGNED_BYTE, imgData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
		cout << "Warning: Texture not loaded\n";
	stbi_image_free(imgData);

	return id;
};