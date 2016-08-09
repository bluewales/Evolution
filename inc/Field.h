#ifndef field_h_
#define field_h_

#include "Dude.h"
#include "direction.h"

#define AREA_WIDTH 256

class Dude;

class Field {
	public:
	
	char filename[64];
	
	unsigned int width;
	unsigned int height;
	unsigned int historic_count;
	unsigned int day;
	unsigned char ** vegatation;
	Dude *** location_of_dudes;
	
	Dude ** dudes;
	unsigned int dude_population;
	
	Field(char * filename);
	~Field();
	
	void create_empty_field();
	
	void advance_state();
	void load();
	void save();
	void move(Dude * dude, turn t);
	double smell_vegetation(Dude * dude, turn t);
	double smell_move_energy(Dude * dude, turn t);
	double smell_fight_energy(Dude * dude, turn t);
	double smell_reproductive_energy(Dude * dude, turn t);
	void add_dude(Dude * new_dude, unsigned int x, unsigned int y);
	unsigned int get_historic_count();
};


#endif