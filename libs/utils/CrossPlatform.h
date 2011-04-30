#ifndef CROSS_PLATFORM_H
#define CROSS_PLATFORM_H

#if defined _WIN32 || defined _WIN64
#define NOMINMAX
#include <windows.h>
#endif

#endif
