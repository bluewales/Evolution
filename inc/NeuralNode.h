#ifndef neuralnetnode_h_
#define neuralnetnode_h_

class Dude;
class NeuralLink;

typedef enum {
	INPUT_NODE,
	HIDDEN_NODE,
	OUTPUT_NODE
} node_type;

class NeuralNode {
	public:
	
	// genome data
	unsigned int historic_mark;
	double start_value;
	
	// state data
	
	// dynamic data
	unsigned int connected;
	
	virtual void deserialize_genome(unsigned char * stream, unsigned int * index) = 0;
	virtual unsigned char * serialize_genome(unsigned char * stream, unsigned int * length) = 0;
	virtual void deserialize_state(unsigned char * stream, unsigned int * index) = 0;
	virtual unsigned char * serialize_state(unsigned char * stream, unsigned int * length) = 0;

	virtual double get_value() = 0;
	virtual node_type get_node_type() = 0;
	virtual void add_link(NeuralLink * link) = 0;
	virtual unsigned int mutate() = 0;
	
};

NeuralNode * create_node(node_type type, Dude * dude);


#endif