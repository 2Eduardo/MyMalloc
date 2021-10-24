#ifndef MEM_H
#define MEM_H

#define _USE_MISC

#include <stdio.h>
#include <stdbool.h>

#define HEAP_START ((void *)0x04040000)

struct mem;

#pragma pack(push, 1)
struct mem
{
  struct mem *next;
  size_t capacity;
  bool is_free;
};
#pragma pack(pop)

extern void *malloc (size_t query);
extern void free (void *mem);

#define DEBUG_FIRST_BYTES 4

extern void memalloc_debug_struct_info (FILE *f,
                                        const struct mem *const address);

extern void memalloc_debug_heap (FILE *f, const struct mem *ptr);

#endif /* MEM_H */
