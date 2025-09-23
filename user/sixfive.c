#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "stddef.h"

// helper: check if a character is a separator
static int is_sep(char c) {
  return (c == ' ' || c == '-' || c == '\r' || c == '\t' ||
          c == '\n' || c == '.' || c == '/' || c == ',');
}

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: sixfive file...\n");
    exit(1);
  }

  for (int f = 1; f < argc; f++) {
    int fd = open(argv[f], 0);
    if (fd < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[f]);
      exit(1);
    }

    char buf[1];
    char numbuf[32];
    int nlen = 0;

    while (read(fd, buf, 1) == 1) {
      if (is_sep(buf[0])) {
        if (nlen > 0) {
          numbuf[nlen] = '\0';
          int n = atoi(numbuf);
          if (n % 5 == 0 || n % 6 == 0) {
            printf("%d\n", n);
          }
          nlen = 0;
        }
      } else if (buf[0] >= '0' && buf[0] <= '9') {
        if (nlen < sizeof(numbuf) - 1)
          numbuf[nlen++] = buf[0];
      }
    }

    // handle number at EOF
    if (nlen > 0) {
      numbuf[nlen] = '\0';
      int n = atoi(numbuf);
      if (n % 5 == 0 || n % 6 == 0) {
        printf("%d\n", n);
      }
    }

    close(fd);
  }

  exit(0);
}

