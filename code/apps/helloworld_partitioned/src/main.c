#include <stdio.h>

int val[33554432];

int main() {
    printf("hello world - executing test hypercalls\n");


    asm __volatile__ (
        "movl $12, %%eax \n\t"
        "vmmcall \n\t"
        :
        :
        : "%rax");

    asm __volatile__ (
        "movl $13, %%eax \n\t"
        "movl $1, %%ebx \n\t"
        "vmmcall \n\t"
        :
        :
        : "%rax", "%rbx");

    for(int i = 0; i < 33554432; i++) {
	    val[i] = 1;
    }

    int x = 0;

    for(int i = 0; i < 33554432; i++) {
	    x += val[i];
    }

    printf("final value is %d\n", x);

    return 0;
}
