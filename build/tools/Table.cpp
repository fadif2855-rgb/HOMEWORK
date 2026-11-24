#include "Table.h"
#include "Layout.h"

using namespace std;
using namespace glm;
using namespace Layout;

void Table::addQuad(vec3 a, vec3 b, vec3 c, vec3 d, vec2 uvMax, GLuint texture, vec3 color)
{
	vector<vec3> v = {a, b, c, d};
	vector<vec2> t = {vec2(0, 0), vec2(0, uvMax.y), vec2(uvMax.x, uvMax.y), vec2(uvMax.x, 0)};
	PolygonWithTexture p(v, t, color);
	p.setTexture(texture);
	parts.push_back(p);
}

void Table::addFlat(float cx, float cz, float w, float d, float y, GLuint texture, vec3 color)
{
	float hw = w / 2, hd = d / 2;
	addQuad(vec3(cx - hw, y, cz + hd), vec3(cx - hw, y, cz - hd),
			vec3(cx + hw, y, cz - hd), vec3(cx + hw, y, cz + hd),
			vec2(1, 1), texture, color);
}

void Table::addZone(int zone, vec3 feltColor, GLuint plain)
{
	float cz = zoneCenterZ(static_cast<Zone>(zone));
	vec3 gold(0.75f, 0.58f, 0.22f);
	vec3 slotColor = feltColor * 0.55f;

	// gold frame (bigger quad) with the felt on top of it (smaller quad)
	addFlat(0, cz, 2 * ZONE_HALF_X + 2 * ZONE_BORDER, ZONE_DEPTH + 2 * ZONE_BORDER, Y_FRAME, plain, gold);
	addFlat(0, cz, 2 * ZONE_HALF_X, ZONE_DEPTH, Y_FELT, plain, feltColor);

	// the five slots a card can sit in
	for (int i = 0; i < SLOTS; i++)
		addFlat(slotX(i), cz, CARD_WIDTH + 2 * SLOT_MARGIN, CARD_HEIGHT + 2 * SLOT_MARGIN, Y_SLOT, plain, slotColor);
}

void Table::addLeg(float cx, float cz, GLuint texture)
{
	float h = 0.15f; // half of the leg's thickness
	vec3 mn(cx - h, FLOOR_Y, cz - h), mx(cx + h, -TABLE_THICKNESS, cz + h);
	vec3 color(0.55f);
	vec2 uv(0.4f, 1.5f);

	addQuad(vec3(mn.x, mn.y, mx.z), vec3(mn.x, mx.y, mx.z), vec3(mx.x, mx.y, mx.z), vec3(mx.x, mn.y, mx.z), uv, texture, color); // near
	addQuad(vec3(mx.x, mn.y, mn.z), vec3(mx.x, mx.y, mn.z), vec3(mn.x, mx.y, mn.z), vec3(mn.x, mn.y, mn.z), uv, texture, color); // far
	addQuad(vec3(mn.x, mn.y, mn.z), vec3(mn.x, mx.y, mn.z), vec3(mn.x, mx.y, mx.z), vec3(mn.x, mn.y, mx.z), uv, texture, color); // left
	addQuad(vec3(mx.x, mn.y, mx.z), vec3(mx.x, mx.y, mx.z), vec3(mx.x, mx.y, mn.z), vec3(mx.x, mn.y, mn.z), uv, texture, color); // right
}

Table::Table(GLuint wood, GLuint plain)
{
	float x = TABLE_HALF_X, z = TABLE_HALF_Z, t = TABLE_THICKNESS;
	vec3 side(0.62f, 0.62f, 0.62f); // sides a bit darker than the top

	// top surface (wood texture repeats 2 x 1.4 times)
	addQuad(vec3(-x, 0, z), vec3(-x, 0, -z), vec3(x, 0, -z), vec3(x, 0, z),
			vec2(2.0f, 1.4f), wood, vec3(1.0f));

	// four sides
	addQuad(vec3(-x, -t, z), vec3(-x, 0, z), vec3(x, 0, z), vec3(x, -t, z), vec2(2, 0.15f), wood, side);	// near
	addQuad(vec3(x, -t, -z), vec3(x, 0, -z), vec3(-x, 0, -z), vec3(-x, -t, -z), vec2(2, 0.15f), wood, side); // far
	addQuad(vec3(-x, -t, -z), vec3(-x, 0, -z), vec3(-x, 0, z), vec3(-x, -t, z), vec2(1.4f, 0.15f), wood, side); // left
	addQuad(vec3(x, -t, z), vec3(x, 0, z), vec3(x, 0, -z), vec3(x, -t, -z), vec2(1.4f, 0.15f), wood, side);	// right

	// four legs
	addLeg(-x + 0.4f, -z + 0.4f, wood);
	addLeg(x - 0.4f, -z + 0.4f, wood);
	addLeg(-x + 0.4f, z - 0.4f, wood);
	addLeg(x - 0.4f, z - 0.4f, wood);

	// the three zones
	addZone(OPPONENT, vec3(0.16f, 0.20f, 0.30f), plain); // slate blue
	addZone(PLAYED, vec3(0.34f, 0.10f, 0.12f), plain);	 // burgundy
	addZone(STOCK, vec3(0.10f, 0.28f, 0.17f), plain);	 // green
}

void Table::draw(Shader &shader)
{
	shader.setMat4("model", mat4(1.0f));
	for (int i = 0; i < parts.size(); i++)
	{
		parts[i].transformation(mat4(1.0f));
		parts[i].draw(shader);
	}
}
