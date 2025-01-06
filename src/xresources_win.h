#pragma once

#ifdef WITH_X

#include <memory>
#include <vector>
#include "xwindow_win.h"

struct XResources {
	std::unique_ptr<XWindow> oscilloscope;
	std::unique_ptr<XWindow> spectrum;
	std::unique_ptr<XWindow> constellation;
};

#endif
