#ifndef DICT_H
#define DICT_H

#include "arena.h"

// TODO(liam): stuck on how dictionary should handle collisions.
// Likely linked list, but need to pin down the how of it.

typedef struct DictLinkedList {
    uint32 hash;
    uint32 value;
    struct DictLinkedList* next;
} DictLinkedList;

typedef struct {
    DictLinkedList* values; // size of keys.
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

#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

static uint32_t lookup3(const void *key, size_t length, uint32_t initval)
{
    uint32_t  a,b,c;
    const uint8_t  *k;
    const uint32_t *data32Bit;

    data32Bit = key;
    a = b = c = 0xdeadbeef + (((uint32_t)length)<<2) + initval;

    while (length > 12) {
        a += *(data32Bit++);
        b += *(data32Bit++);
        c += *(data32Bit++);
        mix(a,b,c);
        length -= 12;
    }

    k = (const uint8_t *)data32Bit;
    switch (length) {
        case 12: c += ((uint32_t)k[11])<<24;
        case 11: c += ((uint32_t)k[10])<<16;
        case 10: c += ((uint32_t)k[9])<<8;
        case 9 : c += k[8];
        case 8 : b += ((uint32_t)k[7])<<24;
        case 7 : b += ((uint32_t)k[6])<<16;
        case 6 : b += ((uint32_t)k[5])<<8;
        case 5 : b += k[4];
        case 4 : a += ((uint32_t)k[3])<<24;
        case 3 : a += ((uint32_t)k[2])<<16;
        case 2 : a += ((uint32_t)k[1])<<8;
        case 1 : a += k[0];
                 break;
        case 0 : return c;
    }
    final(a,b,c);
    return c;
}

static uint32 DictHash(uint32 key, uint32 size)
{
    uint32 initval;
    uint32 hashAddress;

    initval = 12345;
    hashAddress = lookup3(&key, 1, initval);
    uint32 res = (hashAddress & (size - 1));
    printf("HASH: in: %d, out: %d\n", key, res);
    return res;
    // If hashtable is guaranteed to always have a size that is a power of 2,
    // replace the line above with the following more effective line:
    //     return (hashAddress & (hashTableSize - 1));
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
        memory_index newCap = Max(dict->capacity * 2, dict->minimumDictSize);

        uint32* newKeys = PushArray(&dict_arena, uint32, newCap);
        DictLinkedList* newValues = PushArray(&dict_arena, struct DictLinkedList, newCap);

        if (dict->capacity)
        {
            //TODO(liam): properly rehash everything.
            memory_index sizePos = dict->capacity;
            uint32* oldKey = dict->keys;
            while (sizePos--)
            {
                uint32 oldHash = *oldKey;

                uint32 oldValue = *(dict->values + oldHash);



                if (oldValue)
                {
                    printf("value: %u | key: %ld | hash: %d\n", oldValue, sizePos, oldHash);
                    uint32 newHash = DictHash(sizePos, newCap);
                    *(newKeys + sizePos) = newHash;
                    *(newValues + newHash) = value;
                }

                oldKey++;
            }
        }

        dict->capacity = newCap;
        dict->keys = newKeys;
        dict->values = newValues;

        printf("Capacity adjusted! Now at %lu\n", dict->capacity);
    }

    uint32 hashed_key = DictHash(key, dict->capacity);

    uint32* keyPos = (dict->keys + key);
    // NOTE(liam): don't change size if we're replacing existing entry.
    if (!(*keyPos)) {
        /*static int counter = 0;*/
        /*printf("new entry! %d\n", counter++);*/
        dict->size++;
    }
    *keyPos = hashed_key;

    *(dict->values + hashed_key).value = value;

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
    // NOTE(liam): effective reset. Does not change capacity.
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
