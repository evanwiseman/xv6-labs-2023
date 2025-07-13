#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  int left_pipe[2];
  pipe(left_pipe);

  int pid = fork();
  if (pid > 0) {
    close(left_pipe[0]);  // close read end of left_pipe
    for (int i = 2; i <= 35; i++) {
      write(left_pipe[1], &i, sizeof(int));
    }
    close(left_pipe[1]);  // done writing
    wait(0);
  } else {
    close(left_pipe[1]);  // close the write end of left_pipe

    int prime;
    int n;

    if ((n = read(left_pipe[0], &prime, sizeof(int))) != sizeof(int)) { 
      close(left_pipe[0]);
      exit(0);
    }

    printf("prime %d\n", prime);

    int right_pipe[2];
    pipe(right_pipe);

    int pid = fork();
    if (pid > 0) {
      close(right_pipe[0]);
      
      int num;
      while ((n = read(left_pipe[0], &num, sizeof(int))) == sizeof(int)) {
        if (num % prime != 0) {
          write(right_pipe[1], &num, sizeof(int));
        }
      }
      close(left_pipe[0]);
      close(right_pipe[1]);
      wait(0);
    } else {
      close(left_pipe[0]);
      close(left_pipe[1]);

      close(0);
      dup(right_pipe[0]);
      close(right_pipe[0]);

      exec(argv[0], argv);
      
      fprintf(2, "exec failed\n");
      exit(1);
    }
  }
  
  exit(0);
}
