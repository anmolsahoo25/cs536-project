#include <stdio.h>

int val[33554432];

int main() {
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
