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
		memset(vegatation[i], 255, height * sizeof(Dude *));

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
			buffer_to_char(buffer, &vegatation[i][j], index);
		}
	}
	
	// load dudes
	buffer_to_int(buffer, &dude_population, &index);
	
	
	printf("LOAD %d dudes\r\n", dude_population);
	dudes = (Dude **)malloc(dude_population * sizeof(Dude *)) ;
	
	
	for(i = 0; i < dude_population; i++) {
		
		dudes[i] = new Dude(this);
		unsigned int read_length = 0;
		dudes[i]->deserialize_state(buffer, &index);
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
			stream = add_char_to_stream(stream, &length, &vegatation[i][j]);
		}
	}
	
	/*
	// save genomes
	hashMap * genomes = hmcreate();
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
		char filename[SHA_DIGEST_LENGTH*2+1];
		
		SHA1(genome, genome_length, hash);
		
		for(j = 0; j < SHA_DIGEST_LENGTH; j++) {
			sprintf(&filename[j*2], "%02X", hash[j]);
		}
		if(hmget(genomes, filename)) {
			free(genome);	
		} else {
			hmset(genomes, filename, genome);
		}
	}
	int_to_buffer(&genomes->count, n_buf);
	r->write(n_buf, 4);
	
	for(i = 0; i < genomes->size; i++) {
		if(genomes->map[i].key) {
			char * filename = genomes->map[i].key;
			unsigned char * genome = (unsigned char *)hmget(genomes, filename);
			r->write((unsigned char *)filename, SHA_DIGEST_LENGTH*2);
			r->write(genome, genome_length);
			free(genome);
		}
	}
	hmdestroy(genomes);
	*/
	
	
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