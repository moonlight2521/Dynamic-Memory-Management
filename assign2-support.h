#ifndef CMSC257_A2SUPPORT_INCLUDED
#define CMSC257_A2SUPPORT_INCLUDED


////////////////////////////////////////////////////////////////////////////////
//
//  File          : assign1-support.h
//  Description   : This is a set of general-purpose utility functions we use
// Functional Prototypes


void *nofree_malloc(size_t size);

void *malloc(size_t size);

void *calloc(size_t nelem, size_t elsize);

void free(void *ptr);

void *realloc(void *ptr, size_t size);

void split_block(struct block_meta *b,size_t s);

struct block_meta *merge(struct block_meta *b);

int memory_leaks();

void print_list();
  
void print_all();

void printAddress();

#endif // CMSC257_A2SUPPORT_INCLUDED
