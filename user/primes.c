#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Filter logic: reads from input_fd, filters, and spawns next stage
void
filter(int input_fd) {
  int prime, n;

  if ((n = read(input_fd, &prime, sizeof(int))) != sizeof(int)) {
    close(input_fd);
    exit(0);
  }

  printf("prime %d\n", prime);

  int p[2];
  pipe(p);

  int pid = fork();
  if (pid > 0) {
    // Parent: filter numbers and write to next pipe
    close(p[0]);
    int num;
    while ((n = read(input_fd, &num, sizeof(int))) == sizeof(int)) {
      if (num % prime != 0) {
        write(p[1], &num, sizeof(int));
      }
    }
    close(input_fd);
    close(p[1]);
    wait(0);
    exit(0);
  } else {
    // Child: prepare for exec with next pipe as stdin
    close(p[1]);
    close(input_fd);

    // Redirect p[0] → fd 0
    close(0);
    dup(p[0]);
    close(p[0]);

    char *args[] = {"sieve", "child", 0};
    exec(args[0], args);
    fprintf(2, "exec failed\n");
    exit(1);
  }
}

int
main(int argc, char **argv) {
  if (argc == 1) {
    // Initial parent: generate numbers 2..35
    int p[2];
    pipe(p);

    int pid = fork();
    if (pid > 0) {
      close(p[0]);
      for (int i = 2; i <= 35; i++) {
        write(p[1], &i, sizeof(int));
      }
      close(p[1]);
      wait(0);
      exit(0);
    } else {
      close(p[1]);
      filter(p[0]);
    }
  } else {
    // Re-exec’d children: read from stdin (which was dup’d from pipe)
    filter(0);
  }

  exit(0);
}