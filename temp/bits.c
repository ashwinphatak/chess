#include <stdio.h>

void main(void)
{

    int i;

    for(i = 63; i >=0; i--) {
        printf("%2d ", i);
        if(i%8 == 0) printf("\n\n");
    }


}
