#pragma once

#include <cpprelude/defines.h>

#ifdef OS_WINDOWS
#ifdef RGX_DLL
    #define API_RGX __declspec(dllexport)
#else
	#define API_RGX __declspec(dllimport)
#endif
#endif

#ifdef OS_LINUX
    #define API_RGX
#endif