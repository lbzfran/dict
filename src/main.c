#include "random.h"
#include <stdio.h>

#define DICTIONARY_IMPLEMENTATION
#include "dict.h"

#include "random.h"


int main(void)
{
    RandomSeries local_series = RandomSeed(1999);
    printf("Hello World!\n");

    DictStruct d = {0};

    dict.add(&d, 12, 6969);
    dict.add(&d, 13, 6969);
    dict.add(&d, 14, 6969);
    dict.add(&d, 15, 6969);
    dict.add(&d, 16, 6969);
    dict.add(&d, 17, 6969);
    dict.add(&d, 18, 6969);
    dict.add(&d, 19, 6969);
    for (int i = 0; i < 48; i++)
    {
        dict.add(&d, RandomBetween(&local_series, 20, 999), RandomBetween(&local_series, 20, 999));
    }

    dict.add(&d, 10, RandomBetween(&local_series, 20, 999));
    dict.add(&d, 20, RandomBetween(&local_series, 20, 999));
    dict.add(&d, 30, RandomBetween(&local_series, 20, 999));
    dict.add(&d, 40, RandomBetween(&local_series, 20, 999));
    dict.add(&d, 50, RandomBetween(&local_series, 20, 999));
    dict.add(&d, 60, RandomBetween(&local_series, 20, 999));

    printf("value of dict after random adds at %d: %d\n", 12, dict.get(d, 12));
    printf("value of dict after random adds at %d: %d\n", 13, dict.get(d, 13));
    printf("value of dict after random adds at %d: %d\n", 14, dict.get(d, 14));
    printf("value of dict after random adds at %d: %d\n", 15, dict.get(d, 15));
    printf("value of dict after random adds at %d: %d\n", 16, dict.get(d, 16));
    printf("value of dict after random adds at %d: %d\n", 17, dict.get(d, 17));
    printf("value of dict after random adds at %d: %d\n", 18, dict.get(d, 18));
    printf("value of dict after random adds at %d: %d\n", 19, dict.get(d, 19));

    dict.add(&d, 12, 5);
    printf("value of dict after explicit replace at %d: %d\n", 12, dict.get(d, 12));

    printf("value of %d during pop: %d\n", 40, dict.pop(&d, 40));
    printf("value of %d during pop: %d\n", 50, dict.pop(&d, 50));
    printf("value of %d during pop: %d\n", 60, dict.pop(&d, 60));

    dict.pop(&d, 10);
    dict.pop(&d, 20);
    dict.pop(&d, 30);

    printf("value of %d after pop: %d\n", 10, dict.get(d, 10));
    printf("value of %d after pop: %d\n", 20, dict.get(d, 20));
    printf("value of %d after pop: %d\n", 30, dict.get(d, 30));

    DictFree();
    return 0;
}
