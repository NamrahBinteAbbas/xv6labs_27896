// user/find.c  -- find with optional -exec support

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"   // for MAXARG

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return the name part.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

/* run_exec: fork and exec the command cmd[0..cmdargc-1] with file appended */
void
run_exec(char *file, char **cmd, int cmdargc)
{
  char *argv[MAXARG+2];
  int i;

  if(cmdargc >= MAXARG){
    fprintf(2, "find: exec command too long (max %d)\n", MAXARG-1);
    return;
  }

  for(i = 0; i < cmdargc; i++){
    argv[i] = cmd[i];
  }
  argv[cmdargc] = file;
  argv[cmdargc+1] = 0;

  int pid = fork();
  if(pid < 0){
    fprintf(2, "find: fork failed\n");
    return;
  }
  if(pid == 0){
    exec(argv[0], argv);
    // If exec returns, it failed
    fprintf(2, "find: exec %s failed\n", argv[0]);
    exit(1);
  } else {
    // parent waits for the child
    wait(0);
  }
}

/* find: recursive directory traversal with optional exec behavior */
void
find(char *path, char *target, int do_exec, char **cmd, int cmdargc)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(strcmp(fmtname(path), target) == 0){
      if(do_exec){
        run_exec(path, cmd, cmdargc);
      } else {
        printf("%s\n", path);
      }
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, target, do_exec, cmd, cmdargc);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "usage: find <path> <filename> [-exec cmd ...]\n");
    exit(1);
  }

  char *path = argv[1];
  char *target = argv[2];

  int do_exec = 0;
  char **cmd = 0;
  int cmdargc = 0;

  if(argc > 3 && strcmp(argv[3], "-exec") == 0){
    do_exec = 1;
    cmd = &argv[4];
    cmdargc = argc - 4;
    if(cmdargc <= 0){
      fprintf(2, "find: -exec requires a command\n");
      exit(1);
    }
    if(cmdargc >= MAXARG){
      fprintf(2, "find: exec command too long (max %d)\n", MAXARG-1);
      exit(1);
    }
  }

  find(path, target, do_exec, cmd, cmdargc);
  exit(0);
}

