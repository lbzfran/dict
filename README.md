
# Dictionaries

Dictionary library in C.

Uses "fake" namespacing with an extern struct "dict",
and a mix of static function definitions.

TODO:
[ ] : allow different datatypes.

# Usage

```c main.c
#define DICTIONARY_IMPLEMENTATION
#include "dict.h"

DictStruct d = {0};

dict.put(d, 1, 54);

uint64 value = dict.get(d, 1); // value = 54.

// Release all current allocated dictionaries.
dict.free();
```
