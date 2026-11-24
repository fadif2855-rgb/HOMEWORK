#pragma once
#ifndef ROOM_H
#define ROOM_H

#include <vector>
#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>
#include "Polygon.h"

// The room around the table: wooden floor, wallpapered walls, ceiling, skirting boards,
// and a poster on the back wall whose top-right corner is torn off.
class Room
{
public:
	Room(GLuint wallpaper, GLuint floorTexture, GLuint woodTexture, GLuint plainTexture, GLuint posterTexture);
	void draw(Shader &shader);

private:
	std::vector<PolygonWithTexture> parts;

	void addQuad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
				 glm::vec2 uvMax, GLuint texture, glm::vec3 color);
	void addPoster(GLuint posterTexture);
};

#endif
