#pragma once
#ifndef LAYOUT_H
#define LAYOUT_H

// All the measurements of the scene in ONE place, so the table, the cards and
// the cameras always agree with each other.
// Coordinate system: X = right, Y = up, Z = towards the player.
// The table's top surface is the plane y = 0.
namespace Layout
{
	// ---- table ----
	const float TABLE_HALF_X = 3.2f;
	const float TABLE_HALF_Z = 2.2f;
	const float TABLE_THICKNESS = 0.25f;

	// ---- the three zones (bands that run along X, stacked along Z) ----
	//   far from the player (-Z)  : OPPONENT  (stays empty)
	//   middle                    : PLAYED    (cards the player has played)
	//   close to the player (+Z)  : STOCK     (the player's deck / hand)
	enum Zone { OPPONENT = 0, PLAYED = 1, STOCK = 2 };

	const float ZONE_DEPTH = 1.25f;
	const float ZONE_GAP = 0.10f;
	const float ZONE_HALF_X = 3.0f;
	const float ZONE_BORDER = 0.06f;

	// ---- cards ----
	const float CARD_WIDTH = 0.7f;
	const float CARD_HEIGHT = 1.0f;
	const int SLOTS = 5;
	const float SLOT_PITCH = 1.1f;
	const float SLOT_MARGIN = 0.05f; // slot is a bit bigger than the card

	inline float zoneCenterZ(Zone z) { return (static_cast<int>(z) - 1) * (ZONE_DEPTH + ZONE_GAP); }
	inline float slotX(int i) { return (i - SLOTS / 2) * SLOT_PITCH; }

	// tiny heights used to avoid z-fighting between layers lying on the table
	const float Y_FRAME = 0.004f;
	const float Y_FELT = 0.008f;
	const float Y_SLOT = 0.012f;
	const float Y_CARD = 0.016f;

	// ---- the room around the table ----
	const float FLOOR_Y = -1.2f; // the table legs go from here up to the table top
	const float ROOM_HALF_X = 7.0f;
	const float ROOM_HALF_Z = 7.5f;
	const float ROOM_TOP_Y = 7.0f;

	// ---- moving a card from the stock to the play area (key G) ----
	const float MOVE_SECONDS = 0.8f; // how long the move lasts
	const float MOVE_LIFT = 0.6f;	 // how high the card rises in the middle of the move
}

#endif
