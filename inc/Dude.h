#ifndef dude_h_
#define dude_h_

#include <openssl/sha.h>

#include "Field.h"
#include "direction.h"
#include "hashMap.h"

#define INTERNALLY_TRADED_ATTRIBUTES 5
#define GENOME_FILENAME_LENGTH	SHA_DIGEST_LENGTH*2

class Field;
class NeuralNetwork;

class Dude {
	public:
	
	// state data
	unsigned int x;
	unsigned int y;
	unsigned int regular_energy_level;
	unsigned int fight_energy_level;
	unsigned int reproductive_energy_level;
	direction orientation;
	unsigned int dead;
	
	char genome_hash[SHA_DIGEST_LENGTH];
	char parent_genome_hash[SHA_DIGEST_LENGTH];
	unsigned int mutation_day;
	
	unsigned char internally_traded_attribute_buffer[(INTERNALLY_TRADED_ATTRIBUTES * (INTERNALLY_TRADED_ATTRIBUTES-1)) / 2];
	
	// internally traded attributes
	unsigned int regular_energy_max;
	unsigned int fight_energy_max;
	unsigned int reproductive_energy_max;
	unsigned int live_cost;
	unsigned int move_cost; 
	
	// other attributes
	unsigned int regular_energy_ratio;
	unsigned int fight_energy_ratio;
	unsigned int reproductive_energy_ratio;
	unsigned int mutation_chance;
	unsigned int traded_attribute_mutation_chance;
	unsigned int other_attribute_mutation_chance;
	unsigned int neaural_network_mutation_chance;
	
	NeuralNetwork * neural_network;
	Field * field;
	
	Dude(Field * field);
	~Dude();
	
	void to_string(char * string);
	
	void save_genome();
	void load_genome(char * filename);
	
	void parse_attribute_stream();
	void deserialize_genome(unsigned char * stream, unsigned int * length);
	unsigned char * serialize_genome(unsigned char * stream, unsigned int * length);
	void deserialize_state(unsigned char * stream, unsigned int * length, hashMap * genomes);
	unsigned char * serialize_state(unsigned char * stream, unsigned int * length);
	
	void eat(unsigned int energy);
	unsigned int fight(Dude * enemy);
	void consume(Dude * defeated);
	void act();
	void reproduce();
	unsigned int mutate();
	void bound_params();

};


#endif