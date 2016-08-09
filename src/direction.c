#include "direction.h"
#include "fastRandom.h"

direction random_direction() {
	switch(SHR128() &0x3) {
		case 0x0:
			return NORTH;
		case 0x1:
			return SOUTH;
		case 0x2:
			return EAST;
		case 0x3:
			return WEST;
		default:
			return NO_DIRECTION;
	}
}

turn random_turn() {
	switch(SHR128() &0x3) {
		case 0x0:
			return FORWARD;
		case 0x1:
			return BACKWARD;
		case 0x2:
			return RIGHT;
		case 0x3:
			return LEFT;
		default:
			return NO_TURN;
	}
}

direction get_direction_after_turn(direction orientation, turn t) {
	switch(t) {
		case NO_TURN :
			return NO_DIRECTION;
		case FORWARD :
			return orientation;
		case BACKWARD:
			switch(orientation) {
				case NO_DIRECTION :
					return NO_DIRECTION;
				case NORTH:
					return SOUTH;
				case SOUTH:
					return NORTH;
				case EAST:
					return WEST;
				case WEST:
					return EAST;
				case RANDOM_DIRECTION:
					return RANDOM_DIRECTION;
				default:
					return NO_DIRECTION;
			}
		case RIGHT:
			switch(orientation) {
				case NO_DIRECTION :
					return NO_DIRECTION;
				case NORTH:
					return EAST;
				case SOUTH:
					return WEST;
				case EAST:
					return SOUTH;
				case WEST:
					return NORTH;
				case RANDOM_DIRECTION:
					return RANDOM_DIRECTION;
				default:
					return NO_DIRECTION;
			}
		case LEFT:
			switch(orientation) {
				case NO_DIRECTION :
					return NO_DIRECTION;
				case NORTH:
					return WEST;
				case SOUTH:
					return EAST;
				case EAST:
					return NORTH;
				case WEST:
					return SOUTH;
				case RANDOM_DIRECTION:
					return RANDOM_DIRECTION;
				default:
					return NO_DIRECTION;
			}
		case RANDOM_TURN:
			return RANDOM_DIRECTION;
		default:
			return NO_DIRECTION;
	}
	
}