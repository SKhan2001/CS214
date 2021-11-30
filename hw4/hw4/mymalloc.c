#include "mymalloc.h"
#include <time.h>
#define CAPACITY 1024*1024
char heap[CAPACITY];
clock_t t;
int algo;
double total_allocated =0.0;
double total_space=0.0; 
struct block *Block = (void*)heap;
struct block *freeList = (void*)heap;
struct block *head = (void*)heap;
struct block *tail = (void*)heap;
struct block *last_allocated = (void*)heap;  

void myinit(int allocAlg)
{
    algo = allocAlg;
    Block->size = CAPACITY-sizeof(struct block);
    Block->free = 1;
    Block->next = NULL;
    heap[CAPACITY];

}


void *mymalloc(size_t noOfBytes) 
{
    struct block *curr = head;
    void *result;
    void *previous;

    if (algo == 0) curr = firstfit(curr, noOfBytes);
    if (algo == 1) curr = nextfit(last_allocated, noOfBytes);
    if (algo == 2) curr = bestfit(curr, noOfBytes);

    if((curr->size)==noOfBytes)
    {
        //printf("EXACT FIT\n");
        curr->free=0;
        result=curr;
        total_allocated += noOfBytes;
        return result;
    }

    else if((curr->size)>(noOfBytes+sizeof(struct block)))
    {
        //printf("SPLITTING BLOCK\n");
        split(curr,noOfBytes);
        result=curr;
        curr->free = 0;
        Block = Block->next;
        Block->prev = curr;
        total_allocated += noOfBytes;
        total_space+= noOfBytes;
        tail = result;
        return result;
    }

    else
    {
        result=NULL;
       printf("Sorry. No sufficient memory to allocate\n");
        return result;
    }
}

struct block *firstfit(struct block *curr, size_t noOfBytes)
{
    while ((curr->next) != NULL)
    {
        if (((curr->size) >= noOfBytes) && (curr->free)) {
            if(curr->size == noOfBytes) break;
             //split(curr, noOfBytes);
            break;
        }
        curr = curr->next;
    }
    last_allocated = curr;
    return curr;
}

struct block *nextfit(struct block *curr, size_t noOfBytes)
{
    if (tail ==head){
        curr = firstfit(curr, noOfBytes);
        last_allocated = curr;
        return curr;
    }
    if (curr->next == NULL){
        curr = firstfit(curr, noOfBytes);
        last_allocated = curr;
        return curr;
    }
    curr = curr->next;
    while ((curr->next) != NULL)
    {
        if (((curr->size) >= noOfBytes) && (curr->free)) {
            if(curr->size == noOfBytes) break;
            split(curr, noOfBytes);
            break;
        }
        curr = curr->next;
    }
    last_allocated = curr;
    return curr;
}

struct block *bestfit(struct block *curr, size_t noOfBytes)
{
    double tempSpace =0;
    double currSpace =0;
    struct block *temp = firstfit(curr, noOfBytes);
    //if(temp -> next == NULL)return temp;
    while ((curr->next) != NULL)
    {
        tempSpace = temp->size - noOfBytes;
        currSpace = curr->size - noOfBytes;
        if(curr->free && (currSpace < tempSpace)) temp = curr;
        curr = curr->next;
    }

    if (((temp->size) - noOfBytes) < 0) return NULL;
    return temp;
}

void myfree(void *ptr) 
{
  if (!ptr) return;
  struct block* block_ptr = ptr;
  total_allocated -= block_ptr->size;
  if(block_ptr == tail){
     total_space -= block_ptr-> size;
     tail = block_ptr -> prev;
  }
  block_ptr->free = 1;
  merge();
}

void *myrealloc(void *ptr, size_t size) 
{
    struct block* block_ptr = ptr;
    int temp = size - block_ptr->size;
    total_allocated -= temp;
    myfree(ptr);
    return mymalloc(size);
}

void mycleanup()
{
    struct block *temp = head;
    while (temp->next != NULL)
    {
        temp = temp->next;
        myfree(temp->prev);
    }
    merge();
    myfree(Block);
    tail = (void*)heap;
    head = (void*)heap;
    last_allocated = head;
    total_allocated =0;
    total_space =0;
}

void split(struct block *fitting_slot,size_t noOfBytes)
{
  struct block *new = (void*)((void*)fitting_slot+noOfBytes+sizeof(struct block));
  new->size=(fitting_slot->size)-noOfBytes-sizeof(struct block);
  new->free=1;
  new->next=fitting_slot->next;
  fitting_slot->size=noOfBytes;
  fitting_slot->free=0;
  fitting_slot->next = new;
}

void merge()
{
    struct block *curr;
    curr = Block;
    while((curr->next)!=NULL)
    {
        if((curr->free) && (curr->next->free))
        {
            curr->size+=(curr->next->size)+sizeof(struct block);
            curr->next=curr->next->next;
            continue;
        }
        curr=curr->next;
    }
}

double total_blocks(){
    return total_allocated; 
}
double utilization(){

    //printf("%f/%f\n", total_allocated,total_space);
    return total_allocated/total_space;
}

void* getTail(){
    return tail;
}

void* getPrev(void *ptr){
    struct block *block_ptr = ptr;
    return block_ptr->prev;

}
void* getNext(void *ptr){
    struct block *block_ptr = ptr;
    return block_ptr->next;
}


int getSize(void *ptr){
    struct block *block_ptr = ptr;
    return block_ptr->size;
}

void* getLast(){
    return last_allocated;
}

//Goes before calling function mymalloc/myrealloc/myfree
void start_clock()
{
    t = clock();
}

//Goes after calling function mymalloc/myrealloc/myfree
double throughput()
{
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    return 10000000/time_taken;
}

void printFree(){
    struct block *block_ptr = head;
    while(block_ptr->next != NULL){
        if(block_ptr->free){
            printf("%p->[%ld]\n", block_ptr, block_ptr->size);
        }
        block_ptr = block_ptr->next;
    }
}
void allocList(){
    struct block *block_ptr = head;
    double temp =0;
    while(block_ptr->next != NULL){
        if(!block_ptr->free){
        printf("%f + %ld\n", temp, block_ptr->size);
        temp += block_ptr->size;
        }
        block_ptr = block_ptr->next;
    }

}


  
