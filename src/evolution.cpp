#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "Field.h"
#include "Dude.h"
#include "OutputNode.h"
#include "NeuralNetwork.h"
#include "InputNode.h"
#include "NeuralLink.h"
#include "fastRandom.h"
#include "direction.h"


int main(int argc, char *argv[]) {
	
	unsigned int i,j;
	
	sfastRandom(time(0));
	
	if(argc == 1) {
		for(i = 0; i < 1000; i++)  {
			Field * field = new Field((char *)"field/one.field");
			field->advance_state();
			delete field;
		}
	} else {
		printf("%s\r\n", argv[1]);
		if(!strcmp("reset",argv[1])) {
			
			printf("Erase genome\r\n");
			for(i = 0; i < 256; i++) {
				char command[64];
				sprintf(command, "exec rm dudes/%02X*.dude 2> /dev/null", i);
				printf("%s\r\n", command);
				system(command);
			}
			
			
			printf("New field\r\n");
			Field * field = new Field(0);
			
			strcpy(field->filename, (char *)"field/one.field");
			
			field->width = 256;
			field->height = 256;
			field->day = 0;
			
			field->create_empty_field();
			
			for(i = 0; i < field->width; i++) {
				for(j = 0; j < field->height; j++) {
					field->vegatation[i][j] = 255;
				}
			}
			
			field->dude_population = 0;
			
			printf("Seed dude\r\n");
			Dude * dude = new Dude(field);
			dude->mutation_day = 0;
			
			memcpy(dude->parent_genome, "The first genome did not mutate from another genome", GENOME_FILENAME_LENGTH);
			
			unsigned char attribute_buffer[10];
			
			attribute_buffer[0] = 244;
			attribute_buffer[1] = 253;
			attribute_buffer[2] = 0;
			attribute_buffer[3] = 0;
			attribute_buffer[4] = 21;
			attribute_buffer[5] = 9;
			attribute_buffer[6] = 7;
			attribute_buffer[7] = 0;
			attribute_buffer[8] = 1;
			attribute_buffer[9] = 128;
			
			memcpy(dude->internally_traded_attribute_buffer, attribute_buffer, 10);
			dude->parse_attribute_stream();
			
			dude->regular_energy_level = dude->regular_energy_max;
			dude->fight_energy_level = dude->fight_energy_max;
			dude->reproductive_energy_level = 0;
	
			dude->orientation = random_direction();
			dude->dead = false;
	
			// other attributes
			dude->regular_energy_ratio = 219;
			dude->fight_energy_ratio = 14;
			dude->reproductive_energy_ratio = 76;
			dude->mutation_chance = 16;
			dude->traded_attribute_mutation_chance = 128;
			dude->other_attribute_mutation_chance = 128;
			dude->neaural_network_mutation_chance = 128;
			
			unsigned int x = SHR128() % field->width;
			unsigned int y = SHR128() % field->height;
			
			printf("Place dude\r\n");
			
			field->add_dude(dude,x,y);
			
			printf("New outputs\r\n");
			
			OutputNode * no_turn = new OutputNode();
			no_turn->historic_mark = 1;
			no_turn->output = NO_TURN;
			no_turn->start_value = 0.5;
			dude->neural_network->register_output(no_turn);
			
			OutputNode * forward = new OutputNode();
			forward->historic_mark = 2;
			forward->output = FORWARD;
			forward->start_value = 0.5;
			dude->neural_network->register_output(forward);
			
			OutputNode * backward = new OutputNode();
			backward->historic_mark = 3;
			backward->output = BACKWARD;
			backward->start_value = 0.5;
			dude->neural_network->register_output(backward);
			
			OutputNode * right = new OutputNode();
			right->historic_mark = 4;
			right->output = RIGHT;
			right->start_value = 0.5;
			dude->neural_network->register_output(right);
			
			OutputNode * left = new OutputNode();
			left->historic_mark = 5;
			left->output = LEFT;
			left->start_value = 0.5;
			dude->neural_network->register_output(left);
			
			OutputNode * random = new OutputNode();
			random->historic_mark = 6;
			random->output = RANDOM_TURN;
			random->start_value = 1;
			dude->neural_network->register_output(random);
			
			/*
			InputNode * forward_smell = create_random_input_node(dude);
			forward_smell->sense = SMELL_SENSE;
			forward_smell->facing = FORWARD;
			
			forward_smell->historic_mark = 7;
			
			NeuralLink * forward_link = new NeuralLink();
			forward_link->weight = 1;
			forward_link->historic_mark = 8;
				
			forward_link->input_node_id = 7;
			forward_link->node = forward_smell;
				
			forward_link->output_node_id = 2;
			forward->add_link(forward_link);
				
			dude->neural_network->neural_links = (NeuralLink **)realloc(dude->neural_network->neural_links, (dude->neural_network->number_of_links + 1) * sizeof(NeuralLink *));
			dude->neural_network->neural_links[dude->neural_network->number_of_links] = forward_link;
			dude->neural_network->number_of_links++;
			
			dude->neural_network->neural_nodes = (NeuralNode **)realloc(dude->neural_network->neural_nodes, (dude->neural_network->number_of_nodes+1) * sizeof(NeuralNode *));
			dude->neural_network->neural_nodes[dude->neural_network->number_of_nodes] = forward_smell;
			dude->neural_network->number_of_nodes++;
			
			
			
			InputNode * left_smell = create_random_input_node(dude);
			left_smell->sense = SMELL_SENSE;
			left_smell->facing = LEFT;
			
			left_smell->historic_mark = 9;
			
			NeuralLink * left_link = new NeuralLink();
			left_link->weight = 1;
			left_link->historic_mark = 10;
				
			left_link->input_node_id = 9;
			left_link->node = left_smell;
				
			left_link->output_node_id = 5;
			left->add_link(left_link);
				
			dude->neural_network->neural_links = (NeuralLink **)realloc(dude->neural_network->neural_links, (dude->neural_network->number_of_links + 1) * sizeof(NeuralLink *));
			dude->neural_network->neural_links[dude->neural_network->number_of_links] = left_link;
			dude->neural_network->number_of_links++;
			
			dude->neural_network->neural_nodes = (NeuralNode **)realloc(dude->neural_network->neural_nodes, (dude->neural_network->number_of_nodes+1) * sizeof(NeuralNode *));
			dude->neural_network->neural_nodes[dude->neural_network->number_of_nodes] = left_smell;
			dude->neural_network->number_of_nodes++;
			
			
			InputNode * right_smell = create_random_input_node(dude);
			right_smell->sense = SMELL_SENSE;
			right_smell->facing = RIGHT;
			
			right_smell->historic_mark = 11;
			
			NeuralLink * right_link = new NeuralLink();
			right_link->weight = 1;
			right_link->historic_mark = 12;
				
			right_link->input_node_id = 11;
			right_link->node = right_smell;
				
			right_link->output_node_id = 4;
			right->add_link(right_link);
				
			dude->neural_network->neural_links = (NeuralLink **)realloc(dude->neural_network->neural_links, (dude->neural_network->number_of_links + 1) * sizeof(NeuralLink *));
			dude->neural_network->neural_links[dude->neural_network->number_of_links] = right_link;
			dude->neural_network->number_of_links++;
			
			dude->neural_network->neural_nodes = (NeuralNode **)realloc(dude->neural_network->neural_nodes, (dude->neural_network->number_of_nodes+1) * sizeof(NeuralNode *));
			dude->neural_network->neural_nodes[dude->neural_network->number_of_nodes] = right_smell;
			dude->neural_network->number_of_nodes++;
			*/
			
			field->historic_count = 13;
			
			printf("save genome\r\n");
			
			dude->save_genome();
			
			delete field;
		}
	}
}

