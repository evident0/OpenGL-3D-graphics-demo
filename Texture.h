#pragma once
// Include GLEW
#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>

class Texture {

public: 
	GLuint ID;
	GLenum type;


	Texture(const char* img, GLenum textureType, GLenum slot, GLenum format, GLenum pixelType);

	void bind();
	void unbind();
	void del();

};