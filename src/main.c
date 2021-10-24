#include <stdio.h>

#include <unistd.h>

#include "mem.h"

typedef struct
{
  int x, y;
} qword;

static void debug ();

/* TODO: we need more test cases. */
int
main ()
{
  size_t s = getpagesize () / sizeof (qword);
  qword *q0 = malloc (s * sizeof (qword));
  qword *q1 = malloc (sizeof (qword));
  free (q0);
  qword *q2 = malloc (sizeof (qword));
  debug ();
  return 0;
}

static void
debug ()
{
  FILE *log = fopen ("memlog", "w");
  if (log)
    {
      memalloc_debug_heap (log, HEAP_START);
      fclose (log);
    }
}
