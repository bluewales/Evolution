#include "stdlib.h"
#include "string.h"
#include "stdio.h"


void int_to_buffer(unsigned int * i, unsigned char * buffer) {
	memcpy(buffer, i, sizeof(unsigned int));
}

void double_to_buffer(double * d, unsigned char * buffer) {
	memcpy(buffer, d, sizeof(double));
}

void buffer_to_int(unsigned char * buffer, unsigned int * i, unsigned int * index) {
	memcpy(i, &buffer[*index], sizeof(unsigned int));
	*index += sizeof(unsigned int);
}

void buffer_to_double(unsigned char * buffer, double * d, unsigned int * index) {
	memcpy(d, &buffer[*index], sizeof(double));
	*index += sizeof(double);
}

void buffer_to_char(unsigned char * buffer, unsigned char * character, unsigned int * index) {
	*character = buffer[*index];
	*index += 1;
}

void copy_buffer(unsigned char * buffer, unsigned char * destination, unsigned int length, unsigned int * index) {
	memcpy(destination, &buffer[*index], length);
	*index += length;
}

unsigned char * add_char_to_stream(unsigned char * stream, unsigned int * length, unsigned char new_char) {
	stream = (unsigned char *)realloc(stream, (*length)+1);
	stream[*length] = new_char;
	(*length)++;
	return stream;
}

unsigned char * add_buffer_to_stream(unsigned char * stream, unsigned int * length, unsigned char * buffer, unsigned int buffer_length) {
	stream = (unsigned char *)realloc(stream, (*length)+buffer_length);
	memcpy(&stream[*length], buffer, buffer_length);
	(*length) += buffer_length;
	return stream;
}

unsigned char * add_int_to_stream(unsigned char * stream, unsigned int * length, unsigned int * i) {
	unsigned char n_buf[sizeof(unsigned int)];
	int_to_buffer(i, n_buf);
	return add_buffer_to_stream(stream, length, n_buf, sizeof(unsigned int));
}

unsigned char * add_double_to_stream(unsigned char * stream, unsigned int * length, double * d) {
	unsigned char n_buf[sizeof(double)];
	double_to_buffer(d, n_buf);
	stream = add_buffer_to_stream(stream, length, n_buf, sizeof(double));
	return stream;
}
