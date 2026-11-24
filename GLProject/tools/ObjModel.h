#pragma once
#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <string>
#include "../libs/glad_loader_docs.h"
#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>

// A 3D model loaded from a Wavefront .obj text file (positions and texture coordinates).
// Why not the lecture's Model class? That class needs the assimp library, and the assimp
// library bundled with the project is built for Visual Studio, so MinGW cannot link it.
// Vertex layout: 0 = position, 1 = texture coordinate (the scene has no lighting, so no normals are needed).
class ObjModel
{
public:
	ObjModel(const std::string &path, GLuint texture);

	void transformation(glm::mat4 m) { model = m; }
	void draw(Shader &shader);

private:
	GLuint VAO, VBO;
	int vertexCount;
	GLuint texture;
	glm::mat4 model;
};

#endif
