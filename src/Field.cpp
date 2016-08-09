#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

void Field::load() {
	
	Reader * r = new Reader((char *)filename, (char *)"r");
	
	unsigned int n_i;
	unsigned char n_buf[4];
	
	// load field size
	n_i = 0;
	r->read(n_buf, 4);
	buffer_to_int(n_buf, &width, &n_i);
	n_i = 0;
	r->read(n_buf, 4);
	buffer_to_int(n_buf, &height, &n_i);
	n_i = 0;
	r->read(n_buf, 4);
	buffer_to_int(n_buf, &historic_count, &n_i);
	n_i = 0;
	r->read(n_buf, 4);
	buffer_to_int(n_buf, &day, &n_i);
	
	width = 256;
	height = 256;
	
	// load vegatation
	unsigned int index = 0;
	unsigned int i, j;
	unsigned char * buffer = (unsigned char *)malloc(width * height);
	
	create_empty_field();
	
	r->read(buffer, width * height);

	for(i = 0; i < width; i++) {
		for(j = 0; j < height; j++) {
			vegatation[i][j] = buffer[index++];
		}
	}
	
	// load dudes
	n_i = 0;
	r->read(n_buf, 4);
	buffer_to_int(n_buf, &dude_population, &n_i);
	
	
	printf("LOAD %d dudes\r\n", dude_population);
	dudes = (Dude **)malloc(dude_population * sizeof(Dude *)) ;
	
	unsigned int stream_size = 0;
	unsigned char * dude_stream = 0;
	
	for(i = 0; i < dude_population; i++) {
		
		unsigned int dude_length;
		n_i = 0;
		r->read(n_buf, 4);
		buffer_to_int(n_buf, &dude_length, &n_i);
		
		if(dude_length > stream_size) {
			dude_stream = (unsigned char *)realloc(dude_stream, dude_length);
			stream_size = dude_length;
		}
		r->read(dude_stream, dude_length);
		
		dudes[i] = new Dude(this);
		unsigned int read_length = 0;
		dudes[i]->deserialize_state(dude_stream, &read_length);
		location_of_dudes[dudes[i]->x][dudes[i]->y] = dudes[i];
		if(read_length != dude_length) {
			printf("ERROR: deserializing dude state in field %d,%d\r\n", dude_length, read_length);
			exit(0);
		}
	}
	
	delete r;
	free(buffer);
	if(dude_stream) {
		free(dude_stream);
	}
}

void Field::create_empty_field() {
	unsigned int i;
	vegatation = (unsigned char **)malloc(width * sizeof(unsigned char *));
	location_of_dudes = (Dude ***)malloc(width * sizeof(Dude **));
	for(i = 0; i < width; i++) {
		vegatation[i] = (unsigned char *)malloc(height * sizeof(unsigned char));
		location_of_dudes[i] = (Dude **)malloc(height * sizeof(Dude *));
		memset(location_of_dudes[i], 0, height * sizeof(Dude *));
	}
}

void Field::save() {

	unsigned int index = 0;
	Reader * r = new Reader(filename, (char *)"w");
	
	// save area size
	unsigned char n_buf[4];
	int_to_buffer(&width, n_buf);
	
	r->write(n_buf, 4);
	
	int_to_buffer(&height, n_buf);
	r->write(n_buf, 4);
	
	int_to_buffer(&historic_count, n_buf);
	r->write(n_buf, 4);
	
	int_to_buffer(&day, n_buf);
	r->write(n_buf, 4);
	
	// save vegatation
	unsigned char * buffer = (unsigned char *)malloc(width * height);
	
	unsigned int i, j;
	for(i = 0; i < width; i++) {
		for(j = 0; j < height; j++) {
			buffer[index++] = vegatation[i][j];
		}
	}
	r->write(buffer, width * height);
	
	unsigned int live_count = dude_population;
	for(i = 0; i < dude_population; i++) {
		if(dudes[i]->dead) {
			live_count--;
		}
	}
	
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
		unsigned char filename[SHA_DIGEST_LENGTH*2+1];
		
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
			unsigned char * filename = genomes->map[i].key;
			unsigned char * genome = hmget(genomes, filename);
			r->write(filename, SHA_DIGEST_LENGTH*2);
			r->write(genome, genome_length);
			free(genome);
		}
	}
	hmdestroy(genomes);
	
	
	// save dudes
	int_to_buffer(&live_count, n_buf);
	r->write(n_buf, 4);
	
	printf("SAVE %d dudes\r\n", live_count);
	
	for(i = 0; i < dude_population; i++) {
		if(dudes[i]->dead) {
			continue;
		}
			
		unsigned int dude_length = 0;
		buffer = dudes[i]->serialize_state(buffer, &dude_length);
		
		int_to_buffer(&dude_length, n_buf);
		r->write(n_buf, 4);
		r->write(buffer, dude_length);
	}
	
	delete r;
	free(buffer);

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