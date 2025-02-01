
#ifndef DICT_H
#define DICT_H

#include "arena.h"

typedef struct {
    uint32* keys;
    uint32* values;
    uint32 size;
    uint32 occupied;
} DictStruct;

// NOTE(liam): exposed functions.
typedef struct {
    void(*add)(DictStruct*,uint32,uint32);
    uint32(*get)(DictStruct,uint32);
    void(*clear)(DictStruct*);
    void(*free)(void);
} dict_lib;
extern dict_lib const dict;

#endif // DICT_H

#define DICT_IMPLEMENTATION // DEBUG
#ifdef DICT_IMPLEMENTATION

#define ARENA_IMPLEMENTATION
#include "arena.h"

#include "random.h"

static Arena dict_arena = {0};
static RandomSeries dict_series = {0};

static uint32 DictHash(uint32 x, uint32 m)
{
    // NOTE(liam): implements 2-independent hashing.
    static uint32 a = 0, b = 0;
    if (!a && !b)
    {
        if (!dict_series.index)
        {
            dict_series = RandomSeed(999);
        }
        a = RandomChoice(&dict_series, 999) % m;
        b = RandomChoice(&dict_series, 999) % m;
    }
    uint32 res = (uint32)((a * x + b) % m);

    return res;
}

static void DictPut(DictStruct* dict, uint32 key, uint32 value)
{
    if ((dict->occupied + 1) > dict->size)
    {
        if (!dict->size)
        {
            // NOTE(liam): default value
            dict->size = 64;
        }
        // TODO(liam): allow automated resizing.
        dict->keys = PushArray(&dict_arena, uint32, dict->size);
        dict->values = PushArray(&dict_arena, uint32, dict->size);
    }

    uint32 hashed_key = DictHash(key, dict->size);

    *(dict->keys + key) = hashed_key;
    *(dict->values + hashed_key) = value;
    dict->occupied++;

    printf("Occupied %d at %d\n", hashed_key, dict->values[hashed_key]);
}

static uint32 DictGet(DictStruct dict, uint32 key)
{
    uint32 hashed_key = *(dict.keys + key);

    uint32 res;
    if (!hashed_key)
    {
        printf("Missing hash key.\n");
        res = 0;
    }
    else
    {
        res = *(dict.values + hashed_key);
    }

    return res;
}

static void DictClear(DictStruct* dict)
{
    dict->keys = NULL;
    dict->values = NULL;
    dict->occupied = 0;
}

static void DictFree(void)
{
    // NOTE(liam): this frees the entire arena,
    // rendering all previous allocations unusable.
    ArenaFree(&dict_arena);
}

dict_lib const dict = {
    .add = DictPut,
    .get = DictGet,
    .clear = DictClear,
    .free = DictFree,
    /*DictPut*/
};

#endif // DICT_IMPLEMENTATION
