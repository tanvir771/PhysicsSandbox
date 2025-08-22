#pragma once

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	Shader(const char* vertexPath, const char* fragPath);
	void use() const;

	void setMat4(const std::string& name, const glm::mat4& matrix);
	void setVec3(const std::string& name, const glm::vec3& vec);
private:
	unsigned int programID;
};