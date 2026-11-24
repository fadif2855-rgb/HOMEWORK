#include "Decor.h"
#include "Layout.h"
#include "../libs/glad_loader_docs.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

// ------------------------------------------------------------------------------------------
// building blocks (all of them are flat shapes made of Polygon)
// ------------------------------------------------------------------------------------------

// the z of a flat shape: just in front of the back wall
float Decor::wallZ(int layer)
{
	return -Layout::ROOM_HALF_Z + 0.02f + 0.01f * layer;
}

void Decor::addQuad(vec2 a, vec2 b, vec2 c, vec2 d, int layer, vec3 color)
{
	float z = wallZ(layer);
	vector<vec3> v = {vec3(a, z), vec3(b, z), vec3(c, z), vec3(d, z)};
	polys.push_back(Polygon(v, color));
}

void Decor::addRect(float cx, float cy, float w, float h, int layer, vec3 color)
{
	addQuad(vec2(cx - w / 2, cy - h / 2), vec2(cx + w / 2, cy - h / 2),
			vec2(cx + w / 2, cy + h / 2), vec2(cx - w / 2, cy + h / 2), layer, color);
}

// a thin rectangle going from one point to another
void Decor::addBar(vec2 from, vec2 to, float thickness, int layer, vec3 color)
{
	vec2 dir = normalize(to - from);
	vec2 side = vec2(-dir.y, dir.x) * (thickness / 2); // perpendicular to the direction
	addQuad(from + side, to + side, to - side, from - side, layer, color);
}

// a triangle fan: the first vertex is the middle, the others go around the circle
void Decor::addDisc(float cx, float cy, float radius, int layer, vec3 color)
{
	const int SIDES = 24;
	float z = wallZ(layer);
	vector<vec3> v;
	v.push_back(vec3(cx, cy, z));
	for (int i = 0; i <= SIDES; i++)
	{
		float a = radians(360.0f * i / SIDES);
		v.push_back(vec3(cx + radius * cos(a), cy + radius * sin(a), z));
	}
	polys.push_back(Polygon(v, color));
}

// the lower half of an ellipse (the pan of the scale seen from the front)
void Decor::addHalfDisc(float cx, float cy, float radius, float depth, int layer, vec3 color)
{
	const int SIDES = 16;
	float z = wallZ(layer);
	vector<vec3> v;
	v.push_back(vec3(cx, cy, z));
	for (int i = 0; i <= SIDES; i++)
	{
		float a = radians(180.0f * i / SIDES); // 0 .. 180 degrees
		v.push_back(vec3(cx + radius * cos(a), cy - depth * sin(a), z));
	}
	polys.push_back(Polygon(v, color));
}

// ------------------------------------------------------------------------------------------
// piece 1: balance scale (a flat picture, like a plaque on the wall)
// ------------------------------------------------------------------------------------------
void Decor::addScalePicture(float cx, float cy)
{
	vec3 frame(0.30f, 0.18f, 0.08f), background(0.10f, 0.13f, 0.18f);
	vec3 gold(0.88f, 0.68f, 0.24f), silver(0.78f, 0.78f, 0.82f);

	addRect(cx, cy, 2.4f, 2.4f, 0, frame);		 // frame
	addRect(cx, cy, 2.15f, 2.15f, 1, background); // background

	addRect(cx, cy - 0.85f, 0.9f, 0.10f, 2, gold);	  // foot
	addRect(cx, cy - 0.76f, 0.5f, 0.08f, 2, gold);	  // step
	addRect(cx, cy - 0.02f, 0.08f, 1.5f, 2, gold);	  // column
	addDisc(cx, cy + 0.78f, 0.09f, 2, gold);		  // top

	// the beam is tilted a little: left end up, right end down
	float L = 0.85f, a = radians(6.0f);
	vec2 pivot(cx, cy + 0.78f);
	vec2 ends[2] = {vec2(pivot.x - L * cos(a), pivot.y + L * sin(a)), vec2(pivot.x + L * cos(a), pivot.y - L * sin(a))};
	addBar(ends[0], ends[1], 0.05f, 3, gold);

	// two pans hanging from the ends of the beam
	for (int i = 0; i < 2; i++)
	{
		vec2 pan(ends[i].x, ends[i].y - 0.7f);
		addBar(ends[i], vec2(pan.x - 0.33f, pan.y), 0.018f, 3, silver); // two chains
		addBar(ends[i], vec2(pan.x + 0.33f, pan.y), 0.018f, 3, silver);
		addHalfDisc(pan.x, pan.y, 0.33f, 0.14f, 3, gold);
	}
}

// ------------------------------------------------------------------------------------------
// piece 2: chess board (a flat picture, like a plaque on the wall)
// ------------------------------------------------------------------------------------------

// a flat chess piece: a round head, a trapezoid body and a base (the king also has a cross)
void Decor::addPiece(float x, float y, vec3 color, bool isKing, int layer)
{
	addRect(x, y - 0.085f, 0.15f, 0.03f, layer, color);										 // base
	addQuad(vec2(x - 0.065f, y - 0.07f), vec2(x + 0.065f, y - 0.07f),
			vec2(x + 0.03f, y + 0.03f), vec2(x - 0.03f, y + 0.03f), layer, color);			 // body
	addDisc(x, y + 0.07f, 0.045f, layer, color);											 // head
	if (isKing)
	{
		addRect(x, y + 0.145f, 0.02f, 0.07f, layer, color); // cross: vertical bar
		addRect(x, y + 0.155f, 0.06f, 0.02f, layer, color); // cross: horizontal bar
	}
}

void Decor::addChessPicture(float cx, float cy)
{
	vec3 frame(0.30f, 0.18f, 0.08f);
	vec3 whiteSquare(0.85f, 0.75f, 0.55f), blackSquare(0.20f, 0.11f, 0.06f);

	addRect(cx, cy, 2.4f, 2.4f, 0, frame); // frame

	// the board: 8 x 8 squares, every square is one flat rectangle
	float sq = 0.24f;
	float start = -4 * sq; // the board is centered in the frame
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			vec3 color = ((i + j) % 2 == 0) ? whiteSquare : blackSquare;
			addRect(cx + start + (i + 0.5f) * sq, cy + start + (j + 0.5f) * sq, sq, sq, 1, color);
		}

	// a few pieces: white ones on black squares, black ones on white squares, so they can be seen
	vec3 white(0.95f, 0.93f, 0.85f), black(0.08f, 0.08f, 0.10f);
	int whitePawns[3][2] = {{1, 2}, {3, 2}, {5, 2}};
	int blackPawns[3][2] = {{2, 4}, {4, 4}, {6, 4}};
	for (int k = 0; k < 3; k++)
	{
		addPiece(cx + start + (whitePawns[k][0] + 0.5f) * sq, cy + start + (whitePawns[k][1] + 0.5f) * sq, white, false, 2);
		addPiece(cx + start + (blackPawns[k][0] + 0.5f) * sq, cy + start + (blackPawns[k][1] + 0.5f) * sq, black, false, 2);
	}
	addPiece(cx + start + 4.5f * sq, cy + start + 1.5f * sq, white, true, 2); // white king (column 4, row 1)
	addPiece(cx + start + 3.5f * sq, cy + start + 5.5f * sq, black, true, 2); // black king (column 3, row 5)
}

// ------------------------------------------------------------------------------------------

Decor::Decor(GLuint plainTexture, GLuint globeTexture) : globe("./models/globe.obj", globeTexture)
{
	plain = plainTexture;

	// the two flat pictures hang on the back wall, one on each side of the poster
	addScalePicture(-4.4f, 2.0f);
	addChessPicture(4.4f, 2.0f);

	// the globe (the 3D model) stands on the floor, left, at the back (its model has its base at y = 0)
	mat4 m = mat4(1.0f);
	m = translate(m, vec3(-5.0f, Layout::FLOOR_Y, -4.6f));
	m = rotate(m, radians(105.0f), vec3(0.0f, 1.0f, 0.0f)); // turn it so Africa and Europe face the room
	globe.transformation(m);
}

void Decor::draw(Shader &shader)
{
	glBindTexture(GL_TEXTURE_2D, plain); // all the flat shapes use the white texture
	for (int i = 0; i < polys.size(); i++)
		polys[i].draw(shader);
}

void Decor::drawModels(Shader &modelShader)
{
	globe.draw(modelShader);
}
