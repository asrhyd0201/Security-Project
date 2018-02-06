
#include<stdlib.h>

typedef unsigned int (*hashfunc_t)(const char *);
typedef struct {
	size_t asize;
	unsigned char *a;
	size_t nfuncs;
	hashfunc_t *funcs;
} BLOOM;

BLOOM *bloom_create(size_t size, size_t nfuncs, ...);
int bloom_destroy(BLOOM *bloom);
int bloom_add(BLOOM *bloom, const char *s);
int bloom_check(BLOOM *bloom, const char *s);
unsigned int BKDRHash(char* str);
unsigned int DJBHash(char* str);
unsigned int sdbm_hash(const char *key);
unsigned int sax_hash(const char *key);
unsigned int RSHash(char* str, unsigned int len);
unsigned int JSHash(char* str, unsigned int len);
unsigned int PJWHash(char* str, unsigned int len);
unsigned int ELFHash(char* str, unsigned int len);
void bloom_display(BLOOM *bloom);

