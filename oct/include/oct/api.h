#pragma once

#include <cpprelude/defines.h>

#ifdef OS_WINDOWS
#ifdef OCT_DLL
	#define API_OCT __declspec(dllexport)
#else
	#define API_OCT __declspec(dllimport)
#endif
#endif

#ifdef OS_LINUX
	#define API_OCT
#endif