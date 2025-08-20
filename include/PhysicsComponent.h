#pragma once
#include <glm/glm.hpp>

class PhysicsComponent {
public:
	PhysicsComponent(glm::vec3 startPos = glm::vec3(0.0f, 0.0f, 0.0f)) : position(startPos) {}

	void update(float deltaTime, float gravity, float drag, float limit);

	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 velocity{ 0.0f, 0.0f, 0.0f };

};