

// Include Files
#include <stdio.h>
#include <math.h>

// Project Includes
#include <assign2-support.h>

//
// Functions


int main(){

//	printf("Before Malloc: %p\n",sbrk(0));
//	printf("After Malloc: %p\n",sbrk(0));
//	printf("Memory Leaks: %d\n",memory_leaks() );
//	printf("\n");

//10 malloc call:
	
//	char *p1 = (char *)malloc(5);
//        char *p2 = (char *)malloc(10);
//	free(p1);
//        char *p3 = (char *)malloc(5);
//        char *p4 = (char *)malloc(10);
//        char *p5 = (char *)malloc(5);
//        char *p6 = (char *)malloc(10);
//	free(p4);
//        char *p7 = (char *)malloc(5);
//        char *p8 = (char *)malloc(10);
//        char *p9 = (char *)malloc(5);
//        char *p10 = (char *)malloc(10);
	  
	int *q;
	printf("Starting the malloc\n");	
	q = malloc(20);
	q = malloc(15);
	q = malloc(30);
	printf("print the address and the memory leaks before using free \n");
	printAddress();
        printf("Memory Leaks: %d\n",memory_leaks());        
	free(q);       
        printf("After using free \n");
        printf("Memory Leaks: %d\n",memory_leaks());
        q = malloc(40);
        q = malloc(55);
        q = malloc(60);
        q = malloc(70);
        q = malloc(85);
        q = malloc(90);
        q = malloc(100);
	printf("called total of 10 malloc and here is the address: \n");
	printAddress();
        printf("Memory Leaks: %d\n",memory_leaks());
	free(q);
	printf("using free agina");
      	printf("\n");
      	printf("Memory Leaks: %d\n",memory_leaks() );
	printf("more memory leaks after using free \n");
	printf("\n");
        printf("Starting the realloc\n");	
	q = realloc(q,10);
        q = realloc(q,20);       
	q = realloc(q,30);
        q = realloc(q,40);
        q = realloc(q,50);
        q = realloc(q,60);
        q = realloc(q,100);
        q = realloc(q,20);
        q = realloc(q,40);
 	printAddress();
        printf("Memory Leaks: %d\n",memory_leaks());
	printf("No memory leaks after using realloc\n");
	printf("\n");

        printf("called total of 10 calloc: \n");
        calloc(100,1);
        calloc(110,1);
        calloc(121,1);
        calloc(104,1);
        calloc(111,1);
        calloc(132,1);
        calloc(134,1);
        calloc(122,1);
        calloc(123,1);
        calloc(124,1);
	printf("Memory Leaks: %d\n",memory_leaks());
        printf("memory leaks after using realloc\n");        
return(0);
}
