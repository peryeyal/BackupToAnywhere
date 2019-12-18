#include "LTRFS.h"

int main(int argc, char *argv[])
{

  LTRFS fs;

  int status = fs.run(argc, argv);

  return status;
}
