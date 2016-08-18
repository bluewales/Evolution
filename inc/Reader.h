#ifndef reader_h_
#define reader_h_

#include <stdio.h>

class Reader {
	public:
	
	FILE * fd;
	char mode[8];
	
	Reader(char * filename, char * mode);
	~Reader();
	
	void read(unsigned char * buffer, unsigned int n);

	void readfile(unsigned char * buffer);
	void write(unsigned char * buffer, unsigned int n);

};


#endif