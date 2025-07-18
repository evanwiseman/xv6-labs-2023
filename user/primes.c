#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void 
exec_pipe(int fd) {
  int prime;
  if (read(fd, &prime, 4) != 4) {
    close(fd);
    return;
  }

  // output current prime number
  printf("prime %d\n", prime);

  // build right pipe
  int p[2];
  pipe(p);

  // filter prime numbers into right
  int num;
  while (read(fd, &num, 4) == 4) {
    if (num % prime != 0) {
      write(p[1], &num, 4);
    }
  }
  
  close(p[1]);

  int pid = fork();
  if (pid == 0) { 
    // close file descriptors
    // execute on right pipe
    close(fd);
    exec_pipe(p[0]);
    close(p[0]);
  } else {
    // all work done in parent
    close(p[0]);
    close(fd);
    wait(0);
  }
}

int
main(int argc, char **argv) {
  // build left pipe
  int p[2];
  pipe(p);
  
  int pid = fork();
  if (pid == 0) {
    close(p[1]);
    exec_pipe(p[0]);
    close(p[0]);
  } else {
    close(p[0]);
    for (int i = 2; i < 35; i++) {
      write(p[1], &i, 4);
    }
    close(p[1]);
    wait(0);
  }
  exit(0);
}