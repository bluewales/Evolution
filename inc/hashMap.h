#ifndef hashMap_h_
#define hashMap_h_

typedef struct {
	char * key;
	void * value;
} hashNode;

typedef struct {
	unsigned int size;
	unsigned int count;
	hashNode * map;
} hashMap;

hashMap * hmcreate();
void hmdestroy(hashMap * v);
int hmset(hashMap * v, char * key, void * value);
void * hmget(hashMap * v, char * key);

#endif
