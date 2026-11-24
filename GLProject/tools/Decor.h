#pragma once
#ifndef DECOR_H
#define DECOR_H

#include <vector>
#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>
#include "Polygon.h"
#include "ObjModel.h"

// Three decoration pieces for the room:
//   1. a balance scale   2. a chess board   -> both are FLAT (2D) pictures hung on the back wall,
//                                              drawn only with the lecture class Polygon
//   3. an antique globe                     -> the ONE piece that is a 3D model loaded from a file
class Decor
{
public:
	Decor(GLuint plainTexture, GLuint globeTexture);
	void draw(Shader &shader);			  // the flat pieces
	void drawModels(Shader &modelShader); // the 3D model (it has its own shader)

private:
	GLuint plain;				// the white texture (the shapes are colored with objectColor)
	std::vector<Polygon> polys; // every flat shape, already in world coordinates
	ObjModel globe;				// the 3D model piece

	// building blocks: every shape lies in a plane parallel to the back wall.
	// "layer" pushes a shape a tiny bit away from the wall, so shapes on top of each other do not flicker.
	float wallZ(int layer);
	void addRect(float cx, float cy, float w, float h, int layer, glm::vec3 color);
	void addBar(glm::vec2 from, glm::vec2 to, float thickness, int layer, glm::vec3 color); // a thin tilted rectangle
	void addDisc(float cx, float cy, float radius, int layer, glm::vec3 color);
	void addHalfDisc(float cx, float cy, float radius, float depth, int layer, glm::vec3 color); // a bowl seen from the front
	void addQuad(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d, int layer, glm::vec3 color);

	// the two flat pieces (cx, cy = the middle of the picture on the wall)
	void addScalePicture(float cx, float cy);
	void addChessPicture(float cx, float cy);
	void addPiece(float x, float y, glm::vec3 color, bool isKing, int layer);
};

#endif
