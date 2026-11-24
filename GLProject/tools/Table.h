#pragma once
#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>
#include "Polygon.h"

// The wooden table: top, four sides, and the three zones drawn on its top
// (OPPONENT, PLAYED, STOCK), each with a gold frame and five card slots.
class Table
{
public:
	Table(GLuint woodTexture, GLuint plainTexture);
	void draw(Shader &shader);

private:
	std::vector<PolygonWithTexture> parts;

	// adds a quad (4 corners in order) to `parts`
	void addQuad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
				 glm::vec2 uvMax, GLuint texture, glm::vec3 color);
	// a quad lying on the table: centered at (cx, y, cz), size (w x d)
	void addFlat(float cx, float cz, float w, float d, float y, GLuint texture, glm::vec3 color);
	void addZone(int zone, glm::vec3 feltColor, GLuint plainTexture);
	// a table leg (4 sides) from the floor up to the table, centered at (cx, cz)
	void addLeg(float cx, float cz, GLuint texture);
};

#endif
