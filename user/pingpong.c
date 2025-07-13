#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  int p1[2], p2[2];
  char buf[5];

  pipe(p1); // parent -> child
  pipe(p2); // child -> parent

  int pid = fork();
  if (pid > 0) {
    // Parent
    close(p1[0]);             // close parent read (not using)
    close(p2[1]);             // close child write (not using) 

    write(p1[1], "ping", 4);  // write to child
    close(p1[1]);             // close parent write (done using)

    read(p2[0], buf, 4);      // read from child
    close(p2[0]);             // close child read   (done using)

    buf[4] = '\0';
    printf("%s", buf);
  } else {
    close(p1[1]);             // close parent write (not using)
    close(p2[0]);             // close child read (not using)

    read(p1[0], buf, 4);      // read from parent
    close(p1[0]);             // close parent read (done using)

    buf[4] = '\0';
    printf("%s", buf);

    write(p2[1], "pong", 4);  // write to parent
    close(p2[1]);             // close child write (done using)
  }

  exit(0);
}
