#include "mymalloc.h"
#define CAPACITY 1024*1024
char heap[CAPACITY];
int algo;
double total_allocated =0.0;
double total_space=0.0; 
struct block *Block = (void*)heap;
struct block *head = (void*)heap;
struct block *tail = (void*)heap;
 
void myinit(int allocAlg)
{
    algo = allocAlg;
    Block->size = CAPACITY-sizeof(struct block);
    Block->free = 1;
    Block->next = NULL;
}


void *mymalloc(size_t noOfBytes) 
{
    struct block *curr = head;
    void *result;
    void *previous;

    if (algo == 0) curr = firstfit(curr, noOfBytes);
    if (algo == 1) curr = nextfit(curr, noOfBytes);
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
       // printf("SPLITTING BLOCK\n");
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
            split(curr, noOfBytes);
            break;
        }
        curr = curr->next;
    }
  
    return curr;
}

struct block *nextfit(struct block *curr, size_t noOfBytes)
{
    curr = firstfit(curr, noOfBytes);
    if (curr->next == NULL){
        curr = firstfit(curr, noOfBytes);
        return curr;
    }
    curr = curr->next;
    while ((curr->next) != NULL)
    {
        if (((curr->size) <= noOfBytes) &&   (curr->free)) {
            printf("%p\n", curr);
            break;
        }
        curr = curr->next;
    }
    
    return curr;
}

struct block *bestfit(struct block *curr, size_t noOfBytes)
{
    struct block *temp = curr;
    while ((curr->next) != NULL)
    {
        
        if (((temp->size) - noOfBytes) > ((curr->size) - noOfBytes)) curr = temp;
        curr = curr->next;
    }

    if (((curr->size) - noOfBytes) < 0) return NULL;
    if(curr->next == NULL){
        total_allocated += noOfBytes;
        total_space += noOfBytes;
    } 
    else{
        total_allocated += noOfBytes;
    }
    return curr;
}

void myfree(void *ptr) 
{
  if (!ptr) return;
  struct block* block_ptr = ptr;
  total_allocated -= block_ptr->size;
  if(block_ptr == tail){
      total_space-=block_ptr->size;
      tail = block_ptr->prev;
  }
  block_ptr->free = 1;
  merge();
 
}

void *myrealloc(void *ptr, size_t size) 
{
    struct block* block_ptr = ptr;
    int temp = size - block_ptr->size;
    total_allocated += temp;
    total_space += temp;
    myfree(ptr);
    return mymalloc(size);
}

void mycleanup()
{
    while (Block->next != NULL)
    {
        struct block *temp = Block;
        myfree(Block);
        Block = temp;
    }
    myfree(Block);
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


  