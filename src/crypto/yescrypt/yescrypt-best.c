#if defined (__x86_64__)
#include "yescrypt-simd.c"
#else
#include "yescrypt-opt.c"
#endif
