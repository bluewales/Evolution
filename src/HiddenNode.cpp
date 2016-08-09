#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "HiddenNode.h"
#include "NeuralLink.h"
#include "stream_utils.h"
#include "mutation.h"
#include "fastRandom.h"

HiddenNode::HiddenNode() {
	historic_mark = 0;
	connected = false;
	
	checked = false;
	
	start_value = 0;
	value = 0;
	
	link_count = 0;
	links = 0;
	
	bias = 0;
}

HiddenNode::~HiddenNode() {
	if(links) {
		free(links);
	}
}

unsigned char * HiddenNode::serialize_genome(unsigned char * stream, unsigned int * length) {
	stream = add_int_to_stream(stream, length, &historic_mark);
	stream = add_double_to_stream(stream, length, &start_value);
	stream = add_double_to_stream(stream, length, &bias);
	
	return stream;
}

void HiddenNode::deserialize_genome(unsigned char * stream, unsigned int * index) {
	buffer_to_int(stream, &historic_mark, index);
	buffer_to_double(stream, &start_value, index);
	buffer_to_double(stream, &bias, index);
	
	value = start_value;
}

unsigned char * HiddenNode::serialize_state(unsigned char * stream, unsigned int * length) {
	stream = add_double_to_stream(stream, length, &value);
	return stream;
}

void HiddenNode::deserialize_state(unsigned char * stream, unsigned int * index) {
	buffer_to_double(stream, &value, index);
}

double HiddenNode::get_value() {
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

node_type HiddenNode::get_node_type() {
	return HIDDEN_NODE;
}

void HiddenNode::add_link(NeuralLink * link) {
	links = (NeuralLink **)realloc(links, (link_count + 1) * sizeof(NeuralLink *));
	links[link_count] = link;
	link_count++;
}


unsigned int HiddenNode::mutate() {
	unsigned int changed = false;
	unsigned int r = SHR128() & 1;
	if(r) {
		changed = mutate_double(&start_value);
	} else {
		changed = mutate_double(&bias);
	}
	return changed;
}