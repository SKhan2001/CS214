#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
struct block 
{
  size_t size;
  struct block *next;
  struct block *prev;
  int free;
};

void myinit(int);
void *mymalloc(size_t);
struct block *firstfit(struct block*, size_t);
struct block *nextfit(struct block*, size_t);
struct block *bestfit(struct block*, size_t);
void myfree(void*);
void *myrealloc(void*, size_t);
void mycleanup();
void split(struct block*, size_t);
void merge();
double utilization();
double total_blocks();
void* getTail();
void* getPrev(void*);
void* getNext(void*);
int getSize(void*);
void* getLast();