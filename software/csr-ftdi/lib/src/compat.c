
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "compat.h"

#ifdef COMPAT_MINGW_MS_VSNPRINTF
/*
 * This fixes linking with precompiled libusb-1.0.18-win and
 * libusb-1.0.19-rc1-win: "undefined reference to __ms_vsnprintf". See:
 * http://sourceforge.net/p/mingw-w64/mailman/mingw-w64-public/thread/4F8CA26A.70103@users.sourceforge.net/
 */

#include <stdio.h>
#include <stdarg.h>

int __cdecl __ms_vsnprintf(char * __restrict__ d,size_t n,const char * __restrict__ format,va_list arg)
{
    return vsnprintf(d, n, format, arg);
}
#endif /* COMPAT_MINGW_MS_VSNPRINTF */

    int gettimeofday(struct timeval * tp, struct timezone * tzp)
    {
        // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
        static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

        SYSTEMTIME  system_time;
        FILETIME    file_time;
        uint64_t    time;

        GetSystemTime( &system_time );
        SystemTimeToFileTime( &system_time, &file_time );
        time =  ((uint64_t)file_time.dwLowDateTime )      ;
        time += ((uint64_t)file_time.dwHighDateTime) << 32;

        tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
        tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
        return 0;
    }

    int c99_vsnprintf(char *outBuf, size_t size, const char *format, va_list ap)
    {
        int count = -1;

        if (size != 0)
            count = _vsnprintf_s(outBuf, size, _TRUNCATE, format, ap);
        if (count == -1)
            count = _vscprintf(format, ap);

        return count;
    }

    int c99_snprintf(char *outBuf, size_t size, const char *format, ...)
    {
        int count;
        va_list ap;

        va_start(ap, format);
        count = c99_vsnprintf(outBuf, size, format, ap);
        va_end(ap);

        return count;
    }

