#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Cube.h"

float planeVertices[] = {
	-1.0f, -1.0f, 0.0f,  1.0f, 0.5f, 0.5f,// bottom left
	1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, // bottom right
	1.0f, 1.0f, 0.0f,  1.0f, 0.5f, 0.5f, // top right
	-1.0f, 1.0f, 0.0f,  1.0f, 0.5f, 0.5f,// top left
};

unsigned int planeIndices[] = {
	0, 1, 2,
	2, 3, 0,
};

// Camera
float fov = 45.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// delta calculations
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// mouse constants
float lastX = 400, lastY = 300;
float pitch = 0.0f, yaw = -90.0f;
bool firstMouse = true;

// physics constants
float velocity = 0.0f;
float gravity = -9.8f;
float limit = 5.0f; // half-size of world
float drag = 0.98f;

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "SolarSystem", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to initialize GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// plane
	unsigned int planeVAO, planeVBO, planeEBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glGenBuffers(1, &planeEBO);
	
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

	// position attribute for plane
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Shader
	Shader ourShader("shaders/vertex.s", "shaders/fragment.s");

	glEnable(GL_DEPTH_TEST);

	Cube cubeA(glm::vec3(0.5f, 0.5f, 0.5f));
	Cube cubeB(glm::vec3(0.5f, 0.5f, 10.0f));

	while (!glfwWindowShouldClose(window)) {
		// delta calculations
		float currentTime = (float)glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();

		// projection matrix
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); // think of lookAt as camera; 
													//then it should do opposite of translating the world
		ourShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);

		if ()

		cubeA.physics.update(deltaTime, gravity, drag, limit);
		ourShader.setMat4("model", cubeA.getModelMatrix());
		cubeA.draw();

		cubeB.physics.update(deltaTime, gravity, drag, limit);
		ourShader.setMat4("model", cubeB.getModelMatrix());
		cubeB.draw();

		// plane
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 1.0f));
		ourShader.setMat4("model", model);

		glBindVertexArray(planeVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	const float speed = deltaTime * 2.5f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		// goes forward
		cameraPos += (speed * cameraFront);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		// goes backwards
		cameraPos -= (speed * cameraFront);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		// goes right
		glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp)); // make sure to normalize to avoid having magnitude play a role
		cameraPos += (speed * right);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		// goes left
		glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp)); // make sure to normalize to avoid having magnitude play a role
		cameraPos -= (speed * right);
	}

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	const float sens = 0.1f;
	xoffset *= sens;
	yoffset *= sens;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	fov -= (float)yoffset;
	if (fov < 1.0f) fov = 1.0f;
	if (fov > 85.0f) fov = 85.0;
}