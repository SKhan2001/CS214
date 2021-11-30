#include "mymalloc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
int main()
{
    // myinit(0);
    // char *test1 = mymalloc(1);
    // char *test2 = mymalloc(50);
    // char *test3 = mymalloc(139);
    // myfree(test2);
    // char *test4 = mymalloc(20);
    // printf("First fit throughput: %f ops/sec\n", throughput());
    // printf("First fit utilization: %f\n", utilization());
    // mycleanup();

    // myinit(1);
    // char *test5 = mymalloc(34);
    // char *test6 = mymalloc(7);
    // char *test7 = mymalloc(158);
    // myfree(test6);
    // char *test8 = mymalloc(20);
    // myfree(test7);
    // char *test9 = mymalloc(4);
    // printf("Next fit throughput: %f ops/sec\n", throughput());
    // printf("Next fit utilization: %f\n", utilization());
    // allocList();
    // printFree();
    // mycleanup();

    myinit(2);
    char *test10 = mymalloc(8);
    char *test11 = mymalloc(3);
    char *test12 = mymalloc(2);
    myfree(test11);
    char *test13 = mymalloc(22);
    allocList();
    printf("Best fit throughput: %f ops/sec\n", throughput());
    printf("Best fit utilization: %f\n", utilization());
    mycleanup();
    
    return 0;
}       