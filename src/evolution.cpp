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
			
			printf("New field\r\n");
			Field * field = new Field(0);
			
			strcpy(field->filename, (char *)"field/one.field");
			
			field->width = 256;
			field->height = 256;
			field->day = 0;
			
			field->create_empty_field();
			
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
			
			dude->neural_network->register_output(NO_TURN);
			dude->neural_network->register_output(FORWARD);
			dude->neural_network->register_output(BACKWARD);
			dude->neural_network->register_output(RIGHT);
			dude->neural_network->register_output(LEFT);
			dude->neural_network->register_output(RANDOM_TURN);
			
			printf("save genome\r\n");
			
			dude->save_genome();
			
			delete field;
		}
	}
}

