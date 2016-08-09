#ifndef direction_h_
#define direction_h_

typedef enum {
	NO_DIRECTION,
	NORTH,
	SOUTH,
	EAST,
	WEST,
	RANDOM_DIRECTION
} direction;

typedef enum {
	NO_TURN,
	FORWARD,
	BACKWARD,
	RIGHT,
	LEFT,
	RANDOM_TURN
} turn;

turn random_turn();

direction random_direction();
direction get_direction_after_turn(direction orientation, turn t);

#endif