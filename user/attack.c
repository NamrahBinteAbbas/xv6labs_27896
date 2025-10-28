#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../kernel/fcntl.h"
#include "user.h"
#define PGSIZE 4096
#define MIN_SECRET 4      // grader secrets >= 4
#define MAX_SECRET 32
#define MAX_PAGES 1500    // tuned: large but avoids exhausting host RAM

static int is_alnum_char(char c) {
  if (c >= '0' && c <= '9') return 1;
  if (c >= 'A' && c <= 'Z') return 1;
  if (c >= 'a' && c <= 'z') return 1;
  return 0;
}

int
main(int argc, char *argv[])
{
  char *page;
  char *addr;
  int i, j, w;

  for (i = 0; i < MAX_PAGES; i++) {
    addr = sbrk(PGSIZE);
    if (addr == (char*) -1) break;   // out of memory / no more room
    page = addr;

    /* scan this page; print and exit on first token that looks like a secret */
    for (j = 0; j < PGSIZE; j++) {
      if (!is_alnum_char(page[j])) continue;
      for (w = 1; w <= MAX_SECRET && j + w < PGSIZE; w++) {
        if (!is_alnum_char(page[j + w])) break;
      }
      if (w >= MIN_SECRET && w <= MAX_SECRET) {
        char buf[MAX_SECRET + 1];
        int k;
        for (k = 0; k < w; k++) buf[k] = page[j + k];
        buf[w] = '\0';
        printf("%s\n", buf);   // final required output (only the secret)
        exit(0);
      }
      j = j + w;
    }
  }

  /* Not found — exit with nonzero (no output) */
  exit(1);
}
