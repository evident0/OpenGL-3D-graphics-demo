#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Texture.h"


class Object {
public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	GLuint vao;
	const char* path;

	Object(const char* path);
	void draw(Texture& texture, glm::vec3 pos, GLuint programID);
	bool loadOBJ();
	void del();
};