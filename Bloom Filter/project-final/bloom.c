/* Bloom Filter Code */
#include<limits.h>
#include<stdarg.h>
#include<stdio.h>

#include"bloom.h"

#define SETBIT(a, n) (a[n/8] |= (1<<(n%8)))
#define GETBIT(a, n) (a[n/8] & (1<<(n%8)))

/*Hash Functions*/
unsigned int sax_hash(const char *key)
{
	unsigned int h=0;

	while(*key) h^=(h<<5)+(h>>2)+(unsigned char)*key++;

	return h;
}

unsigned int sdbm_hash(const char *key)
{
	unsigned int h=0;
	while(*key) h=(unsigned char)*key++ + (h<<6) + (h<<16) - h;
	return h;
}

unsigned int DJBHash(char* str)
{
   unsigned int hash = 5381;
   unsigned int i    = 0;

   for(i = 0; (*str)!='\0'; str++, i++)
   {
      hash = ((hash << 5) + hash) + (*str);
   }

   return hash;
}

unsigned int BKDRHash(char* str)
{
   unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; (*str)!='\0'; str++, i++)
   {
      hash = (hash * seed) + (*str);
   }

   return hash;
}

/*Function to create a bloom filter*/
BLOOM *bloom_create(size_t size, size_t nfuncs, ...)
{
	BLOOM *bloom;
	va_list l;
	int n;
	
	if(!(bloom=malloc(sizeof(BLOOM)))) return NULL;
	if(!(bloom->a=calloc((size/8), sizeof(char)))) 
	{
		free(bloom);
		return NULL;
	}
	if(!(bloom->funcs=(hashfunc_t*)malloc(nfuncs*sizeof(hashfunc_t)))) 
	{
		free(bloom->a);
		free(bloom);
		return NULL;
	}

	va_start(l, nfuncs);
	for(n=0; n<nfuncs; ++n) 
	{
		bloom->funcs[n]=va_arg(l, hashfunc_t);
	}
	va_end(l);

	bloom->nfuncs=nfuncs;
	bloom->asize=size;

	return bloom;
}
/*Function to delete bloom filter*/
int bloom_destroy(BLOOM *bloom)
{
	free(bloom->a);
	free(bloom->funcs);
	free(bloom);

	return 0;
}

/*Function to insert a string into bloom filter*/
int bloom_add(BLOOM *bloom, const char *s)
{
	size_t n;
        printf("Bits set\n");
	for(n=0; n<bloom->nfuncs; ++n) {
		SETBIT(bloom->a, bloom->funcs[n](s)%bloom->asize);
                printf("%zd ",bloom->funcs[n](s)%bloom->asize);
	}
	//printf("\n");
	return 0;
}

/* Function to check whether a string is present or not*/
int bloom_check(BLOOM *bloom, const char *s)
{
	size_t n;

	for(n=0; n<bloom->nfuncs; ++n) {
		if(!(GETBIT(bloom->a, bloom->funcs[n](s)%bloom->asize))) return 0;
	}

	return 1;
}

/*Function to display the conetents of Blomm Filters*/
void bloom_display(BLOOM *bloom)
{
	int i,j;
	printf("Bloom Filter contents");
	for(i=0;i<bloom->asize/8;i++)
	{
                //printf("\nBloom%d: ",i);
		for(j=0;j<8;j++)
		{
			if(bloom->a[i] & (1<<j))
				printf("1");
			else
				printf("0");
		}
	}
	printf("\n");
}
