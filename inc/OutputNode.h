#ifndef outputnode_h_
#define outputnode_h_

#include "NeuralNode.h"
#include "direction.h"

class NeuralLink;

class OutputNode: public NeuralNode {
	public:
	
	// genome data
	turn output;
	double bias;
	
	
	// state data
	double value;
	
	
	// dynamic data
	unsigned int checked;
	
	unsigned int link_count;
	NeuralLink ** links;
	
	
	
	
	OutputNode();
	~OutputNode();
	
	void deserialize_genome(unsigned char * stream, unsigned int * index);
	unsigned char * serialize_genome(unsigned char * stream, unsigned int * length);
	void deserialize_state(unsigned char * stream, unsigned int * index);
	unsigned char * serialize_state(unsigned char * stream, unsigned int * length);

	double get_value();
	node_type get_node_type();
	void add_link(NeuralLink * link);
	unsigned int mutate();
};


#endif