#if defined (__x86_64__)
#pragma message("Building Argon2 with SIMD optimisations")
#include "opt.c"
#else
#prgma message("Building Argon2 without SIMD optimisations")
#include "ref.c"
#endif
