#include "Room.h"
#include "Layout.h"

using namespace std;
using namespace glm;
using namespace Layout;

void Room::addQuad(vec3 a, vec3 b, vec3 c, vec3 d, vec2 uvMax, GLuint texture, vec3 color)
{
	vector<vec3> v = {a, b, c, d};
	vector<vec2> t = {vec2(0, 0), vec2(0, uvMax.y), vec2(uvMax.x, uvMax.y), vec2(uvMax.x, 0)};
	PolygonWithTexture p(v, t, color);
	p.setTexture(texture);
	parts.push_back(p);
}

Room::Room(GLuint wallpaper, GLuint floorTexture, GLuint wood, GLuint plain, GLuint poster)
{
	float x = ROOM_HALF_X, z = ROOM_HALF_Z;
	float y0 = FLOOR_Y, y1 = ROOM_TOP_Y;
	float tile = 3.5f; // size of one wallpaper / floor tile (the texture repeats)

	// floor
	addQuad(vec3(-x, y0, z), vec3(-x, y0, -z), vec3(x, y0, -z), vec3(x, y0, z),
			vec2(2 * x / tile, 2 * z / tile), floorTexture, vec3(1.0f));

	// ceiling (plain dark color)
	addQuad(vec3(-x, y1, -z), vec3(-x, y1, z), vec3(x, y1, z), vec3(x, y1, -z),
			vec2(1, 1), plain, vec3(0.18f, 0.15f, 0.14f));

	// four walls with the wallpaper
	vec3 wallColor(0.95f);
	addQuad(vec3(-x, y0, -z), vec3(-x, y1, -z), vec3(x, y1, -z), vec3(x, y0, -z),
			vec2(2 * x / tile, (y1 - y0) / tile), wallpaper, wallColor); // back wall
	addQuad(vec3(x, y0, z), vec3(x, y1, z), vec3(-x, y1, z), vec3(-x, y0, z),
			vec2(2 * x / tile, (y1 - y0) / tile), wallpaper, wallColor); // front wall (behind the player)
	addQuad(vec3(-x, y0, z), vec3(-x, y1, z), vec3(-x, y1, -z), vec3(-x, y0, -z),
			vec2(2 * z / tile, (y1 - y0) / tile), wallpaper, wallColor); // left wall
	addQuad(vec3(x, y0, -z), vec3(x, y1, -z), vec3(x, y1, z), vec3(x, y0, z),
			vec2(2 * z / tile, (y1 - y0) / tile), wallpaper, wallColor); // right wall

	// skirting boards (a low wooden strip along the bottom of every wall)
	float s = 0.3f, e = 0.01f;
	vec3 boardColor(0.5f);
	addQuad(vec3(-x, y0, -z + e), vec3(-x, y0 + s, -z + e), vec3(x, y0 + s, -z + e), vec3(x, y0, -z + e), vec2(8, 0.3f), wood, boardColor);
	addQuad(vec3(x, y0, z - e), vec3(x, y0 + s, z - e), vec3(-x, y0 + s, z - e), vec3(-x, y0, z - e), vec2(8, 0.3f), wood, boardColor);
	addQuad(vec3(-x + e, y0, z), vec3(-x + e, y0 + s, z), vec3(-x + e, y0 + s, -z), vec3(-x + e, y0, -z), vec2(8, 0.3f), wood, boardColor);
	addQuad(vec3(x - e, y0, -z), vec3(x - e, y0 + s, -z), vec3(x - e, y0 + s, z), vec3(x - e, y0, z), vec2(8, 0.3f), wood, boardColor);

	addPoster(poster);
}

// The poster is cut into 24 vertical strips. Every strip is a small quad whose top edge sits at a
// different height, so the top-right corner of the picture looks torn off (the wall shows through).
void Room::addPoster(GLuint posterTexture)
{
	const int N = 24;
	float w = 3.28f, h = 2.6f;	 // size of the whole poster (same ratio as the painting, so it is not stretched)
	float cx = 0.0f;			 // centered on the back wall
	float bottom = 0.6f;		 // height of its bottom edge
	float z = -ROOM_HALF_Z + 0.02f; // just in front of the wall

	// small irregularities so the tear is not a straight line
	float jitter[N + 1] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
						   0.4f, -0.5f, 0.6f, -0.3f, 0.7f, -0.6f, 0.3f, -0.7f, 0.5f, -0.4f, 0.6f, -0.2f};

	// how much of the height is missing at every vertical cut
	float missing[N + 1];
	for (int i = 0; i <= N; i++)
	{
		float u = (float)i / N;
		missing[i] = 0.0f;
		if (u > 0.55f) // the tear starts here and grows towards the right edge
			missing[i] = (u - 0.55f) / 0.45f * 0.85f * h + jitter[i] * 0.07f;
		if (missing[i] < 0.0f)
			missing[i] = 0.0f;
		if (missing[i] > h - 0.05f)
			missing[i] = h - 0.05f;
	}

	for (int i = 0; i < N; i++)
	{
		float ul = (float)i / N, ur = (float)(i + 1) / N;
		float xl = cx - w / 2 + w * ul, xr = cx - w / 2 + w * ur;
		float topL = bottom + h - missing[i], topR = bottom + h - missing[i + 1];

		vector<vec3> v = {vec3(xl, bottom, z), vec3(xl, topL, z), vec3(xr, topR, z), vec3(xr, bottom, z)};
		// the picture is not squeezed: v follows the real height
		vector<vec2> t = {vec2(ul, 0), vec2(ul, (topL - bottom) / h), vec2(ur, (topR - bottom) / h), vec2(ur, 0)};
		PolygonWithTexture p(v, t);
		p.setTexture(posterTexture);
		parts.push_back(p);
	}
}

void Room::draw(Shader &shader)
{
	for (int i = 0; i < parts.size(); i++)
	{
		parts[i].transformation(mat4(1.0f));
		parts[i].draw(shader);
	}
}
