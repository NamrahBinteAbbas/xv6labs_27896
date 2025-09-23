#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  char *p = data;

  for (; *fmt; fmt++) {
    char f = *fmt;
    switch (f) {
    case 'i': {
      int v = ((unsigned char)p[0]) | ((unsigned char)p[1] << 8) |
              ((unsigned char)p[2] << 16) | ((unsigned char)p[3] << 24);
      printf("%d ", v);
      p += 4;
      break;
    }
    case 'p': {
      unsigned long v = 0;
      for (int i = 0; i < 8; i++)
        v |= ((unsigned long)(unsigned char)p[i]) << (8 * i);

      printf("0x%lx ", v);
      p += 8;
      break;
    }
    case 'h': {
      short v = ((unsigned char)p[0]) | ((unsigned char)p[1] << 8);
      printf("%d ", v);
      p += 2;
      break;
    }
    case 'c': {
      char ch = *p;
      if (ch >= 32 && ch <= 126)
        printf("%c ", ch);
      else
        printf("? ");
      p += 1;
      break;
    }
    case 's': {
      unsigned long ptr = 0;
      for (int i = 0; i < 8; i++)
        ptr |= ((unsigned long)(unsigned char)p[i]) << (8 * i);
      char *sp = (char *)ptr;
      if (sp)
        printf("%s ", sp);
      else
        printf("(null) ");
      p += 8;
      break;
    }
    case 'S': {
      printf("%s ", p);
      p += strlen(p) + 1;
      break;
    }
    default:
      printf("[?%c?] ", f);
      break;
    }
  }
  printf("\n");

}
