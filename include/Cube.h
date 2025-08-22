#pragma once
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "PhysicsComponent.h"

class Cube {
public:
	
	Cube();
	Cube(glm::vec3 position);
	~Cube();

	const glm::mat4& getModelMatrix() const { return modelMatrix; }

	void draw();

	PhysicsComponent physics;

private:
	static constexpr float vertices[] = {
	-0.5f, -0.5f, 0.0f, 0.9f, 0.45f, 0.1f, // bottom left - 0
	0.5f, -0.5f, 0.0f, 0.9f, 0.45f, 0.1f, // bottom right - 1
	- 0.5f, 0.5f, 0.0f, 0.9f, 0.45f, 0.1f, // top left - 2
	0.5f, 0.5f, 0.0f,  0.9f, 0.45f, 0.1f, // top right - 3

	- 0.5f, -0.5f, -1.0, 0.9f, 0.45f, 0.1f, // back bottom left - 4
	0.5f, -0.5f, -1.0,  0.9f, 0.45f, 0.1f, // back bottom right - 5
	- 0.5f, 0.5f, -1.0,0.9f, 0.45f, 0.1f, // back top left - 6
	0.5f, 0.5f, -1.0, 0.9f, 0.45f, 0.1f, // back top right - 7
	};

	static constexpr unsigned int indices[] = {
		0, 1, 2,   2, 3, 1, // front face
		4, 5, 6,   6, 7, 5, // back face
		4, 0, 2,   2, 6, 4, // left face
		1, 5, 3,   3, 7, 5, // right face
		2, 3, 6,   6, 3, 7, // top face
		4, 5, 0,   0, 5, 1 // bottom face
	};

	unsigned int VAO{ 0 }, VBO{ 0 }, EBO{0};

	mutable glm::mat4 modelMatrix{ 1.0f };

	void updateModelMatrix() const;
};