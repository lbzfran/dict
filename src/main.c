#include <stdio.h>

#define DICTIONARY_IMPLEMENTATION
#include "dict.h"


int main(void)
{
    printf("Hello World!\n");

    DictStruct d = {0};

    dict.add(&d, 12, 1);
    dict.add(&d, 13, 2);
    dict.add(&d, 14, 3);
    dict.add(&d, 15, 4);
    dict.add(&d, 16, 5);

    printf("value of dict at %d: %d\n", 12, dict.get(d, 12));

    dict.add(&d, 12, 5);
    printf("value of dict at %d: %d\n", 12, dict.get(d, 12));

    DictFree();
    return 0;
}
