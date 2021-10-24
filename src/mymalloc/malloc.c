#include "mem.h"

#include <stdbool.h>
#include <errno.h>

#include <sys/mman.h>
#include <unistd.h>

#define PAGESIZE getpagesize ()

#define MMAP_PROT (PROT_READ | PROT_WRITE)
#define MMAP_FLAGS (MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE)

#define BLOCK_MIN_SIZE 4 * sizeof (void *)
#define CHECK_QUERY(query)                                                    \
  (((query) < (BLOCK_MIN_SIZE)) ? (BLOCK_MIN_SIZE) : (query))

static bool heap_started = false;

static size_t check_size_alignment (size_t size);
static void *heap_init (size_t initial_size);

void *
malloc (size_t query)
{
  size_t size = CHECK_QUERY (query);
  struct mem *cur = heap_started ? (struct mem *)HEAP_START : NULL;
  void *result;

  if (!cur)
    {
      heap_started = true;
      cur = heap_init (check_size_alignment (size));
    }

  while (cur)
    {
      if (cur->is_free)
        {
          if (query <= cur->capacity - sizeof (struct mem) - BLOCK_MIN_SIZE)
            {
              /* Split block. */
              struct mem *fwd;
              size_t cur_capacity = cur->capacity;

              result = (void *)((size_t)cur + sizeof (struct mem));

              fwd = (struct mem *)((size_t)result + query);
              fwd->next = cur->next;

              cur->next = fwd;
              cur->capacity = size;
              cur->is_free = false;

              fwd->capacity = cur_capacity - size;
              fwd->is_free = true;

              return result;
            }
        }
      if (!cur->next)
        {
          /* End of page. */
          void *end = (void *)cur + cur->capacity;

          /* Needs more memory. */
          end = mmap (end, PAGESIZE, MMAP_PROT, MMAP_FLAGS, -1, 0);
          if (end != MAP_FAILED)
            {
              if (cur->is_free)
                {
                  cur->capacity += PAGESIZE;
                  continue;
                }
              else
                {
                  cur->next = (struct mem *)end;
                  cur->next->is_free = true;
                  cur->next->capacity = PAGESIZE;
                  cur->next->next = NULL;
                }
            }
        }
      cur = cur->next;
    }

  errno = ENOMEM;
  return NULL;
}

void
free (void *ptr)
{
  struct mem *mem_ptr = (struct mem *)(ptr - sizeof (struct mem));
  struct mem *fwd = mem_ptr->next;

  mem_ptr->is_free = true;
  if (fwd && fwd->is_free)
    {
      /* Join blocks. */
      mem_ptr->next = fwd->next;
      mem_ptr->capacity += fwd->capacity;
    }
}

static void *
heap_init (size_t initial_size)
{
  void *map;

  map = mmap (HEAP_START, initial_size, MMAP_PROT, MMAP_FLAGS, -1, 0);
  if (map != MAP_FAILED)
    {
      struct mem *m = (struct mem *)map;
      m->capacity = initial_size;
      m->is_free = true;
      m->next = NULL;

      return map;
    }
  else
    return NULL;
}

static size_t
check_size_alignment (size_t size)
{
  size_t result;

  if (size < PAGESIZE)
    return PAGESIZE;

  result = size % PAGESIZE;
  if (result != 0)
    result = size - result + PAGESIZE;
  else
    result = size;

  return result;
}
