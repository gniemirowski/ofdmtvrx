#pragma once

#include "platform.h"

#ifdef USE_SIGFD

#include <csignal>

class SigFD {
public:
	SigFD();
	~SigFD();

	int getFD() const;

	/* Returns signal number or 0 */
	int readHandler();

private:
	sigset_t m_sigset;
	int m_fd{-1};
};

#endif
