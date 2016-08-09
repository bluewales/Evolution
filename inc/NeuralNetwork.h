#ifndef neuralnetwork_h_
#define neuralnetwork_h_

#include "direction.h"

class Dude;
class NeuralNode;
class NeuralLink;
class OutputNode;

class NeuralNetwork {
	public:
	
	Dude * dude;
	
	unsigned int number_of_output_nodes;
	unsigned int number_of_nodes;
	unsigned int number_of_links;
	
	NeuralNode ** output_nodes;
	NeuralNode ** neural_nodes;
	NeuralLink ** neural_links;
	
	NeuralNetwork(Dude * dude);
	~NeuralNetwork();
	
	void to_string(char *);
	
	
	void deserialize_genome(unsigned char * stream, unsigned int * index);
	unsigned char * serialize_genome(unsigned char * stream, unsigned int * length);
	void deserialize_state(unsigned char * stream, unsigned int * index);
	unsigned char * serialize_state(unsigned char * stream, unsigned int * length);
	
	void register_output(OutputNode * node);
	turn get_turn();
	
	unsigned int mutate();
};


#endif