/* ffs has no generic C implementation in libc/string, so the rvsc machine
   directory has to supply one.  The RISC-V version is plain C and compiles
   through the machine description like any other source, so reuse it.  */

#include "../riscv/ffs.c"
