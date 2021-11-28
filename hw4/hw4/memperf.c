#include "mymalloc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
int main()
{
    myinit(2);
    char *p1 = mymalloc(20);
    char *p2 = mymalloc(40);
    char *p3 = mymalloc(60);
    char *p4 = mymalloc(80);
    char *p5 = mymalloc(60);
    myfree(p2);
    char *p6 = mymalloc(40);
    printf("%f\n", utilization());
    return 0;
}       