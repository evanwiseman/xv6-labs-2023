#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  if(argc != 2){
    fprintf(2, "error: invalid arguments\n");
    exit(1);
  }
  sleep(atoi(argv[1]));
  exit(0);
}
