#include <stdio.h>

int main() {
    printf("hello world - executing test hypercalls\n");

    int ret = 1;

    asm __volatile__ (
        "movl $12, %%eax \n\t"
        "vmmcall \n\t"
        "movl %%eax, %0"
        : "=r" (ret)
        :
        : "%rax");

    printf("hypercall KVM_HC_CAT_SETUP returned %d\n", ret);

    ret = 1;

    asm __volatile__ (
        "movl $13, %%eax \n\t"
        "movl $1, %%ebx \n\t"
        "vmmcall \n\t"
        "movl %%eax, %0"
        : "=r" (ret)
        :
        : "%rax", "%rbx");

    printf("hypercall KVM_HC_CAT_ALLOC returned %d\n", ret);

    return 0;
}
