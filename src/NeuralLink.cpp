#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "NeuralLink.h"
#include "NeuralNode.h"
#include "stream_utils.h"

NeuralLink::NeuralLink() {
	checked = false;
	connected = false;
	value = 0;
}

NeuralLink::~NeuralLink() {
	
}


unsigned char * NeuralLink::serialize_genome(unsigned char * stream, unsigned int * length) {
		
	stream = add_int_to_stream(stream, length, &historic_mark);
	stream = add_int_to_stream(stream, length, &input_node_id);
	stream = add_int_to_stream(stream, length, &output_node_id);
	stream = add_double_to_stream(stream, length, &weight);
	
	//printf("NeuralLink::serialize_genome %d\r\n", *length);
	
	return stream;
}

void NeuralLink::deserialize_genome(unsigned char * stream, unsigned int * index) {
	buffer_to_int(stream, &historic_mark, index);
	buffer_to_int(stream, &input_node_id, index);
	buffer_to_int(stream, &output_node_id, index);
	buffer_to_double(stream, &weight, index);
	
	if(weight > 1) weight = 1;
	if(weight < -1) weight = -1;
	
	//printf("NeuralLink::deserialize_genome %d\r\n", *index);
}

unsigned char * NeuralLink::serialize_state(unsigned char * stream, unsigned int * length) {
	
	return stream;
}

void NeuralLink::deserialize_state(unsigned char * stream, unsigned int * index) {
	
}

double NeuralLink::get_value() {
	
	connected = true;
	if(checked) {
		return value;
	} else {
		checked = true;
		value = node->get_value() * weight;
		return value;
	}
}