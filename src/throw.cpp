#include <string>
#include <stdexcept>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include "throw.h"
#include "platform.h"

void doThrow(const char *file, int line, const char *fmt, ...)
{
	char buf[128];
	snprintf(buf, sizeof(buf), "%s:%d: ", file, line);

	va_list ap;
	va_start(ap, fmt);

#ifdef USE_VASPRINTF
	char *p;
	if(vasprintf(&p, fmt, ap) == -1) {
		throw std::runtime_error("Memory allocation error (throw)");
	}

	va_end(ap);

	const std::string s(std::string(buf) + p);
	free(p);
#else
	/* MinGW doesn't have vasprintf... */
	char buf2[4096];
	if(vsnprintf(buf2, sizeof(buf2), fmt, ap) < 0) {
		throw std::runtime_error("vsnprintf() error when throwing an exception");
	}

	const std::string s(std::string(buf) + buf2);
#endif

	throw std::runtime_error(s);
}
