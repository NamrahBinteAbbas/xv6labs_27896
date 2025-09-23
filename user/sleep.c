#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    printf("Usage: sleep <ticks>\n");
    exit(1);
  }

  int ticks = atoi(argv[1]);   // convert string argument to integer
  if(ticks < 0){
    printf("sleep: invalid number of ticks\n");
    exit(1);
  }

  sleep(ticks);  // call the xv6 system call
  exit(0);
}
