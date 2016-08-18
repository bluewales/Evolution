#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Reader.h"
#include "fastRandom.h"

Reader::Reader(char * filename, char * mode) {
	strcpy(this->mode, mode);
	fd = fopen(filename, this->mode);
}

Reader::~Reader() {
	fclose(fd);
}


unsigned int Reader::filesize() {
	fseek(fd, 0, SEEK_END);
	unsigned int n = fsize = ftell(fd);
	fseek(f, 0, SEEK_SET);
	return n;
}
	
void Reader::read(unsigned char * buffer, unsigned int n) {
	unsigned int bytes_read = fread(buffer, 1, n, fd);
	unsigned int index;
	for(index = bytes_read; index < n; index++) {
		buffer[index] = SHR128();
	}
}

void Reader::readfile(unsigned char * buffer) {
	unsigned int n = filesize();
	read(buffer, n);
}

void Reader::write(unsigned char * buffer, unsigned int n) {
	fwrite(buffer, 1, n, fd);
}
