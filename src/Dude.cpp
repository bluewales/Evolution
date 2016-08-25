#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "Dude.h"
#include "Field.h"
#include "Reader.h"
#include "stream_utils.h"
#include "fastRandom.h"
#include "NeuralNetwork.h"
#include "mutation.h"

Dude::Dude(Field * field) {
	this->field = field;
	
	dead = false;
	
	regular_energy_max = 128;
	fight_energy_max = 128;
	reproductive_energy_max = 128;
	live_cost = 2;
	move_cost = 20;
	
	
	
	
	regular_energy_ratio = 128;
	fight_energy_ratio = 64;
	reproductive_energy_ratio = 64;
	mutation_chance = 1280;
	traded_attribute_mutation_chance = 128;
	other_attribute_mutation_chance = 128;
	neaural_network_mutation_chance = 128;
	orientation = random_direction();
	
	neural_network = new NeuralNetwork(this);
	
}

Dude::~Dude() {
	delete neural_network;
}

void Dude::to_string(char * string) {
	sprintf(string, (char*)"(% 4d,% 4d) e:% 4d/% 4d f:% 4d/% 4d r:% 4d/% 4d cost:(% 4d,% 4d) ratios(% 4d:% 4d:% 4d) mutation(% 4d:% 4d:% 4d:% 4d) %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
		x, y,
		regular_energy_level, regular_energy_max,
		fight_energy_level, fight_energy_max,
		reproductive_energy_level, reproductive_energy_max,
		live_cost,
		move_cost,
		regular_energy_ratio,
		fight_energy_ratio,
		reproductive_energy_ratio,
		mutation_chance,
		traded_attribute_mutation_chance,
		other_attribute_mutation_chance,
		neaural_network_mutation_chance,
		internally_traded_attribute_buffer[0],
		internally_traded_attribute_buffer[1],
		internally_traded_attribute_buffer[2],
		internally_traded_attribute_buffer[3],
		internally_traded_attribute_buffer[4],
		internally_traded_attribute_buffer[5],
		internally_traded_attribute_buffer[6],
		internally_traded_attribute_buffer[7],
		internally_traded_attribute_buffer[8],
		internally_traded_attribute_buffer[9]
	);
	neural_network->to_string(&string[strlen(string)]);
}

void Dude::save_genome() {
	
	printf("Can't save genome\r\n");
	exit(0);
	
	unsigned int i;
	unsigned int genome_length = 0;
	unsigned char * genome = serialize_genome(0, &genome_length);
	char filename[GENOME_FILENAME_LENGTH];
	strcpy(filename,"dudes/");
	
	//printf("Dude::save_genome\r\n");
	
	unsigned char hash[SHA_DIGEST_LENGTH];
	
	SHA1(genome, genome_length, hash);
	
	for(i = 0; i < SHA_DIGEST_LENGTH; i++) {
		sprintf(&filename[i*2+6], "%02X", hash[i]);
	}
	strcpy(&filename[i*2+6], ".dude");
	
	Reader * r = new Reader(filename, (char *)"w");
	
	unsigned char n_buf[4];
	int_to_buffer(&genome_length, n_buf);
	
	r->write(n_buf, 4);
	
	r->write(genome, genome_length);
	
	free(genome);
	delete r;
}

void Dude::load_genome(char * filename) {

	printf("Dude::load_genome can't load genome\n");
	exit(0);
	
	Reader * r = new Reader(filename, (char *)"r");
	
	unsigned int n_i = 0;
	unsigned char n_buf[4];
	
	unsigned int genome_length = 0;
	
	r->read(n_buf, 4);
	buffer_to_int(n_buf, &genome_length, &n_i);
	
	unsigned char * genome = (unsigned char *)malloc(genome_length * sizeof(unsigned char));
	
	r->read(genome, genome_length);
	genome_length = 0;
	deserialize_genome(genome, &genome_length);
	free(genome);
	delete r;
}

void Dude::parse_attribute_stream() {
	unsigned int attributes[INTERNALLY_TRADED_ATTRIBUTES];
	
	unsigned int a, i, c;
	for(a = 0; a < INTERNALLY_TRADED_ATTRIBUTES; a++) {
		unsigned int sum = 0;
		for(i = 0; i < INTERNALLY_TRADED_ATTRIBUTES - 1; i++) {
			if(i >= a) {
				unsigned int offset = i - a;
				for(c = 0; c < a; c++) {
					offset += INTERNALLY_TRADED_ATTRIBUTES - c - 1;
				}
				sum += internally_traded_attribute_buffer[offset];
			}
			if(i < a) {
				unsigned int offset = a - 1;
				for(c = 0; c < i; c++) {
					offset += INTERNALLY_TRADED_ATTRIBUTES - c - 2;
				}
				sum += 255 - internally_traded_attribute_buffer[offset];
			}
		}
		attributes[a] = sum / (INTERNALLY_TRADED_ATTRIBUTES - 1);
	}
	
	regular_energy_max = 1 + attributes[0];
	fight_energy_max = 1 + attributes[1];
	reproductive_energy_max = 256 -  attributes[2];
	live_cost = sqrt(255 - attributes[3]) + 2;
	move_cost = sqrt(255 - attributes[4]) + 1;
}

unsigned char * Dude::serialize_genome(unsigned char * stream, unsigned int * length) {
	*length = 0;
	
	stream = add_int_to_stream(stream, length, &mutation_day);
	
	unsigned int attribute_stream_length = (INTERNALLY_TRADED_ATTRIBUTES * (INTERNALLY_TRADED_ATTRIBUTES-1)) / 2;
	
	stream = add_buffer_to_stream(stream, length, internally_traded_attribute_buffer, attribute_stream_length);

	stream = add_int_to_stream(stream, length, &regular_energy_ratio);
	stream = add_int_to_stream(stream, length, &fight_energy_ratio);
	stream = add_int_to_stream(stream, length, &reproductive_energy_ratio);
	stream = add_int_to_stream(stream, length, &mutation_chance);
	stream = add_int_to_stream(stream, length, &traded_attribute_mutation_chance);
	stream = add_int_to_stream(stream, length, &other_attribute_mutation_chance);
	
	stream = neural_network->serialize_genome(stream, length);
	
	//printf("Dude::serialize_genome %d\r\n", *length);
	
	return stream;
}

void Dude::deserialize_genome(unsigned char * stream, unsigned int * index) {


	buffer_to_int(stream, &mutation_day, index);

	unsigned int attribute_stream_length = (INTERNALLY_TRADED_ATTRIBUTES * (INTERNALLY_TRADED_ATTRIBUTES-1)) / 2;
	
	copy_buffer(stream, internally_traded_attribute_buffer, attribute_stream_length, index);
	parse_attribute_stream();

	buffer_to_int(stream, &regular_energy_ratio, index);
	buffer_to_int(stream, &fight_energy_ratio, index);
	buffer_to_int(stream, &reproductive_energy_ratio, index);
	buffer_to_int(stream, &mutation_chance, index);
	buffer_to_int(stream, &traded_attribute_mutation_chance, index);
	buffer_to_int(stream, &other_attribute_mutation_chance, index);

	//mutation_chance = 1024;
	//traded_attribute_mutation_chance = 1024;
	//other_attribute_mutation_chance = 1024;
	
	bound_params();
	
	neural_network->deserialize_genome(stream, index);
	
	//printf("Dude::deserialize_genome %d\r\n", *index);
}

unsigned char * Dude::serialize_state(unsigned char * stream, unsigned int * length) {
	
	unsigned int this_genome_length = 0;
	unsigned char * genome = serialize_genome(0, &this_genome_length);
	unsigned char genome_hash[SHA_DIGEST_LENGTH];
	SHA1(genome, this_genome_length, genome_hash);
	free(genome);

	stream = add_buffer_to_stream(stream, length, (unsigned char *)genome_hash, SHA_DIGEST_LENGTH);
	
	stream = add_int_to_stream(stream, length, &x);
	stream = add_int_to_stream(stream, length, &y);
	stream = add_char_to_stream(stream, length, (char)orientation);
	stream = add_int_to_stream(stream, length, &regular_energy_level);
	stream = add_int_to_stream(stream, length, &fight_energy_level);
	stream = add_int_to_stream(stream, length, &reproductive_energy_level);
	
	stream = neural_network->serialize_state(stream, length);
	
	//printf("Dude::serialize_state %d\r\n", *length);
	
	return stream;
}

void Dude::deserialize_state(unsigned char * stream, unsigned int * index, hashMap * genomes) {

	//printf("Dude::deserialize_state start %d\r\n", *index);

	copy_buffer(stream, (unsigned char *)genome_hash, SHA_DIGEST_LENGTH, index);

	unsigned char * genome = (unsigned char *)hmget(genomes, genome_hash);

	if(!genome) {
		printf("Dude::deserialize_state ERROR reding genome\n");
		exit(0);
	}

	unsigned int temp_index = 0;
	deserialize_genome(genome, &temp_index);

	buffer_to_int(stream, &x, index);
	buffer_to_int(stream, &y, index);
	buffer_to_char(stream, (unsigned char*)&orientation, index);
	buffer_to_int(stream, &regular_energy_level, index);
	buffer_to_int(stream, &fight_energy_level, index);
	buffer_to_int(stream, &reproductive_energy_level, index);
	
	neural_network->deserialize_state(stream, index);
	
	//printf("Dude::deserialize_state end %d\r\n", *index);
}

void Dude::eat(unsigned int energy) {
	
	unsigned int total_energy_ratio = reproductive_energy_ratio + fight_energy_ratio + regular_energy_ratio;
	unsigned int new_reproductive_energy = energy * reproductive_energy_ratio / total_energy_ratio;
	unsigned int new_fight_energy = energy * fight_energy_ratio / total_energy_ratio;
	unsigned int new_regular_energy = energy - (new_reproductive_energy + new_fight_energy);
	
	regular_energy_level += new_regular_energy;
	if(regular_energy_level > regular_energy_max) {
		regular_energy_level = regular_energy_max;
	}
	
	fight_energy_level += new_fight_energy;
	if(fight_energy_level > fight_energy_max) {
		fight_energy_level = fight_energy_max;
	}
	
	reproductive_energy_level += new_reproductive_energy;
	while(reproductive_energy_level > reproductive_energy_max) {
		reproductive_energy_level -= reproductive_energy_max;
		reproduce();
	}
	
}

unsigned int Dude::fight(Dude * enemy) {
	
	//printf("FIGHT! FIGHT! FIGHT! at (%d,%d)", enemy->x, enemy->y);
	
	if(this->fight_energy_level < enemy->fight_energy_level) {
		enemy->fight_energy_level -= this->fight_energy_level;
		this->fight_energy_level = 0;
		enemy->consume(this);
		//printf(" defender wins\r\n");
		return false;
	} else {
		this->fight_energy_level -= enemy->fight_energy_level;
		enemy->fight_energy_level = 0;
		this->consume(enemy);
		//printf(" attacker wins\r\n");
		return true;
	}
}
void Dude::consume(Dude * defeated) {
	regular_energy_level += defeated->regular_energy_level;
	defeated->regular_energy_level = 0;
	
	reproductive_energy_level += defeated->reproductive_energy_level;
	defeated->reproductive_energy_level = 0;
	
	defeated->dead = true;
	
	if(regular_energy_level > regular_energy_max) {
		regular_energy_level = regular_energy_max;
	}
	
	while(reproductive_energy_level > reproductive_energy_max) {
		reproductive_energy_level -= reproductive_energy_max;
		reproduce();
	}
}

void Dude::act() {
	if(regular_energy_level > live_cost) {
		regular_energy_level -= live_cost;
	} else {
		dead = true;
		fight_energy_level = 0;
		reproductive_energy_level = 0;
		regular_energy_level = 0;
		//printf("STARVE TO DEATH\r\n");
		return;
	}
	
	if(regular_energy_level > move_cost) {
		
		regular_energy_level -= move_cost;
		turn action = neural_network->get_turn();
		field->move(this, action);
	} else {
		field->move(this, NO_TURN);
	}
}

void Dude::reproduce() {
	//printf("REPRODUCE\r\n");
	
	Dude * offspring = new Dude(field);

	unsigned int this_genome_length = 0;
	unsigned char * genome = serialize_genome(0, &this_genome_length);
	unsigned int temp_index = 0;
	offspring->deserialize_genome(genome, &temp_index);
	free(genome);
	
	offspring->regular_energy_level = this->regular_energy_level/2;
	this->regular_energy_level -= offspring->regular_energy_level;
	
	offspring->fight_energy_level = this->fight_energy_level/2;
	this->fight_energy_level -= offspring->fight_energy_level;
	
	offspring->reproductive_energy_level = 0;
	
	//offspring->orientation = orientation;
	
	field->add_dude(offspring, this->x, this->y);
	
	unsigned int r = SHR128() % mutation_chance;
	while(r == 0) {
		r = (SHR128() % mutation_chance);
		if(!offspring->mutate()) {
			r = 0;
		}
		offspring->mutation_day = field->day;
	}
}

void Dude::bound_params() {
	while(traded_attribute_mutation_chance > 0x3FF ||
			other_attribute_mutation_chance > 0x3FF ||
			neaural_network_mutation_chance > 0x3FF) {
		traded_attribute_mutation_chance >>= 1;
		other_attribute_mutation_chance >>= 1;
		neaural_network_mutation_chance >>= 1;
	}
	while(traded_attribute_mutation_chance == 0 ||
			other_attribute_mutation_chance == 0 ||
			neaural_network_mutation_chance == 0) {
		traded_attribute_mutation_chance++;
		other_attribute_mutation_chance++;
		neaural_network_mutation_chance++;
	}
	if(mutation_chance < 2) {
		mutation_chance = 2;
	}
	if(mutation_chance > 0x3FF) {
		mutation_chance = 0x3FF;
	}
	while(regular_energy_ratio > 0x3FF ||
			fight_energy_ratio > 0x3FF ||
			reproductive_energy_ratio > 0x3FF) {
		regular_energy_ratio >>= 1;
		fight_energy_ratio >>= 1;
		reproductive_energy_ratio >>= 1;
	}
	while(regular_energy_ratio == 0 ||
			fight_energy_ratio == 0 ||
			reproductive_energy_ratio == 0) {
		regular_energy_ratio++;
		fight_energy_ratio++;
		reproductive_energy_ratio++;
	}
}

unsigned int Dude::mutate() {
	
	printf("MUTATE\r\n");
	unsigned int changed = 0;
	
	unsigned int total = traded_attribute_mutation_chance + other_attribute_mutation_chance + neaural_network_mutation_chance;
	unsigned int r = SHR128() % total;
	
	if(r < traded_attribute_mutation_chance) {
		
		r = SHR128() % (INTERNALLY_TRADED_ATTRIBUTES * (INTERNALLY_TRADED_ATTRIBUTES-1)) / 2;
		unsigned char * mutation_target = &internally_traded_attribute_buffer[r];
		changed = mutate_char(mutation_target);
		parse_attribute_stream();
		
		printf("mutate attribute\r\n");
		
	} else if(r < traded_attribute_mutation_chance + other_attribute_mutation_chance) {
		unsigned int number_of_attributes = 6;
		unsigned int * mutation_target = 0;
		r = SHR128() % (number_of_attributes);
		
		printf("mutate other attribute\r\n");
		
		switch(r) {
			case 0:
				mutation_target = &regular_energy_ratio;
				break;
			case 1:
				mutation_target = &fight_energy_ratio;
				break;
			case 2:
				mutation_target = &reproductive_energy_ratio;
				break;
			case 3:
				mutation_target = &mutation_chance;
				break;
			case 4:
				mutation_target = &traded_attribute_mutation_chance;
				break; 
			case 5:
				mutation_target = &other_attribute_mutation_chance;
				break;
		}
		changed = mutate_int(mutation_target);
	} else {
		
		printf("mutate nn\r\n");
		
		changed = neural_network->mutate();
	}
	bound_params();
	
	printf("MUTATED %d\r\n", changed);
	
	return changed;
}
