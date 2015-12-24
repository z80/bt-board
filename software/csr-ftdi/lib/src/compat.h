#ifndef _COMPAT_H
#define _COMPAT_H

#include "stdint.h"

#ifdef __WINE__
# define stricmp strcasecmp
#endif

#ifdef COMPAT_TIMER_MACROS
/* MinGW has timerisset(), timercmp(), timerclear() defined, but not timeradd()
 * and timersub(). */

/* From glibc's sys/time.h */
/* Convenience macros for operations on timevals.
   NOTE: `timercmp' does not work for >= or <=.  */
#ifndef timerisset
# define timerisset(tvp)    ((tvp)->tv_sec || (tvp)->tv_usec)
#endif
#ifndef timerclear
# define timerclear(tvp)    ((tvp)->tv_sec = (tvp)->tv_usec = 0)
#endif
#ifndef timercmp
# define timercmp(a, b, CMP)                              \
  (((a)->tv_sec == (b)->tv_sec) ?                         \
   ((a)->tv_usec CMP (b)->tv_usec) :                          \
   ((a)->tv_sec CMP (b)->tv_sec))
#endif
#ifndef timeradd
# define timeradd(a, b, result)                           \
  do {                                        \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                 \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                  \
    if ((result)->tv_usec >= 1000000)                         \
      {                                       \
    ++(result)->tv_sec;                           \
    (result)->tv_usec -= 1000000;                         \
      }                                       \
  } while (0)
#endif
#ifndef timersub
# define timersub(a, b, result)                           \
  do {                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                 \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                  \
    if ((result)->tv_usec < 0) {                          \
      --(result)->tv_sec;                             \
      (result)->tv_usec += 1000000;                       \
    }                                         \
  } while (0)
#endif
#endif /* COMPAT_TIMER_MACROS */


//#ifdef WIN32
  #ifndef TIMEVAL
    #define TIMEVAL
    /*
    typedef struct timeval {
      long tv_sec;
      long tv_usec;
    } timeval;*/

    typedef unsigned long ssize_t;


    int gettimeofday(struct timeval * tp, struct timezone * tzp);
  #endif

 /// #if defined(_MSC_VER) && _MSC_VER < 1900

    #define snprintf c99_snprintf
    #define vsnprintf c99_vsnprintf

    #ifdef __cplusplus
        extern "C"
        {
    #endif
            int c99_vsnprintf(char *outBuf, size_t size, const char *format, va_list ap);
            int c99_snprintf(char *outBuf, size_t size, const char *format, ...);
    #ifdef __cplusplus
        }
    #endif

  //#endif

#endif /* _COMPAT_H */
