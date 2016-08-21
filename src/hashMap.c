#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hashMap.h"

#define START_SIZE (1024)
#define NEW_BIGGER_SIZE(old_size) (old_size*2)

#define INDEX_RANGE(index, size) (index & ((size)-1))
#define INCREMENT_INDEX(index,i,dhash) (index+dhash+i)

#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}


hashMap * hmcreate() {
	unsigned int size = START_SIZE;
	hashMap * h = (hashMap *)malloc(sizeof(hashMap));
	h->map = (hashNode *)malloc(size * sizeof(hashNode));
	memset(h->map, 0, (size) * sizeof(hashNode));
	h->size = size;
	h->count = 0;
	return h;
}

void hmdestroy(hashMap * h) {
	if(h) {
		if(h->map) {
			unsigned int i;
			for(i = 0; i < h->size; i++) {
				if(h->map[i].key)
					free(h->map[i].key);
			}
			free(h->map);
		}
		free(h);
	}
}

inline unsigned int doubleHash(char * key) {
	unsigned int hash = 0;
	unsigned int c;
	while ((c = *key++))
		hash = c + (hash << 6) + (hash << 16) - hash;
	return hash;
}

inline unsigned int stringHash(char * key) {
	unsigned int hash = 5381;
	unsigned int c;
	while ((c = *key++))
		hash = ((hash << 5) + hash) ^ c;
	return hash;
}


void hashlittle2(const void *key, unsigned int *pc, unsigned int *pb) {
	unsigned int a,b,c;
	union { const void *ptr; size_t i; } u;

	unsigned int length = strlen((char*)key);

	/* Set up the internal state */
	a = b = c = length + 0x47fd56ba; 
	c += 0x987e5677;

	u.ptr = key;
	if ((u.i & 0x3) == 0) {
		const unsigned int *k = (const unsigned int *)key;         /* read 32-bit chunks */
		

		/*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
		while (length > 12)
		{
			a += k[0];
			b += k[1];
			c += k[2];
			mix(a,b,c);
			length -= 12;
			k += 3;
		}

/*----------------------------- handle the last (probably partial) block */
/* 
 * "k[2]&0xffffff" actually reads beyond the end of the string, but
 * then masks off the part it's not allowed to read.  Because the
 * string is aligned, the masked-off tail is in the same word as the
 * rest of the string.  Every machine with memory protection I've seen
 * does it on word boundaries, so is OK with this.  But VALGRIND will
 * still catch it and complain.  The masking trick does make the hash
 * noticably faster for short strings (like English words).
 */
#ifndef VALGRIND

		switch(length)
		{
		case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
		case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
		case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
		case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
		case 8 : b+=k[1]; a+=k[0]; break;
		case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
		case 6 : b+=k[1]&0xffff; a+=k[0]; break;
		case 5 : b+=k[1]&0xff; a+=k[0]; break;
		case 4 : a+=k[0]; break;
		case 3 : a+=k[0]&0xffffff; break;
		case 2 : a+=k[0]&0xffff; break;
		case 1 : a+=k[0]&0xff; break;
		case 0 : *pc=c; *pb=b; return;  /* zero length strings require no mixing */
		}

#else /* make valgrind happy */
		const unsigned char *k8;
		k8 = (const uint8_t *)k;
		switch(length)
		{
		case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
		case 11: c+=((unsigned int)k8[10])<<16;  /* fall through */
		case 10: c+=((unsigned int)k8[9])<<8;    /* fall through */
		case 9 : c+=k8[8];                   /* fall through */
		case 8 : b+=k[1]; a+=k[0]; break;
		case 7 : b+=((unsigned int)k8[6])<<16;   /* fall through */
		case 6 : b+=((unsigned int)k8[5])<<8;    /* fall through */
		case 5 : b+=k8[4];                   /* fall through */
		case 4 : a+=k[0]; break;
		case 3 : a+=((unsigned int)k8[2])<<16;   /* fall through */
		case 2 : a+=((unsigned int)k8[1])<<8;    /* fall through */
		case 1 : a+=k8[0]; break;
		case 0 : *pc=c; *pb=b; return;  /* zero length strings require no mixing */
		}

#endif /* !valgrind */

	} else {
		const unsigned short *k = (const unsigned short *)key;         /* read 16-bit chunks */
		const unsigned char *k8;

		/*--------------- all but last block: aligned reads and different mixing */
		while (length > 12)
		{
			a += k[0] + (((unsigned int)k[1])<<16);
			b += k[2] + (((unsigned int)k[3])<<16);
			c += k[4] + (((unsigned int)k[5])<<16);
			mix(a,b,c);
			length -= 12;
			k += 6;
		}

		/*----------------------------- handle the last (probably partial) block */
		k8 = (const unsigned char *)k;
		switch(length)
		{
		case 12: c+=k[4]+(((unsigned int)k[5])<<16);
		         b+=k[2]+(((unsigned int)k[3])<<16);
		         a+=k[0]+(((unsigned int)k[1])<<16);
		         break;
		case 11: c+=((unsigned int)k8[10])<<16;     /* fall through */
		case 10: c+=k[4];
		         b+=k[2]+(((unsigned int)k[3])<<16);
		         a+=k[0]+(((unsigned int)k[1])<<16);
		         break;
		case 9 : c+=k8[8];                      /* fall through */
		case 8 : b+=k[2]+(((unsigned int)k[3])<<16);
		         a+=k[0]+(((unsigned int)k[1])<<16);
		         break;
		case 7 : b+=((unsigned int)k8[6])<<16;      /* fall through */
		case 6 : b+=k[2];
		         a+=k[0]+(((unsigned int)k[1])<<16);
		         break;
		case 5 : b+=k8[4];                      /* fall through */
		case 4 : a+=k[0]+(((unsigned int)k[1])<<16);
		         break;
		case 3 : a+=((unsigned int)k8[2])<<16;      /* fall through */
		case 2 : a+=k[0];
		         break;
		case 1 : a+=k8[0];
		         break;
		case 0 : *pc=c; *pb=b; return;  /* zero length strings require no mixing */
		}

	} 

	final(a,b,c);
	*pc=c; *pb=b;
}



int insert(hashNode * map, int size, char * key, void * value) {
	unsigned int hash;
	unsigned int dhash;
	hashlittle2(key, &hash, &dhash);
	int index = INDEX_RANGE(hash, size);
	int i = 0;
	while(map[index].key) {
		if(strcmp(map[index].key, key) == 0) {
			free(map[index].key);
			map[index].key = key;
			map[index].value = value;
			printf("Same key! %s\n", key);
			return 0;
		}
		i++;
		index = INDEX_RANGE(INCREMENT_INDEX(index,i,dhash), size );
	}

	map[index].key = key;
	map[index].value = value;
	return 1;
}

void rehash(hashMap * h, int newSize) {
	unsigned int i;
	printf("  rehashing to %d\n", newSize);
	hashNode * newMap = (hashNode *)malloc(newSize * sizeof(hashNode));
	memset(newMap, 0, newSize * sizeof(hashNode));
	for(i = 0; i < h->size; i++) {
		if(h->map[i].key) {
			insert(newMap, newSize, h->map[i].key, h->map[i].value);
		}
	}
	free(h->map);
	h->map = newMap;
	h->size = newSize;
}

int hmset(hashMap * h, char * key, void * value) {
	if(h->size/2 < h->count + 1) {
		int new_size = NEW_BIGGER_SIZE(h->size);
		rehash(h, new_size);
	}
	char * new_key = (char *)malloc((1 + strlen(key)) * sizeof(char));
	strcpy(new_key, key);

	h->count += insert(h->map, h->size, new_key, value);
	return 1;
}
void * hmget(hashMap * h, char * key) {
	unsigned int hash;
	unsigned int dhash;
	hashlittle2(key, &hash, &dhash);
	int index = INDEX_RANGE(hash, h->size);
	int i = 0;

	if(h->map[index].key == 0)
		return 0;

	while(strcmp(key, h->map[index].key)) {
		
		if(h->map[index].key == 0)
			return 0;
		i++;
		index = INDEX_RANGE(INCREMENT_INDEX(index,i,dhash), h->size );
	}
	return h->map[index].value;
}

/*
#define N 200000
#define TESTS 100
int main() {
	int i, j;
	printf("Testing hashMap\n");

	int start_time = time(0);
	
	char keys[N][30];
	int numbers[N];
	srand(N);
	for(j = N/TESTS; j <= N; j+=N/TESTS) {
		printf("TEST %d\n", j);

		hashMap * h = hmcreate();
		

		for(i = 0; i < j; i++) {
			sprintf(keys[i],"%08xX%X",rand(),i);
		}

		printf("TEST %d - ZEROS\n", j);

		for(i = 0; i < j; i++) {
			if((int)hmget(h,keys[i])) {
				printf("FAIL read non zero from emtpy map i:%d, key:%s\n", i , keys[i]);
			}
		}

		printf("TEST %d - WRITES\n", j);

		for(i = 0; i < j; i++) {

			numbers[i] = rand();


			hmset(h, keys[i],     (void*)numbers[i]);
			hmset(h, &keys[i][1], (void*)numbers[i]);
			hmset(h, &keys[i][2], (void*)numbers[i]);
			hmset(h, &keys[i][3], (void*)numbers[i]);
		}

		printf("TEST %d - READS\n", j);

			
		for(i = 0; i < j; i++) {
			

			char key[30];

			strcpy(&key[3], keys[i]);
			int number = (int)hmget(h,&key[3]);
			if(number != numbers[i])
				printf("%d != %d i=%d\n", number, numbers[i], i);

			strcpy(&key[2], &keys[i][1]);
			number = (int)hmget(h,&key[2]);
			if(number != numbers[i])
				printf("%d != %d i=%d\n", number, numbers[i], i);

			strcpy(&key[1], &keys[i][2]);
			number = (int)hmget(h,&key[1]);
			if(number != numbers[i])
				printf("%d != %d i=%d\n", number, numbers[i], i);

			strcpy(key, &keys[i][3]);
			number = (int)hmget(h,key);
			if(number != numbers[i])
				printf("%d != %d i=%d\n", number, numbers[i], i);
		}
		hmdestroy(h);
	}

	int end_time = time(0);

	int total_time = end_time - start_time;

	printf("Total time: %d\n", total_time);
}
*/