#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int t = uptime();        // call uptime system call
  printf("%d\n", t);       // print ticks since boot
  exit(0);
}

