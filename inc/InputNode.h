#ifndef inputnode_h_
#define inputnode_h_

#include "NeuralNode.h"
#include "direction.h"

class Dude;
class Field;

typedef enum {
	SMELL_SENSE,
	MOVE_ENERGY_SENSE,
	FIGHT_ENERGY_SENSE,
	REPRODUCTIVE_ENERGY_SENSE,
	MOVE_ENERGY_SMELL,
	FIGHT_ENERGY_SMELL,
	REPRODUCTIVE_ENERGY_SMELL,
	RANDOM_SENSE
} sense_type;

class InputNode:public NeuralNode {
	public:
	
	// genome data
	sense_type sense;
	turn facing;
	
	// state data
	
	// dynamic data
	Dude * dude;
	Field * field;
	double value;
	unsigned int checked;
	
	
	InputNode(Dude * dude);
	~InputNode();
	
	void deserialize_genome(unsigned char * stream, unsigned int * index);
	unsigned char * serialize_genome(unsigned char * stream, unsigned int * length);
	void deserialize_state(unsigned char * stream, unsigned int * index);
	unsigned char * serialize_state(unsigned char * stream, unsigned int * length);

	double get_value();
	node_type get_node_type();
	void add_link(NeuralLink * link);
	unsigned int mutate();
};

InputNode * create_random_input_node(Dude * dude);

#endif