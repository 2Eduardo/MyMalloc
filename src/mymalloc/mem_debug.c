#include "mem.h"

void
memalloc_debug_struct_info (FILE *f, const struct mem *address)
{
  size_t i;

  fprintf (f,
           "start:   %p\n"
           "size:    %lu\n"
           "is_free: %d\n",
           (void *)address, address->capacity, address->is_free);

  for (i = 0; i < DEBUG_FIRST_BYTES; ++i)
    fprintf (f, "%hhX", ((char *)address)[sizeof (struct mem) + i]);
  putc ('\n', f);
}

void
memalloc_debug_heap (FILE *f, const struct mem *ptr)
{
  for (; ptr; ptr = ptr->next)
    memalloc_debug_struct_info (f, ptr);
}
