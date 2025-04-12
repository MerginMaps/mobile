#ifndef CPL_BASE_H_INCLUDED_TWEAKED
#define CPL_BASE_H_INCLUDED_TWEAKED

#if defined(__GNUC__) && __GNUC__ >= 4
/** Qualifier for an argument that is unused */
#  define CPL_UNUSED __attribute((__unused__))
#else
/* TODO: add cases for other compilers */
/** Qualifier for an argument that is unused */
#  define CPL_UNUSED
#endif

#endif /* ndef CPL_BASE_H_INCLUDED_TWEAKED */