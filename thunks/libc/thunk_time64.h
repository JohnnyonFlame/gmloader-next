/*

Copyright (c) 2007-2008  Michael G Schwern

This software originally derived from Paul Sheer's pivotal_gmtime_r.c.

The MIT License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Origin: http://code.google.com/p/y2038
Modified for Bionic by the Android Open Source Project

*/
#ifndef TIME64_H
#define TIME64_H

//We want the cruft
#if 0//defined(__LP64__)

#error Your time_t is already 64-bit.

#else

/* Legacy cruft for LP32 where time_t was 32-bit. */

#include <sys/cdefs.h>
#include <time.h>
#include <stdint.h>

__BEGIN_DECLS

typedef int64_t time64_t;

ABI_ATTR char* asctime64_impl(const struct tm*);
ABI_ATTR char* asctime64_r_impl(const struct tm*, char*);
ABI_ATTR char* ctime64_impl(const time64_t*);
ABI_ATTR char* ctime64_r_impl(const time64_t*, char*);
ABI_ATTR struct tm* gmtime64_impl(const time64_t*);
ABI_ATTR struct tm* gmtime64_r(const time64_t*, struct tm*);
ABI_ATTR struct tm* localtime64_impl(const time64_t*);
ABI_ATTR struct tm* localtime64_r_impl(const time64_t*, struct tm*);
ABI_ATTR time64_t mktime64_impl(const struct tm*);
ABI_ATTR time64_t timegm64_impl(const struct tm*);
ABI_ATTR time64_t timelocal64_impl(const struct tm*);

__END_DECLS

#endif

#endif /* TIME64_H */
