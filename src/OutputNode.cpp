#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "OutputNode.h"
#include "NeuralLink.h"
#include "stream_utils.h"
#include "mutation.h"
#include "fastRandom.h"

OutputNode::OutputNode() {
	historic_mark = 0;
	connected = false;
	
	checked = false;
	
	start_value = 0;
	value = 0;
	
	link_count = 0;
	links = 0;
	
	output = RANDOM_TURN;
	bias = 0;
}

OutputNode::~OutputNode() {
	if(links) {
		free(links);
	}
}

unsigned char * OutputNode::serialize_genome(unsigned char * stream, unsigned int * length) {
	
	stream = add_int_to_stream(stream, length, &historic_mark);
	unsigned int neural_node_output = (unsigned int)output;
	stream = add_int_to_stream(stream, length, &neural_node_output);
	stream = add_double_to_stream(stream, length, &start_value);
	stream = add_double_to_stream(stream, length, &bias);
	
	//printf("OutputNode::serialize_genome %d\r\n", *length);
	
	return stream;
}

void OutputNode::deserialize_genome(unsigned char * stream, unsigned int * index) {
	
	buffer_to_int(stream, &historic_mark, index);
	unsigned int neural_node_output = 0;
	buffer_to_int(stream, &neural_node_output, index);
	output = (turn)neural_node_output;
	buffer_to_double(stream, &start_value, index);
	buffer_to_double(stream, &bias, index);
	
	//printf("OutputNode::deserialize_genome %d\r\n", *index);
	
	value = start_value;
}

unsigned char * OutputNode::serialize_state(unsigned char * stream, unsigned int * length) {
	
	stream = add_double_to_stream(stream, length, &value);
	
	//printf("OutputNode::serialize_state %d\r\n", *length);
	
	return stream;
}

void OutputNode::deserialize_state(unsigned char * stream, unsigned int * index) {
	buffer_to_double(stream, &value, index);
	//printf("OutputNode::deserialize_state %d\r\n", *index);
}

double OutputNode::get_value() {
	connected = true;
	
	if(!checked) {
		checked = true;
		if(link_count > 0) {
			unsigned int i;
			double sum = 0;
			double total_weight = 0;
			for(i = 0; i < link_count; i++) {
				sum += links[i]->get_value();
				total_weight += (links[i]->weight > 0)?links[i]->weight:(-1 * links[i]->weight);
			}
			
			sum /= link_count;
			value = sum + bias;
			
			if(value > 1) value = 1;
			if(value < -1) value = -1;
		}
	}
	
	return value;
}

node_type OutputNode::get_node_type() {
	return OUTPUT_NODE;
}

void OutputNode::add_link(NeuralLink * link) {
	links = (NeuralLink **)realloc(links, (link_count + 1) * sizeof(NeuralLink *));
	links[link_count] = link;
	link_count++;
}

unsigned int OutputNode::mutate() {
	unsigned int changed = false;
	unsigned int r = SHR128() & 1;
	if(r) {
		changed = mutate_double(&start_value);
	} else {
		changed = mutate_double(&bias);
	}
	return changed;
}