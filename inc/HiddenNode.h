#ifndef hiddennode_h_
#define hiddennode_h_

#include "NeuralNode.h"

class NeuralLink;

class HiddenNode: public NeuralNode {
	public:
	
	// genome data
	double bias;
	
	
	// state data
	double value;
	
	
	// dynamic data
	unsigned int checked;
	
	unsigned int link_count;
	NeuralLink ** links;
	
	
	HiddenNode();
	~HiddenNode();
	
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