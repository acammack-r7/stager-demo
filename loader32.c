#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>

#include <sys/mman.h>

unsigned char *st_str(unsigned char *elf, long index) {
  Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf;

  Elf32_Shdr *shdr = (Elf32_Shdr *)(elf + ehdr->e_shoff);
  while (shdr->sh_type != SHT_SYMTAB) shdr++;
  while (shdr->sh_type != SHT_STRTAB) shdr++; // The first strtab after the symtab

  return elf + shdr->sh_offset + index;
}

int main(int argc, char **argv)
{
  long testfile, testelf;
  struct stat *statbuf;
  unsigned char *testbuf, *t;
  void (*e_entry)(long *, long *);

  long got, rela, relasz, rel, relsz;

  long stack[9] = {0};
  long *dynv;

  statbuf = mmap(0, sizeof(struct stat), PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  if(argc < 3) {
    printf("loader [bin file] [elf file]\n");
    exit(EXIT_FAILURE);
  }

  testfile = open(argv[1], O_RDONLY);
  if (testfile < 0) {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }
  testelf = open(argv[2], O_RDONLY);
  if (testelf < 0) {
    perror(argv[2]);
    exit(EXIT_FAILURE);
  }

  fstat(testfile, statbuf);
  testbuf = mmap(0, statbuf->st_size, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_PRIVATE, testfile, 0);

  fstat(testelf, statbuf);
  t = mmap(0, statbuf->st_size, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_PRIVATE, testelf, 0);

  Elf32_Ehdr *ehdr = (Elf32_Ehdr *)t;
  Elf32_Phdr *phdr = (Elf32_Phdr *)(t + ehdr->e_phoff);

  Elf32_Shdr *shdr = (Elf32_Shdr *)(t + ehdr->e_shoff);
  while (shdr->sh_type != SHT_SYMTAB) shdr++;

  Elf32_Sym *symb = (Elf32_Sym *)(t + shdr->sh_offset);
  while (strcmp(st_str(t, symb->st_name), "_start_c") != 0) symb++;

  e_entry = (void *)(testbuf + symb->st_value); // TODO: make work with entry point?

  // ARGC, ARGV
  stack[0] = 1;
  stack[1] = (intptr_t)"libc.so";
  stack[2] = 0;

  // ENV
  stack[3] = (intptr_t)"LANG=C";
  stack[4] = 0;

  // AUXV
  stack[5] = AT_BASE; stack[6] = (intptr_t)testbuf; // TODO: maybe let musl calculate?
  stack[7] = AT_NULL; stack[8] = 0;

  // Dynamic linker info:
  while (phdr->p_type != PT_DYNAMIC) phdr++;
  dynv = (void *)(testbuf + phdr->p_vaddr);

  printf("Jumping to %p in %s loaded at %p\n", e_entry, argv[1], testbuf);
  e_entry(stack, dynv);

  return 0;
}
