#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "InputNode.h"
#include "Dude.h"
#include "Field.h"
#include "fastRandom.h"
#include "stream_utils.h"
#include "direction.h"


InputNode::InputNode(Dude * dude) {
	this->dude = dude;
	this->field = dude->field;
	
	value = 0;
	checked = 0;
}

InputNode::~InputNode() {
	
}

unsigned char * InputNode::serialize_genome(unsigned char * stream, unsigned int * length) {
	
	stream = add_int_to_stream(stream, length, &historic_mark);
	
	unsigned int sense_int = (unsigned int)sense;
	stream = add_int_to_stream(stream, length, &sense_int);
	
	unsigned int facing_int = (unsigned int)facing;
	stream = add_int_to_stream(stream, length, &facing_int);
	
	//printf("InputNode::serialize_genome %d\r\n", *length);
	
	return stream;
}

void InputNode::deserialize_genome(unsigned char * stream, unsigned int * index) {
	buffer_to_int(stream, &historic_mark, index);
	
	unsigned int sense_int;
	buffer_to_int(stream, &sense_int, index);
	sense = (sense_type)sense_int;
	
	unsigned int facing_int;
	buffer_to_int(stream, &facing_int, index);
	facing = (turn)facing_int;
	
	//printf("InputNode::deserialize_genome %d\r\n", *index);
}

unsigned char * InputNode::serialize_state(unsigned char * stream, unsigned int * length) {
	
	//printf("InputNode::serialize_state %d\r\n", *length);
	
	return stream;
}

void InputNode::deserialize_state(unsigned char * stream, unsigned int * index) {
	//printf("InputNode::deserialize_state %d\r\n", *index);
}

double InputNode::get_value() {
	
	connected = true;
	if(checked) {
		return value;
	}
	checked = true;
	
	switch(sense) {
		case SMELL_SENSE :
			value = field->smell_vegetation(dude, facing);
			break;
		case MOVE_ENERGY_SENSE :
			value = ((double)(dude->reproductive_energy_level)) / ((double)(dude->reproductive_energy_max));
			break;
		case FIGHT_ENERGY_SENSE :
			value = ((double)(dude->fight_energy_level)) / ((double)(dude->fight_energy_max));
			break;
		case REPRODUCTIVE_ENERGY_SENSE:
			value = ((double)(dude->regular_energy_level)) / ((double)(dude->regular_energy_max));
			break;
			
		case MOVE_ENERGY_SMELL:
			value = field->smell_move_energy(dude, facing);
			break;
		case FIGHT_ENERGY_SMELL:
			value = field->smell_fight_energy(dude, facing);
			break;
		case REPRODUCTIVE_ENERGY_SMELL:
			value = field->smell_reproductive_energy(dude, facing);
			break;
			
		case RANDOM_SENSE :
		default:
			value = VNI();
			break;
	}
	
	return value;
}

node_type InputNode::get_node_type() {
	return INPUT_NODE;
}

void InputNode::add_link(NeuralLink * link) {
	return;
}

InputNode * create_random_input_node(Dude * dude) {
	InputNode * result = new InputNode(dude);
	
	unsigned int r = SHR128() % 5;
	
	result->facing = FORWARD;
	
	switch(r) {
		case 0:
			result->sense = SMELL_SENSE;
			result->facing = random_turn();
			break;
		case 1:
			result->sense = MOVE_ENERGY_SENSE;
			break;
		case 2:
			result->sense = FIGHT_ENERGY_SENSE;
			break;
		case 3:
			result->sense = REPRODUCTIVE_ENERGY_SENSE;
			break;
		case 4:
			result->sense = MOVE_ENERGY_SMELL;
			result->facing = random_turn();
			break;
		case 5:
			result->sense = FIGHT_ENERGY_SMELL;
			result->facing = random_turn();
			break;
		case 6:
			result->sense = REPRODUCTIVE_ENERGY_SMELL;
			result->facing = random_turn();
			break;
		case 7:
			result->sense = RANDOM_SENSE;
			break;
	}
	
	return result;
}

unsigned int InputNode::mutate() {
	return false;
}
