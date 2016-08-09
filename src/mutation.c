#include <stdio.h>

#include "mutation.h"
#include "fastRandom.h"



unsigned int mutate_int(unsigned int * i) {
	unsigned int changed = 0;
	unsigned int delta = (((*i)>>6)?(1 + (SHR128() % ((*i)>>6))):1);
	unsigned int r = SHR128() & 0x1;
	
	if(r) {
		unsigned int new_mutation_value = (*i) + delta;
		if(new_mutation_value > *i) {
			*i = new_mutation_value;
			changed = 1;
		}
	} else {
		unsigned int new_mutation_value = (*i) - delta;
		if(new_mutation_value < *i) {
			*i = new_mutation_value;
			changed = 1;
		}
	}
	
	return changed;
}

unsigned int mutate_char(unsigned char * c) {
	unsigned int changed = 0;
	unsigned int r = SHR128() & 0x1;
	unsigned int delta = 1;
	if(r) {
		unsigned char new_mutation_value = (*c) - delta;
		if(new_mutation_value < *c) {
			*c = new_mutation_value;
			changed = 1;
		}
	} else {
		unsigned char new_mutation_value = (*c) + delta;
		if(new_mutation_value > *c) {
			*c = new_mutation_value;
			changed = 1;
		}
	}
	return changed;
}

unsigned int mutate_double(double * d) {
	unsigned int changed = 0;
	double delta = VNI() / 64;
	unsigned int r = SHR128() % 11;
	
	if(r < 10) {
		double new_mutation_value = (*d) + delta;
		if(new_mutation_value < -1) new_mutation_value = -1;
		if(new_mutation_value > 1) new_mutation_value = 1;
		if(new_mutation_value != *d) {
			*d = new_mutation_value;
			changed = 1;
		}
	} else {
		double new_mutation_value = VNI();
		if(new_mutation_value != *d) {
			*d = new_mutation_value;
			changed = 1;
		}
	}
	return changed;
}
