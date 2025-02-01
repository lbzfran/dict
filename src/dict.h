#ifndef DICT_H
#define DICT_H

#include "arena.h"

typedef struct {
    uint32* keys;
    uint32* values;
    memory_index size;
    memory_index capacity;
    memory_index minimumDictSize;
} DictStruct;

// NOTE(liam): exposed functions.
typedef struct {
    void(*add)(DictStruct*,uint32,uint32);
    uint32(*pop)(DictStruct*,uint32);
    uint32(*get)(DictStruct,uint32);
    void(*setSize)(DictStruct* dict, memory_index size);
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

static uint32 DictHash(uint32 x, memory_index m)
{
    // NOTE(liam): implements 2-independent hashing.
    static uint32 a = 0, b = 0;
    uint32 res;

    if (!a && !b)
    {
        if (!dict_series.index)
        {
            dict_series = RandomSeed(999);
        }
        a = RandomChoice(&dict_series, 999) % m;
        b = RandomChoice(&dict_series, 999) % m;
    }
    if (!x)
    {
        res = 0;
    }
    else
    {
        res = (uint32)((a * x + b) % m);
        if (!res) res = 0;
    }

    return res;
}

static void DictSetMinimumSize(DictStruct* dict, memory_index size)
{
    dict->minimumDictSize = size;
}

static void DictAdd(DictStruct* dict, uint32 key, uint32 value)
{
    if ((dict->size + 1) > dict->capacity)
    {
        if (!dict->minimumDictSize)
        {
            // NOTE(liam): default value
            dict->minimumDictSize = 64;
        }
        // NOTE(liam): performing immediate rehashing.
        // This is an O(n) operation.
        // We're not anticipating resizing too often,
        // so this tradeoff is okay on average.
        // If anything, we can just adjust the minimum size
        // as necessary for the workload.
        memory_index lastCapacity = dict->capacity;
        dict->capacity = Max(dict->capacity * 2, dict->minimumDictSize);

        uint32* newKeys = PushArray(&dict_arena, uint32, dict->capacity);
        uint32* newValues = PushArray(&dict_arena, uint32, dict->capacity);

        if (lastCapacity)
        {
            //TODO(liam): properly rehash everything.
            memory_index sizePos = lastCapacity + 1;
            while (--sizePos)
            {
                uint32* oldKey = dict->keys + sizePos;
                uint32 oldHash = *oldKey;

                uint32 oldValue = *(dict->values + *oldKey);

                printf("key: %lu | value: %d | hash: %d\n", sizePos, oldValue, oldHash);
            }
        }

        dict->keys = newKeys;
        dict->values = newValues;

        printf("Capacity adjusted! Now at %lu\n", dict->capacity);
    }

    uint32 hashed_key = DictHash(key, dict->capacity);

    *(dict->keys + key) = hashed_key;

    *(dict->values + hashed_key) = value;
    dict->size++;

    /*printf("Occupied %d at %d\n", hashed_key, dict->values[hashed_key]);*/
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

static uint32 DictPop(DictStruct* dict, uint32 key)
{
    uint32 hashed_key = *(dict->keys + key);

    uint32 res;
    if (!hashed_key)
    {
        res = 0;
    }
    else
    {
        // NOTE(liam): I implemented a way to downsize capacity, but
        // honestly it shouldn't matter because the memory is already
        // allocated, and will continue to be allocated still,
        // so downsizing accomplishes nothing.
        //
        // It would be more efficient to free the arena instead and
        // reallocate.
        // Also, we can't guarantee the hashed keys will continue to
        // exist using the naive ArenaCopy since it won't read past
        // a certain point of the original capacity.
        // Basically, ArenaCopy is good only when you're allocating
        // bigger than your current capacity, or if you know where
        // all the data is indexed in the struct/array.

        res = *(dict->values + hashed_key);
        dict->values[hashed_key] = 0;
        dict->keys[key] = 0;
        dict->size--;

    }

    return res;
}

static void DictClear(DictStruct* dict)
{
    dict->keys = NULL;
    dict->values = NULL;
    dict->size = 0;
}

static void DictFree(void)
{
    // NOTE(liam): this frees the entire arena,
    // rendering all previous allocations unusable.
    ArenaFree(&dict_arena);
}

dict_lib const dict = {
    .add = DictAdd,
    .pop = DictPop,
    .setSize = DictSetMinimumSize,
    .get = DictGet,
    .clear = DictClear,
    .free = DictFree,
};

#endif // DICT_IMPLEMENTATION
