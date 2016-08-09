#ifndef neurallink_h_
#define neurallink_h_

#include "NeuralLink.h"

class NeuralNode;

class NeuralLink {
	public:
	
	// genome data
	unsigned int historic_mark;
	unsigned int input_node_id;
	unsigned int output_node_id;
	double weight;
	
	
	// state data
	
	// dynamic data
	NeuralNode * node;
	unsigned int connected;
	unsigned int checked;
	double value;
	
	
	
	
	NeuralLink();
	~NeuralLink();
	
	void deserialize_genome(unsigned char * stream, unsigned int * index);
	unsigned char * serialize_genome(unsigned char * stream, unsigned int * length);
	void deserialize_state(unsigned char * stream, unsigned int * index);
	unsigned char * serialize_state(unsigned char * stream, unsigned int * length);
	
	double get_value();
};


#endif