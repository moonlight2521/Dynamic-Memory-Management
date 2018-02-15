
// Include Files
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
// Don't include stdlb since the names will conflict?
//#include <assign2-support.h>
// alignment:
#define align8(x) (((((x) -1) >>2) <<2)+8);
// sbrk some extra space every time we need it.
// This does no bookkeeping and therefore has no ability to free, realloc, etc.

void *global_base = NULL;
typedef struct block_meta *t_block;

#define META_SIZE sizeof(struct block_meta)

void *nofree_malloc(size_t size) {
  void *p = sbrk(0);
  void *request = sbrk(size);
  if (request == (void*) -1) { 
    return NULL; // sbrk failed
  } else {
    assert(p == request); // Not thread safe.
    return p;
  }
}
//block struck
struct block_meta {
  size_t size;
  struct block_meta *next;
  struct block_mata *prev;
  int free;
  void *ptr;    // a pointer to the allocated block 
  char data[1];
  int magic;    // For debugging only. TODO: remove this in non-debug mode.
};



// Iterate through blocks until we find one that's large enough.
// TODO: split block up if it's larger than necessary
void split_block(struct block_meta *b,size_t s){
  struct block_meta *new;
  new = (t_block)(b->data + s);
  new->size = b->size - s - META_SIZE;
  new->next = b->next;
  new->prev = b;
  new->free = 1;
  new->ptr = new->data;
  b->size = s;
  b->next = new;
  if(new->next)
	new->next->prev = new;
}


struct block_meta *find_free_block(struct block_meta **last, size_t size) {
  struct block_meta *current = global_base;
  struct block_meta *b;
  while (current && !(current->free && current->size >= size)) {
    *last = current;
    current = current->next;
  }
  //add best filt algorithm
  b = current;
  while(current){
    if(current->free && current->size < b->size && b->size >= size){
      b=current;
    }
    current = current->next;
  }
  return b;
}

struct block_meta *request_space(struct block_meta* last, size_t size) {
  struct block_meta *block;
  block = sbrk(0);
  void *request = sbrk(size + META_SIZE);
  assert((void*)block == request); // Not thread safe.
  if (request == (void*) -1) {
    return NULL; // sbrk failed.
  }
  
  if (last) { // NULL on first request.
    last->next = block;
  }
  block->size = size;
  block->next = NULL;
  block->free = 0;
  block->magic = 0x12345678;
  return block;
}
// If it's the first ever call, i.e., global_base == NULL, request_space and set global_base.
// Otherwise, if we can find a free block, use it.
// If not, request_space.
void *malloc(size_t size) {
  struct block_meta *block;
  size_t s;
  s = align8(size);

  if (size <= 0) {
    return NULL;
  }

  if (!global_base) { // First call.
    block = request_space(NULL, size);
    if (!block) {
      return NULL;
    }
    global_base = block;
  } else {
    struct block_meta *last = global_base;
    block = find_free_block(&last, size);
    if (!block) { // Failed to find free block.
      block = request_space(last, size);
      if (!block) {
    return NULL;
      }
    } else {      // Found free block
      // TODO: consider splitting block here.
      split_block(block, s);
      block->free = 0;
      block->magic = 0x77777777;
      merge_two();
    }
  }
  
  return(block+1);
}

void *calloc(size_t nelem, size_t elsize) {
  size_t size = nelem * elsize;
  void *ptr = malloc(size);
  memset(ptr, 0, size);
  return ptr;
}

// TODO: maybe do some validation here.
struct block_meta *get_block_ptr(void *ptr) {
  return (struct block_meta*)ptr - 1;
}

int get_addr(void *ptr){
  if(global_base){
    if(ptr > global_base && ptr < sbrk(0)){
      return(ptr == (get_block_ptr(ptr))-> ptr);
    }
  }
return(0);
}

void free(void *ptr) {
  if (!ptr) {
    return;
  }

  // TODO: consider merging blocks once splitting blocks is implemented.
   struct block_meta* block_ptr = get_block_ptr(ptr);
   assert(block_ptr->free == 0);
   assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
   block_ptr->free = 1;
   block_ptr->magic = 0x55555555;  
   merge_two();   
}


//struct block_meta *merge(struct block_meta *b){
//  if (b->next && b->next->free){
//    b->size += META_SIZE + b->next->size;
//    b->next  = b->next->next;
//        if(b->next){
//          b->next->prev = b;
//   }
// }
//  return(b);
//}


// check if the block can be merge.
int check_merge(){
  struct block_meta *b = global_base;
  int more = 0;
  while(b){
    if(b->next && b->free == 1 && b->next->free == 1){
      b->size = b->size + META_SIZE + b->next->size;
      b->next = b->next->next;
      more = 1; 
    }
    b = b-> next;
  }
  return more;
}

//merge the two
void merge_two(){
  int more = 1;
  while(more){
    more = check_merge();
  }
}


void *realloc(void *ptr, size_t size) {
  size_t s;
  s = align8(size);
  struct block_meta *b, new;
  void *newptr; 
  if (!ptr) { 
    // NULL ptr. realloc should act like malloc.
    return malloc(size);
  }

  struct block_meta* block_ptr = get_block_ptr(ptr);
  if (block_ptr->size >= size) {
    // We have enough space. Could free some once we implement split.
    split_block(ptr, s);
    merge_two();
    return ptr;
  }

  // Need to really realloc. Malloc new space and free old space.
  // Then copy old data to new space.
  void *new_ptr;
  new_ptr = malloc(size);
  if (!new_ptr) {
    return NULL; // TODO: set errno on failure.
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  free(ptr);  
  return new_ptr;
}

int memory_leaks(){
  int leaks = 0;
  struct block_meta *b = global_base;
  while(b){
    leaks += META_SIZE;
    if(b->free){
      leaks += b->size;
    }
    b = b->next;
  }
  return leaks;
}
void print_list(){
  struct block_meta *b = global_base;
  if(b){
    printf("%lu %d",b->size,b->free );
    b = b->next;
  }
  printf("\n");
}

void print_all(){
  print_list();
  printf("In the heap %p \n", sbrk(0));
  printf("Memory leask: %d \n",memory_leaks);
}

void printAddress(){
      struct block_meta *b = global_base;
      while(b->next != NULL){
      printf("%p\n",b);
      b = b->next;
   }
   printf("%p\n", b);
 }
