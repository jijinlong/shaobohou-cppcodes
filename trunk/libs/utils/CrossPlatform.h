#ifndef CROSS_PLATFORM_H
#define CROSS_PLATFORM_H

#if defined _WIN32 || defined _WIN64
#define NOMINMAX
#include <windows.h>
#endif

//return system time in seconds,
double getTime();
unsigned int getSeedFromTime();

#endif CROSS_PLATFORM_H
