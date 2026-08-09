/* Copyright (c) 2017  SiFive Inc. All rights reserved.

   This copyrighted material is made available to anyone wishing to use,
   modify, copy, or redistribute it subject to the terms and conditions
   of the FreeBSD License.   This program is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
   including the implied warranties of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  A copy of this license is available at
   http://www.opensource.org/licenses.
*/

#ifndef _INTERNAL_SYSCALL_H
#define _INTERNAL_SYSCALL_H

#include <errno.h>
/* For the O_* values __syscall_open_flags translates, below.  This header is
   included by every libgloss/riscv syscall stub, most of which do not open
   files, so it has to bring its own declarations rather than rely on theirs.  */
#include <fcntl.h>

static inline long
__syscall_error(long a0)
{
  errno = -a0;
  return -1;
}

static inline long
__internal_syscall(long n, int argc, long _a0, long _a1, long _a2, long _a3, long _a4, long _a5)
{
#ifdef __riscv_32e
  register long syscall_id asm("t0") = n;
#else
  register long syscall_id asm("a7") = n;
#endif

  register long a0 asm("a0") = _a0;
  if (argc < 2) {
      asm volatile ("ecall" : "+r"(a0) : "r"(syscall_id));
      return a0;
  }
  register long a1 asm("a1") = _a1;
  if (argc == 2) {
      asm volatile ("ecall" : "+r"(a0) : "r"(a1), "r"(syscall_id));
      return a0;
  }
  register long a2 asm("a2") = _a2;
  if (argc == 3) {
      asm volatile ("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(syscall_id));
      return a0;
  }
  register long a3 asm("a3") = _a3;
  if (argc == 4) {
      asm volatile ("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(syscall_id));
      return a0;
  }
  register long a4 asm("a4") = _a4;
  if (argc == 5) {
      asm volatile ("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(syscall_id));
      return a0;
  }
  register long a5 asm("a5") = _a5;

  asm volatile ("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(syscall_id));

  return a0;
}

static inline long
_syscall_errno(long n, int argc, long _a0, long _a1, long _a2, long _a3, long _a4, long _a5)
{
  long a0 = __internal_syscall (n, argc, _a0, _a1, _a2, _a3, _a4, _a5);

  if (a0 < 0)
    return __syscall_error (a0);
  else
    return a0;
}

#define syscall_errno(N, ARGC, A0, A1, A2, A3, A4, A5) \
  _syscall_errno(N, ARGC, (long)A0, (long)A1, (long)A2, \
	         (long)A3, (long)A4, (long)A5)

/* Translate newlib's open flags to the ones the syscall interface expects.

   These syscalls go to a RISC-V Linux ABI implementation -- the proxy kernel,
   or a simulator's frontend server -- which decodes the flag word with the
   asm-generic values.  newlib's own O_* are the older BSD ones and do not
   agree: newlib's O_CREAT is 0x200, which the other side reads as O_TRUNC,
   and newlib's O_TRUNC is 0x400, read as O_APPEND.  Passing the word through
   unconverted therefore turns fopen(path, "w") into an open with neither
   O_CREAT nor O_TRUNC, which fails with ENOENT on any file that does not
   already exist.  Convert bit by bit rather than passing through, so a flag
   with no counterpart is dropped instead of silently meaning something else.  */

#define __LINUX_O_CREAT		000000100
#define __LINUX_O_EXCL		000000200
#define __LINUX_O_NOCTTY	000000400
#define __LINUX_O_TRUNC		000001000
#define __LINUX_O_APPEND	000002000
#define __LINUX_O_NONBLOCK	000004000
#define __LINUX_O_SYNC		000010000
#define __LINUX_O_DIRECTORY	000200000
#define __LINUX_O_NOFOLLOW	000400000
#define __LINUX_O_CLOEXEC	002000000

static inline long
__syscall_open_flags (long flags)
{
  /* O_RDONLY/O_WRONLY/O_RDWR are 0/1/2 in both encodings.  */
  long out = flags & O_ACCMODE;

#define __XLAT_O(NEWLIB, LINUX) \
  do { if (flags & (NEWLIB)) out |= (LINUX); } while (0)
  __XLAT_O (O_CREAT,     __LINUX_O_CREAT);
  __XLAT_O (O_EXCL,      __LINUX_O_EXCL);
  __XLAT_O (O_NOCTTY,    __LINUX_O_NOCTTY);
  __XLAT_O (O_TRUNC,     __LINUX_O_TRUNC);
  __XLAT_O (O_APPEND,    __LINUX_O_APPEND);
  __XLAT_O (O_NONBLOCK,  __LINUX_O_NONBLOCK);
  __XLAT_O (O_SYNC,      __LINUX_O_SYNC);
  __XLAT_O (O_DIRECTORY, __LINUX_O_DIRECTORY);
  __XLAT_O (O_NOFOLLOW,  __LINUX_O_NOFOLLOW);
  __XLAT_O (O_CLOEXEC,   __LINUX_O_CLOEXEC);
#undef __XLAT_O

  return out;
}

#endif
