#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "../libs/glad_loader_docs.h"
#include <iostream>
#include <string>
#include "stb_image.h"

// Loads an image file into an OpenGL texture (same steps as the lecture code:
// options, stbi_load, glTexImage2D, glGenerateMipmap).
// The image must be an RGB .jpg (no alpha channel).
inline GLuint loadTexture(const std::string &path)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture: " << path << std::endl;
	stbi_image_free(data);
	return texture;
}

#endif
