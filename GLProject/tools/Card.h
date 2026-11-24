#pragma once
#ifndef CARD_CLASS_H
#define CARD_CLASS_H

#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>
#include "Polygon.h"

// One card = one textured quad.
// The card is rotated around its NEAR edge (the "pivot"), so the very same
// transformation makes it lie flat (standAmount = 0) or stand up leaning
// slightly backwards (standAmount = 1). Any value in between is a frame of the
// animation.
class Card
{
public:
	Card(GLuint texture, glm::vec3 pivot);

	void setPivot(glm::vec3 p) { pivot = p; }
	void draw(Shader &shader, float standAmount);

	// ---- key G: move this card to a place in the play area ----
	void moveTo(glm::vec3 target); // starts the animation
	void update(float deltaTime);  // call every frame: advances the animation
	bool isInStock() { return !moving && !played; }

private:
	PolygonWithTexture face;
	glm::vec3 pivot; // middle of the card's near edge, on the table

	glm::vec3 startPivot, endPivot;
	float moveTime; // 0 -> 1 while the card is travelling
	bool moving;
	bool played; // true once it arrived in the play area
};

#endif
