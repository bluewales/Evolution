
void int_to_buffer(unsigned int * i, unsigned char * buffer);
void double_to_buffer(double * d, unsigned char * buffer);
void buffer_to_int(unsigned char * buffer, unsigned int * i, unsigned int * index);
void buffer_to_double(unsigned char * buffer, double * d, unsigned int * index);
void buffer_to_char(unsigned char * buffer, unsigned char * character, unsigned int * index);
void copy_buffer(unsigned char * buffer, unsigned char * destination, unsigned int length, unsigned int * index);
unsigned char * add_char_to_stream(unsigned char * stream, unsigned int * length, unsigned char new_char);
unsigned char * add_buffer_to_stream(unsigned char * stream, unsigned int * length, unsigned char * buffer, unsigned int buffer_length);
unsigned char * add_int_to_stream(unsigned char * stream, unsigned int * length, unsigned int * i);
unsigned char * add_double_to_stream(unsigned char * stream, unsigned int * length, double * d);
char * hex_encode_buffer(unsigned char * buffer, unsigned int length);
