#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "NeuralNetwork.h"
#include "Dude.h"
#include "stream_utils.h"
#include "NeuralNode.h"
#include "NeuralLink.h"
#include "OutputNode.h"
#include "InputNode.h"
#include "HiddenNode.h"
#include "stream_utils.h"
#include "fastRandom.h"
#include "mutation.h"

NeuralNetwork::NeuralNetwork(Dude * dude) {
	this->dude = dude;
	
	number_of_output_nodes = 0;
	number_of_nodes = 0;
	number_of_links = 0;
	
	output_nodes = 0;
	neural_nodes = 0;
	neural_links = 0;
}

NeuralNetwork::~NeuralNetwork() {
	unsigned int i = 0;
	for(i = 0; i < number_of_nodes; i++) {
		delete neural_nodes[i];
		neural_nodes[i] = 0;
	}
	for(i = 0; i < number_of_links; i++) {
		delete neural_links[i];
	}
	if(neural_nodes) {
		free(neural_nodes);
	}
	if(neural_links) {
		free(neural_links);
	}
	if(output_nodes) {
		free(output_nodes);
	}
}

void NeuralNetwork::to_string(char * string) {
	sprintf(string, "\r\n%d Nodes \r\n%d Outputs: (", number_of_nodes, number_of_output_nodes);
	
	unsigned int i;
	for(i = 0; i < number_of_output_nodes; i++) {
		
		
		
		OutputNode * node = (OutputNode *)output_nodes[i];
		
		sprintf(&string[strlen(string)], "%d:%s:%g:%s%g, ", 
			node->historic_mark,
			(node->output==FORWARD)?"F": (
				(node->output==BACKWARD)?"B": (
					(node->output==RIGHT)?"R": (
						(node->output==LEFT)?"L":(
							(node->output==NO_TURN)?"N":(
								(node->output==RANDOM_TURN)?"r":"?"
							)
						)
					)
				)
			), 
			node->get_value(), 
			node->bias<0?"":"+",
			node->bias
		);
	}
	sprintf(&string[strlen(string)], ") \r\nInputs: (");
	
	for(i = 0; i < number_of_nodes; i++) {
		if(neural_nodes[i]->get_node_type() == INPUT_NODE) {
			InputNode * node = (InputNode *)neural_nodes[i];
			sprintf(&string[strlen(string)], "%d:%s:%s:%g, ",
				node->historic_mark, 
				(node->sense==SMELL_SENSE)?"S": (
					(node->sense==MOVE_ENERGY_SENSE)?"E": (
						(node->sense==FIGHT_ENERGY_SENSE)?"F": (
							(node->sense==REPRODUCTIVE_ENERGY_SENSE)?"R": (
								(node->sense==RANDOM_SENSE)?"r":"?"
							)
						)
					)
				),
				(node->facing==FORWARD)?"F": (
					(node->facing==BACKWARD)?"B": (
						(node->facing==RIGHT)?"R": (
							(node->facing==LEFT)?"L":"?"
						)
					)
				),
				node->get_value()
			);
		}
	}
	
	sprintf(&string[strlen(string)], ") \r\nHidden: (");
	
	for(i = 0; i < number_of_nodes; i++) {
		if(neural_nodes[i]->get_node_type() == HIDDEN_NODE) {
			HiddenNode * node = (HiddenNode *)neural_nodes[i];
			sprintf(&string[strlen(string)], "%d:%g:%s%g, ",
				node->historic_mark,
				node->get_value(), 
				node->bias<0?"":"+",
				node->bias
			);
		}
	}
	
	sprintf(&string[strlen(string)], ") \r\n%d Links: (", number_of_links);
	
	for(i = 0; i < number_of_links; i++) {
		sprintf(&string[strlen(string)], "%d-%g->%d ", neural_links[i]->input_node_id, neural_links[i]->weight, neural_links[i]->output_node_id);
	}
	
	strcat(string, ")");
}



unsigned char * NeuralNetwork::serialize_genome(unsigned char * stream, unsigned int * length) {

	unsigned int i = 0;
	
	stream = add_int_to_stream(stream, length, &number_of_nodes);

	for(i = 0; i < number_of_nodes; i++) {
		
		unsigned int neural_node_type = (unsigned int)neural_nodes[i]->get_node_type();
		stream = add_int_to_stream(stream, length, &neural_node_type);
		
		stream = neural_nodes[i]->serialize_genome(stream, length);
	}
	stream = add_int_to_stream(stream, length, &number_of_links); 

	for(i = 0; i < number_of_links; i++) {
		stream = neural_links[i]->serialize_genome(stream, length);
	}
	
	//printf("NeuralNetwork::serialize_genome %d\r\n", *length);
	
	return stream;
}

void NeuralNetwork::deserialize_genome(unsigned char * stream, unsigned int * index) {
	
	unsigned int i, j;

	buffer_to_int(stream, &number_of_nodes, index);
	//printf("NeuralNetwork::deserialize_genome number_of_nodes %d %d\r\n", number_of_nodes, *index);
	neural_nodes = (NeuralNode **)malloc(number_of_nodes * sizeof(NeuralNode *));
	for(i = 0; i < number_of_nodes; i++) {
		unsigned int neural_node_type;
		buffer_to_int(stream, &neural_node_type, index);
		
		neural_nodes[i] = create_node((node_type)neural_node_type, dude);
		
		neural_nodes[i]->deserialize_genome(stream, index);
		
		if(neural_nodes[i]->get_node_type() == OUTPUT_NODE) {
			output_nodes = (NeuralNode **)realloc(output_nodes, (number_of_output_nodes+1) * sizeof(NeuralNode *));
			output_nodes[number_of_output_nodes] = neural_nodes[i];
			
			number_of_output_nodes++;
		}
	}

	buffer_to_int(stream, &number_of_links, index);
	//printf("NeuralNetwork::deserialize_genome number_of_links %d %d\r\n", number_of_links, *index);
	neural_links = (NeuralLink **)malloc(number_of_links * sizeof(NeuralLink *));
	for(i = 0; i < number_of_links; i++) {
		
		neural_links[i] = new NeuralLink();
		neural_links[i]->deserialize_genome(stream, index);
		for(j = 0; j < number_of_nodes; j++) {
			if(neural_nodes[j]->historic_mark == neural_links[i]->input_node_id) {
				neural_links[i]->node = neural_nodes[j];
			}
			if(neural_nodes[j]->historic_mark == neural_links[i]->output_node_id) {
				neural_nodes[j]->add_link(neural_links[i]);
			}
		}
	}
	
	//printf("NeuralNetwork::deserialize_genome %d\r\n", *index);
}

unsigned char * NeuralNetwork::serialize_state(unsigned char * stream, unsigned int * length) {
	unsigned int i;
	
	for(i = 0; i < number_of_nodes; i++) {
		stream = neural_nodes[i]->serialize_state(stream, length);
	}
	
	//printf("NeuralNetwork::serialize_state %d\r\n", *length);
	
	return stream;
}

void NeuralNetwork::deserialize_state(unsigned char * stream, unsigned int * index) {
	unsigned int i;
	
	for(i = 0; i < number_of_nodes; i++) {
		neural_nodes[i]->deserialize_state(stream, index);
	}
	//printf("NeuralNetwork::deserialize_state %d\r\n", *index);
}

turn NeuralNetwork::get_turn() {
	unsigned int i; 
	NeuralNode * winning_node = 0;
	double winning_value = -1;
	for(i = 0; i < number_of_output_nodes; i++) {
		double new_value = output_nodes[i]->get_value();
		if(new_value > winning_value || (new_value == winning_value && SHR128()%number_of_output_nodes == 0)) {
			winning_node = output_nodes[i];
			winning_value = new_value;
		}
	}
	if(winning_node) {
		return ((OutputNode *)winning_node)->output;
	} else {
		return RANDOM_TURN;
	}
}

void NeuralNetwork::register_output(turn output) {

	OutputNode * node = new OutputNode();
	node->historic_mark = dude->field->get_historic_count();
	node->output = output;
	node->start_value = 0.5;

	output_nodes = (NeuralNode **)realloc(output_nodes, (number_of_output_nodes+1) * sizeof(NeuralNode *));
	output_nodes[number_of_output_nodes] = node;
	number_of_output_nodes++;
	
	neural_nodes = (NeuralNode **)realloc(neural_nodes, (number_of_nodes+1) * sizeof(NeuralNode *));
	neural_nodes[number_of_nodes] = node;
	number_of_nodes++;
}


unsigned int NeuralNetwork::mutate() {
	unsigned int r = SHR128() % 7;
	unsigned int changed = 0;
	unsigned int i;
	
	switch(r) {
		case 0: {
			// add input (connected directly to one or more outputs with links of random wieghts)
			
			printf("Mutate new input node\r\n");
			
			InputNode * new_input_node = create_random_input_node(dude);
			for(i = 0; i < number_of_nodes; i++) {
				if(neural_nodes[i]->get_node_type() == INPUT_NODE) {
					if(((InputNode*)neural_nodes[i])->sense == new_input_node->sense && ((InputNode*)neural_nodes[i])->facing == new_input_node->facing) {
						delete new_input_node;
						return changed;
					}
				}
			}
			new_input_node->historic_mark = dude->field->get_historic_count();
			
			i = SHR128() % number_of_nodes;
			if(neural_nodes[i]->get_node_type() == INPUT_NODE) {
				delete new_input_node;
				return changed;
			}
			{
				NeuralLink * new_link = new NeuralLink();
				new_link->weight = VNI();
				new_link->historic_mark = dude->field->get_historic_count();
				
				new_link->input_node_id = new_input_node->historic_mark;
				new_link->node = new_input_node;
				
				new_link->output_node_id = neural_nodes[i]->historic_mark;
				neural_nodes[i]->add_link(new_link);
				
				neural_links = (NeuralLink **)realloc(neural_links, (number_of_links + 1) * sizeof(NeuralLink *));
				neural_links[number_of_links] = new_link;
				number_of_links++;
			}
			
			neural_nodes = (NeuralNode **)realloc(neural_nodes, (number_of_nodes+1) * sizeof(NeuralNode *));
			neural_nodes[number_of_nodes] = new_input_node;
			number_of_nodes++;
			
			changed = true;
			return changed;
		}
		case 1: {
			// add link between two nodes
			i = SHR128() % number_of_nodes;
			NeuralNode * input_node = neural_nodes[i];
			i = SHR128() % number_of_nodes;
			NeuralNode * output_node = neural_nodes[i];
			
			for(i = 0; i < number_of_links; i++) {
				if(neural_links[i]->output_node_id == output_node->historic_mark && neural_links[i]->input_node_id == input_node->historic_mark) {
					return changed;
				}
			}
			
			if(output_node->get_node_type() != INPUT_NODE) {
				NeuralLink * new_link = new NeuralLink();
				new_link->weight = VNI();
				new_link->historic_mark = dude->field->get_historic_count();
				
				new_link->input_node_id = input_node->historic_mark;
				new_link->node = input_node;
				
				new_link->output_node_id = output_node->historic_mark;
				output_node->add_link(new_link);
				
				neural_links = (NeuralLink **)realloc(neural_links, (number_of_links + 1) * sizeof(NeuralLink *));
				neural_links[number_of_links] = new_link;
				number_of_links++;
				changed = true;
			}
			
			return changed;
		}
		case 2:
			// add hidden node to split a link
			printf("Add a node\r\n");
			if(number_of_links > 0) {
				i = SHR128() % number_of_links;
				NeuralLink * link_to_split = neural_links[i];
				
				NeuralNode * up_stream_node = 0;
				NeuralNode * down_stream_node = 0;
				
				for(i = 0; i < number_of_nodes; i++) {
					if(link_to_split->input_node_id == neural_nodes[i]->historic_mark) {
						up_stream_node = neural_nodes[i];
					}
					if(link_to_split->output_node_id == neural_nodes[i]->historic_mark) {
						down_stream_node = neural_nodes[i];
					}
				}
				
				if(up_stream_node->get_node_type() == HIDDEN_NODE) {
					unsigned int output_links = 0;
					unsigned int input_links = 0;
					for(i = 0; i < number_of_links; i++) {
						if(neural_links[i]->output_node_id == up_stream_node->historic_mark) {
							input_links++;
						}
						if(neural_links[i]->input_node_id == up_stream_node->historic_mark) {
							output_links++;
						}
					}
					if(output_links < 2 && input_links < 2) {
						return changed;
					}
				}
				if(down_stream_node->get_node_type() == HIDDEN_NODE) {
					unsigned int output_links = 0;
					unsigned int input_links = 0;
					for(i = 0; i < number_of_links; i++) {
						if(neural_links[i]->output_node_id == down_stream_node->historic_mark) {
							input_links++;
						}
						if(neural_links[i]->input_node_id == down_stream_node->historic_mark) {
							output_links++;
						}
					}
					if(output_links < 2 && input_links < 2) {
						return changed;
					}
				}
				
				NeuralNode * new_node = create_node(HIDDEN_NODE, dude);
				new_node->historic_mark = dude->field->get_historic_count();
				
				neural_nodes = (NeuralNode **)realloc(neural_nodes, (number_of_nodes + 1) * sizeof(NeuralNode *));
				neural_nodes[number_of_nodes] = new_node;
				number_of_nodes++;
				
				NeuralLink * down_stream_link = link_to_split;
				
				down_stream_link->historic_mark = dude->field->get_historic_count();
				
				down_stream_link->input_node_id = new_node->historic_mark;
				down_stream_link->node = new_node;
				
				NeuralLink * up_stream_link = new NeuralLink();
				
				up_stream_link->weight = 1.0;
				up_stream_link->historic_mark = dude->field->get_historic_count();
				
				up_stream_link->input_node_id = up_stream_node->historic_mark;
				up_stream_link->node = up_stream_node;
				
				up_stream_link->output_node_id = new_node->historic_mark;
				new_node->add_link(up_stream_link);
				
				neural_links = (NeuralLink **)realloc(neural_links, (number_of_links + 1) * sizeof(NeuralLink *));
				neural_links[number_of_links] = up_stream_link;
				number_of_links++;
				
				changed = true;
				
			}
			return changed;
		case 3:
			// change a weight on a link
			printf("Mutate weight of a link\r\n");
			if(number_of_links > 0) {
				i = SHR128() % number_of_links;
				changed = mutate_double(&(neural_links[i]->weight));
			}
			return changed;
		case 4:
			// delete a link
			return changed;
		case 5:
			// delete a node (input or hidden, never output)
			return changed;
		case 6:
			r = SHR128() % number_of_nodes;
			return neural_nodes[r]->mutate();
	}
	return changed;
}
