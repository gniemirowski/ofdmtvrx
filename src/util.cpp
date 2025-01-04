#include <ctime>
#include <cstdio>
#include <cstdarg>
#include <stdexcept>
#include "util.h"
#include "platform.h"

std::string util::getTS()
{
	const time_t t(time(NULL));

	struct tm tm;
#ifdef USE_LOCALTIME_S
	localtime_s(&tm, &t);
#else
	localtime_r(&t, &tm);
#endif

	char buf[64];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);

	return buf;
}

std::string util::format(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

#ifdef USE_VASPRINTF
	char *p;
	if(vasprintf(&p, fmt, ap) == -1) {
		throw std::runtime_error("Memory allocation error (format)");
	}

	va_end(ap);

	const std::string s(p);
	free(p);
#else
	/* MinGW doesn't have vasprintf... */
	char buf2[4096];
	if(vsnprintf(buf2, sizeof(buf2), fmt, ap) < 0) {
		throw std::runtime_error("vsnprintf() error when formatting a string");
	}

	const std::string s(buf2);
#endif

	return s;
}
