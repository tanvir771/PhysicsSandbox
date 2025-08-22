#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Shader.h"
#include "Cube.h"

float planeVertices[] = {
	-1.0f, -1.0f, 0.0f, 0.7f, 0.7f, 0.75f,// bottom left
	1.0f, -1.0f, 0.0f, 0.7f, 0.7f, 0.75f, // bottom right
	1.0f, 1.0f, 0.0f,  0.7f, 0.7f, 0.75f, // top right
	-1.0f, 1.0f, 0.0f,  0.7f, 0.7f, 0.75f,// top left
};

unsigned int planeIndices[] = {
	0, 1, 2,
	2, 3, 0,
};

// background
static float bgColor[3] = { 0.1f, 0.12f, 0.15f };

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
bool leftWasDown = false;
bool showCursor = true;

// physics constants
float velocity = 0.0f;
float gravity = -9.8f;
float limit = 5.0f; // half-size of world
float drag = 0.98f;

// objects
std::vector<Cube*> objs; // keeps track of all the objects present

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void imgui_apply_styles();
void imgui_rendering(GLFWwindow* window);

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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// imGUI setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	imgui_apply_styles();

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
	objs.push_back(&cubeA);
	objs.push_back(&cubeB);

	while (!glfwWindowShouldClose(window)) {
		// delta calculations
		float currentTime = (float)glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		processInput(window);

		glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
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

		imgui_rendering(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

auto punchObj = [&](auto& objects) {
	if (objects.empty()) return;
	// find the nearest object
	size_t best = 0;
	float bestD2 = std::numeric_limits<float>::max();
	for (size_t i = 0; i < objects.size(); i++) {
		const glm::vec3& p = objects[i]->physics.position;
		float d2 = glm::length(p - cameraPos);
		if (d2 < bestD2) { bestD2 = d2; best = i; }
	}

	glm::vec3 toObj = objects[best]->physics.position - cameraPos;
	float len = glm::length(toObj); // uses sqrt, not good for performance
	if (len > 1e-5f) {
		glm::vec3 dir = toObj / len; // manually normalize as 0 isnt gaurded
		objects[best]->physics.velocity += dir * 10.0f;
		objects[best]->physics.velocity.y += 2.0f;
	}
};

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	
	// Foucs camera with tab
	static bool tabPressed = false;
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
		showCursor = !showCursor;
		if (showCursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		tabPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
		tabPressed = false;
	}

	ImGuiIO& io = ImGui::GetIO();

	// Don't process camera input if ImGui wants keyboard input
	if (io.WantCaptureKeyboard) {
		return;
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
	// Don't capture mouse for camera if imgui uses mouse
	ImGuiIO& io = ImGui::GetIO();

	if (showCursor) {
		return;
	}
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	bool leftDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	if (leftDown == GLFW_PRESS && !leftWasDown) {
		punchObj(objs); // global objs keeping track of all the objects present in scene
	}
	leftWasDown = (leftDown == GLFW_PRESS);

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

void imgui_apply_styles() {
	// imGUI style
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// Corners
	style.WindowRounding = 8.0f;
	style.ChildRounding = 8.0f;
	style.FrameRounding = 6.0f;
	style.PopupRounding = 6.0f;
	style.ScrollbarRounding = 6.0f;
	style.GrabRounding = 6.0f;
	style.TabRounding = 6.0f;

	// Colors
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.50f, 0.56f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.50f, 0.56f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.56f, 1.00f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.22f, 0.36f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void imgui_rendering(GLFWwindow* window) {
	// ImGui rendering
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), dockFlags);

	// Main Control Panel
	ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	// Scene Controls Section
	ImGui::SeparatorText("Scene Controls");

	// Color picker for background or objects
	ImGui::ColorEdit3("Background Color", bgColor);

	// Cursor toggle
	ImGui::Checkbox("Show Cursor", &showCursor);
	if (ImGui::IsItemClicked()) {
		// Update cursor mode when checkbox is clicked
		if (showCursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	ImGui::Spacing();

	// Physics Controls Section
	ImGui::SeparatorText("Physics Settings");

	ImGui::SliderFloat("Gravity", &gravity, -20.0f, 0.0f, "%.1f");
	ImGui::SliderFloat("Drag", &drag, 0.8f, 1.0f, "%.2f");
	ImGui::SliderFloat("World Limit", &limit, 1.0f, 20.0f, "%.1f");

	ImGui::Spacing();

	// Camera Info Section
	ImGui::SeparatorText("Camera Info");

	ImGui::Text("Position: (%.1f, %.1f, %.1f)", cameraPos.x, cameraPos.y, cameraPos.z);
	ImGui::Text("FOV: %.1f°", fov);
	ImGui::Text("FPS: %.0f", 1.0f / deltaTime);

	if (ImGui::Button("Reset Camera", ImVec2(-1, 0))) { // -1 width = full width
		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		pitch = 0.0f;
		yaw = -90.0f;
		fov = 45.0f;
	}

	ImGui::Spacing();

	// Object Controls Section
	ImGui::SeparatorText("Objects");

	ImGui::Text("Active Objects: %zu", objs.size());

	// Object color picker
	static float objColor[3] = { 1.0f, 0.5f, 0.5f };
	ImGui::ColorEdit3("Object Color", objColor);

	// You can use objColor to update your cube colors
	// For example, update your planeVertices or cube colors
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Handle external windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

}