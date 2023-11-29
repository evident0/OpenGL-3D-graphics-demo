#include "Texture.h";


Texture::Texture(const char* img, GLenum textureType, GLenum slot, GLenum format, GLenum pixelType) {
	type = textureType;
	int width, height, nrChannels;
	unsigned char* data = stbi_load(img, &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(type, ID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //because the textures are not power of two the bytes are not divisible by 4 ex(225*3(GL_RGB) !/ 4)
	glTexImage2D(textureType, 0, format, width, height, 0, format, pixelType, data);
	glGenerateMipmap(textureType);

	glBindTexture(textureType, 0);
	stbi_image_free(data);
}


void Texture::bind() {
	glBindTexture(type, ID);
}


void Texture::unbind() {
	glBindTexture(type, 0);
}

void Texture::del() {
	glDeleteTextures(1, &ID);
}