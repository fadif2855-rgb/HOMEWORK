#include "Card.h"
#include "Layout.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

// the quad is centered on its local origin, in the XY plane, facing +Z
static PolygonWithTexture makeCardFace(GLuint texture)
{
	float hw = Layout::CARD_WIDTH / 2, hh = Layout::CARD_HEIGHT / 2;
	vector<vec3> v = {vec3(-hw, -hh, 0), vec3(-hw, hh, 0), vec3(hw, hh, 0), vec3(hw, -hh, 0)};
	vector<vec2> t = {vec2(0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0)};
	PolygonWithTexture p(v, t);
	p.setTexture(texture);
	return p;
}

Card::Card(GLuint texture, vec3 pivot) : face(makeCardFace(texture)), pivot(pivot)
{
	startPivot = pivot;
	endPivot = pivot;
	moveTime = 0.0f;
	moving = false;
	played = false;
}

void Card::moveTo(vec3 target)
{
	startPivot = pivot;
	endPivot = target;
	moveTime = 0.0f;
	moving = true;
}

void Card::update(float deltaTime)
{
	if (!moving)
		return;

	moveTime = moveTime + deltaTime / Layout::MOVE_SECONDS;
	if (moveTime >= 1.0f)
	{
		moveTime = 1.0f;
		moving = false;
		played = true;
		pivot = endPivot;
		return;
	}
	// straight line from start to end (a + (b - a) * t) ...
	pivot = startPivot + (endPivot - startPivot) * moveTime;
	// ... plus a lift in the middle of the trip: sin goes 0 -> 1 -> 0
	pivot.y = startPivot.y + Layout::MOVE_LIFT * sin(radians(180.0f * moveTime));
}

void Card::draw(Shader &shader, float standAmount)
{
	const float LEAN = 14.0f; // degrees: how far a standing card leans back

	// -90 degrees : lying on the table, front side up, top of the card pointing away from the player
	//  -LEAN      : standing, front side facing the player, leaning back a little
	float angle = radians(-90.0f + (90.0f - LEAN) * standAmount); // a + (b - a) * t

	mat4 model = mat4(1.0f);
	model = translate(model, pivot);                                // 3. put the near edge in its slot
	model = rotate(model, angle, vec3(1.0f, 0.0f, 0.0f));           // 2. tilt around the near edge
	model = translate(model, vec3(0.0f, Layout::CARD_HEIGHT / 2, 0.0f)); // 1. near edge at the origin
	face.transformation(model);
	face.draw(shader);
}
