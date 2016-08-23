#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <openssl/sha.h>

#include "Field.h"
#include "Reader.h"
#include "fastRandom.h"
#include "stream_utils.h"
#include "hashMap.h"

Field::Field(char * filename) {
	historic_count = 0;
	if(filename) {
		strcpy(this->filename, filename);
		load();
	}
}

Field::~Field() {
	save();
	
	unsigned int i;
	for(i = 0; i < width; i++) {
		delete vegatation[i];
		delete location_of_dudes[i];
	}
	delete vegatation;
	delete location_of_dudes;
	for(i = 0; i < dude_population; i++) {
		delete dudes[i];
	}
}

void Field::advance_state() {
	// grow vegitation
	unsigned int i, j;
	unsigned int dessert_size = 0;
	for(i = 0; i < width; i++) {
		for(j = 0; j < height; j++) {
			if(vegatation[i][j] < 255) {
				if(i >= dessert_size && width - dessert_size > i && j >= dessert_size && height - dessert_size > j) {
					vegatation[i][j]++;
				}
			}
		}
	}
	char dude_string[4096];
	for(i = 0; i < dude_population; i++) {
		
		
		dudes[i]->act();
		/*
		dudes[i]->to_string(dude_string);
		printf("% 5d: %s\r\n",i, dude_string);
		*/
		
	}
	Reader * r = new Reader((char *)"log.txt", (char *)"w");
	dudes[0]->to_string(dude_string);
	r->write((unsigned char *)dude_string, strlen(dude_string));
	day++;
	delete r;
};


void Field::create_empty_field() {
	unsigned int i;
	vegatation = (unsigned char **)malloc(width * sizeof(unsigned char *));
	location_of_dudes = (Dude ***)malloc(width * sizeof(Dude **));
	for(i = 0; i < width; i++) {
		vegatation[i] = (unsigned char *)malloc(height * sizeof(unsigned char));
		memset(vegatation[i], 255, height * sizeof(unsigned char));
		
		location_of_dudes[i] = (Dude **)malloc(height * sizeof(Dude *));
		memset(location_of_dudes[i], 0, height * sizeof(Dude *));
	}
}

void Field::load() {
	
	Reader * r = new Reader((char *)filename, (char *)"r");
	
	unsigned int index = 0;
	unsigned char * buffer = (unsigned char *)malloc(r->filesize());

	r->readfile(buffer);
	
	// load field size
	
	buffer_to_int(buffer, &width, &index);
	buffer_to_int(buffer, &height, &index);
	buffer_to_int(buffer, &historic_count, &index);
	buffer_to_int(buffer, &day, &index);
	
	
	// load vegatation
	unsigned int i, j;
	
	create_empty_field();
	
	for(i = 0; i < width; i++) {
		for(j = 0; j < height; j++) {
			buffer_to_char(buffer, &vegatation[i][j], &index);
		}
	}

	unsigned int genome_count = 0;
	buffer_to_int(buffer, &genome_count, &index);

	hashMap * genomes = hmcreate();
	unsigned char * genome_blob = 0;
	unsigned int genome_blob_length = 0;

	for(i = 0; i < genome_count; i++) {

		unsigned char hash[SHA_DIGEST_LENGTH];
		unsigned int genome_length = 0;
		unsigned int genome_buffer_length = 0;

		copy_buffer(buffer, hash, SHA_DIGEST_LENGTH, &index);
		buffer_to_int(buffer, &genome_length, &index);

		if(genome_length > genome_buffer_length) {
			genome_blob = realloc(genome_blob, (genome_blob_length+genome_length) * sizeof(char));
		}

		copy_buffer(buffer, &genome_blob[genome_blob_length], genome_length, &index);

		hmset(genomes, hash, &genome_blob[genome_blob_length]);
		genome_blob_length += genome_length;
	}


	
	// load dudes
	buffer_to_int(buffer, &dude_population, &index);
	
	
	printf("LOAD %d dudes\r\n", dude_population);
	dudes = (Dude **)malloc(dude_population * sizeof(Dude *)) ;
	
	
	for(i = 0; i < dude_population; i++) {
		
		dudes[i] = new Dude(this);
		dudes[i]->deserialize_state(buffer, &index, genomes);
		location_of_dudes[dudes[i]->x][dudes[i]->y] = dudes[i];
	}
	
	delete r;
	free(buffer);
}


void Field::save() {

	// save area size
	unsigned char * stream = 0;
	unsigned int length = 0;

	stream = add_int_to_stream(stream, &length, &width);
	stream = add_int_to_stream(stream, &length, &height);
	stream = add_int_to_stream(stream, &length, &historic_count);
	stream = add_int_to_stream(stream, &length, &day);
	
	unsigned int i, j;
	for(i = 0; i < width; i++) {
		for(j = 0; j < height; j++) {
			stream = add_char_to_stream(stream, &length, vegatation[i][j]);
		}
	}
	
	
	// save genomes
	unsigned int genome_count = 0;
	char ** hashes = 0;
	char ** genomes = 0;

	unsigned int genome_length = 0;

	for(i = 0; i < dude_population; i++) {
		if(dudes[i]->dead) {
			continue;
		}
		
		unsigned int this_genome_length = 0;
		unsigned char * genome = dudes[i]->serialize_genome(0, &this_genome_length);
		
		if(genome_length == this_genome_length || genome_length == 0) {
			genome_length = this_genome_length;
		} else {
			printf("Field::save() Genome length error\r\n");
			exit(0);
		}
		
		unsigned char hash[SHA_DIGEST_LENGTH];
		
		SHA1(genome, genome_length, hash);

		unsigned int already_logged = 0;
		for(j = 0; j < genome_count; j++) {
			if(!memcmp(hash[j], hash, SHA_DIGEST_LENGTH)) {
				already_logged = 1;
				break;
			}
		}

		if(already_logged) {
			continue;
		}

		unsigned int this_index = genome_count;
		genome_count += 1;

		hashes = realloc(hashes, genome_count*sizeof(unsigned char *));
		genomes = realloc(genomes, genome_count*sizeof(unsigned char *));

		hashes[this_index] = (unsigned char *)malloc(SHA_DIGEST_LENGTH*sizeof(unsigned char));
		genomes[this_index] = (unsigned char *)malloc(genome_length*sizeof(unsigned char));

		memcpy(hashes[this_index], hash, SHA_DIGEST_LENGTH);
		memcpy(genomes[this_index], genome, genome_length);
	}
	stream = add_int_to_stream(stream, &length, &genome_count);
	
	for(i = 0; i < genome_count; i++) {

		stream = add_buffer_to_stream(stream, &length, hashes[i], SHA_DIGEST_LENGTH);
		stream = add_int_to_stream(stream, &length, &genome_length);
		stream = add_buffer_to_stream(stream, &length, genome[i], genome_length);

		free(genomes[i]);
		free(hashes[i]);
		
	}

	free(genomes);
	free(hashes);



	
	unsigned int live_count = dude_population;
	for(i = 0; i < dude_population; i++) {
		if(dudes[i]->dead) {
			live_count--;
		}
	}
	stream = add_int_to_stream(stream, &length, &live_count);

	printf("SAVE %d dudes\r\n", live_count);
	
	for(i = 0; i < dude_population; i++) {
		if(dudes[i]->dead) {
			continue;
		}
			
		stream = dudes[i]->serialize_state(stream, &length);
		
		
	}

	Reader * r = new Reader(filename, (char *)"w");
	r->write(stream, length);
	delete r;
	free(stream);

}

void Field::move(Dude * dude, turn t) {
	unsigned int new_x = dude->x;
	unsigned int new_y = dude->y;
	direction dir = get_direction_after_turn(dude->orientation, t);
	switch(dir) {
		case NO_DIRECTION:
			break;
		case NORTH:
			new_y = (new_y - 1) % height;
			break;
		case SOUTH:
			new_y = (new_y + 1) % height;
			break;
		case EAST:
			new_x = (new_x + 1) % width;
			break;
		case WEST:
			new_x = (new_x - 1) % width;
			break;
		case RANDOM_DIRECTION:
			unsigned int r = SHR128() & 0x3;
			switch(r) {
				case 0:
					move(dude, FORWARD);
					break;
				case 1:
					move(dude, BACKWARD);
					break;
				case 2:
					move(dude, LEFT);
					break;
				case 3:
					move(dude, RIGHT);
					break;
			}
			return;
		break;
	}
	
	unsigned int move_okay = true;
	
	if((new_x != dude->x || new_y != dude->y) && location_of_dudes[new_x][new_y]) {
		move_okay = dude->fight(location_of_dudes[new_x][new_y]);
	}
	
	location_of_dudes[dude->x][dude->y] = 0;
	
	if(move_okay) {
		location_of_dudes[new_x][new_y] = dude;
		
		dude->x = new_x;
		dude->y = new_y;
		
		if(dir != NO_DIRECTION) {
			dude->orientation = dir;
		}
		
		dude->eat(vegatation[dude->x][dude->y]);
		vegatation[dude->x][dude->y] = 0;
	}
	
}

double Field::smell_vegetation(Dude * dude, turn t) {
	unsigned int x = dude->x;
	unsigned int y = dude->y;
	direction dir = get_direction_after_turn(dude->orientation, t);
	switch(dir) {
		case NO_DIRECTION:
			return 0;
		case NORTH:
			y = (y - 1) % height;
			break;
		case SOUTH:
			y = (y + 1) % height;
			break;
		case EAST:
			x = (x + 1) % width;
			break;
		case WEST:
			x = (x - 1) % width;
			break;
		case RANDOM_DIRECTION:
			return 0;
		break;
	}
	double value = ((double)vegatation[x][y]) / 255.0;
	return value;
}

double Field::smell_move_energy(Dude * dude, turn t) {
	unsigned int x = dude->x;
	unsigned int y = dude->y;
	direction dir = get_direction_after_turn(dude->orientation, t);
	switch(dir) {
		case NO_DIRECTION:
			return 0;
		case NORTH:
			y = (y - 1) % height;
			break;
		case SOUTH:
			y = (y + 1) % height;
			break;
		case EAST:
			x = (x + 1) % width;
			break;
		case WEST:
			x = (x - 1) % width;
			break;
		case RANDOM_DIRECTION:
			return 0;
		break;
	}
	double value = 0;
	if(location_of_dudes[x][y]) {
		double energy = location_of_dudes[x][y]->regular_energy_level;
		double max_energy = dude->regular_energy_max;
		value = energy / max_energy;
		if(value > 1) value = 1;
	} else {
		value = -1;
	}
	return value;
}
double Field::smell_fight_energy(Dude * dude, turn t) {
	unsigned int x = dude->x;
	unsigned int y = dude->y;
	direction dir = get_direction_after_turn(dude->orientation, t);
	switch(dir) {
		case NO_DIRECTION:
			return 0;
		case NORTH:
			y = (y - 1) % height;
			break;
		case SOUTH:
			y = (y + 1) % height;
			break;
		case EAST:
			x = (x + 1) % width;
			break;
		case WEST:
			x = (x - 1) % width;
			break;
		case RANDOM_DIRECTION:
			return 0;
		break;
	}
	double value = 0;
	if(location_of_dudes[x][y]) {
		double energy = location_of_dudes[x][y]->fight_energy_level;
		double max_energy = dude->fight_energy_max;
		value = energy / max_energy;
		if(value > 1) value = 1;
	} else {
		value = -1;
	}
	return value;
}
double Field::smell_reproductive_energy(Dude * dude, turn t) {
	unsigned int x = dude->x;
	unsigned int y = dude->y;
	direction dir = get_direction_after_turn(dude->orientation, t);
	switch(dir) {
		case NO_DIRECTION:
			return 0;
		case NORTH:
			y = (y - 1) % height;
			break;
		case SOUTH:
			y = (y + 1) % height;
			break;
		case EAST:
			x = (x + 1) % width;
			break;
		case WEST:
			x = (x - 1) % width;
			break;
		case RANDOM_DIRECTION:
			return 0;
		break;
	}
	double value = 0;
	if(location_of_dudes[x][y]) {
		double energy = location_of_dudes[x][y]->reproductive_energy_level;
		double max_energy = dude->reproductive_energy_max;
		value = energy / max_energy;
		if(value > 1) value = 1;
	} else {
		value = -1;
	}
	return value;
}

void Field::add_dude(Dude * new_dude, unsigned int x, unsigned int y) {
	int d = 0;
	
	for(d = 1; d < 10; d++) {
		int dx, dy, edx, edy, ddx, ddy;
		if(SHR128() & 0x1) {
			dx = d;
			ddx = -1;
			edx = (-1 * d) - 1;
		} else {
			dx = -1 * d;
			ddx = 1;
			edx = d + 1;
		}
		if(SHR128() & 0x1) {
			dy = d;
			ddy = -1;
			edy = (-1 * d) - 1;
		} else {
			dy = -1 * d;
			ddy = 1;
			edy = d + 1;
		}
		for(; dx != edx; dx+=ddx) {
			for(; dy != edy; dy+=ddy) {
				unsigned int new_x = (dx + x) % width;
				unsigned int new_y = (dy + y) % height;
				
				if(!location_of_dudes[new_x][new_y]) {
					
					new_dude->x = new_x;
					new_dude->y = new_y;
					
					//char dude_string[512];
					//new_dude->to_string(dude_string);
					//printf("New dude: %s\r\n",dude_string);
					
					location_of_dudes[new_x][new_y] = new_dude;
					
					dudes = (Dude **)realloc(dudes, (dude_population+1) * sizeof(Dude *));
					dudes[dude_population] = new_dude;
					dude_population++;
					
					return;
				}
			}
		}
	}
}

unsigned int Field::get_historic_count() {
	unsigned int result = historic_count;
	historic_count++;
	return result;
}