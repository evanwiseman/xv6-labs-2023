#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void filter(int input[]) {
  int prime, n;

  close(input[1]);
  if ((n = read(input[0], &prime, sizeof(int))) != sizeof(int)) {
    close(input[0]);
    exit(0);
  }

  printf("prime %d\n", prime);

  int output[2];
  pipe(output);

  int pid = fork();
  if(pid == 0) {
    close(output[0]);
    
    int num;
    while ((n = read(input[0], &num, sizeof(int))) == sizeof(int)) {
      if (num % prime != 0) {
        write(output[1], &num, sizeof(int)); 
      }
    }

    close(input[0]);
    close(output[1]);
    exit(0);
  } else {
    wait(0);

    close(input[0]);
    close(output[1]);

    close(0);
    dup(output[0]);
    close(output[0]);

    char *argv[] = {"primes", 0};
    exec(argv[0], argv);

    fprintf(2, "exec failed\n");
    exit(1);
  }
}

int
main(int argc, char **argv) {
  int input[2];
  pipe(input);

  int pid = fork();
  if (pid == 0) { // initialize the pipe
    close(input[0]);
    for (int i = 2; i <= 35; i++) {
      write(input[1], &i, sizeof(int));
    }
    close(input[1]);
    exit(0);
  } else {
    wait(0);
    filter(input);
  }
}