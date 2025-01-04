#pragma once

#if defined(PLATFORM_POSIX)

/* SIGFD (and working X) needs FDWATCH */

#define USE_FDWATCH
#define USE_SIGFD
#define USE_VASPRINTF

#define SIZET_FMT      "%zu"
#define SSIZET_FMT     "%zd"
#define PATH_SEPARATOR "/"
#define WAV_OPEN_FLAGS (O_RDONLY | O_LARGEFILE)

#include <endian.h>

#elif defined(PLATFORM_WIN)

#define USE_LOCALTIME_S

/* MinGW doesn't know about %z */
#define SIZET_FMT      "%llu"
#define SSIZET_FMT     "%lld"
#define PATH_SEPARATOR "\\"
#define WAV_OPEN_FLAGS (O_RDONLY | O_BINARY)

#define le16toh(x) (x)
#define le32toh(x) (x)

#else

#error Refusing to build on unknown platform

#endif
