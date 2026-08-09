## The rvsc targets are RISC-V subsets whose missing instructions are
## synthesized by the machine description, which hand-written assembly never
## passes through.  So this directory deliberately omits the RISC-V machine
## memset/memcpy/memmove/strcmp -- the generic C ones in libc/string are used
## instead -- and keeps only setjmp.S, which has no generic C equivalent.
libc_a_SOURCES += \
	%D%/setjmp.S %D%/ieeefp.c %D%/ffs.c
