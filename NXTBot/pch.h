// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <new>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <strsafe.h>
#include <vector>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <detours.h>
#include <string>
#include <mutex>
#include <fstream>
#include <set>
#include <random>>
#include <iterator>
#include <cmath>
#include <windowsx.h>

#include "glew/GL/glew.h"
#include <gl\glut.h>

#include "json.hpp"
#include "Exception.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif
#endif //PCH_H



#ifdef NDEBUG
#define printf(fmt, ...) (0)
#define wprintf(fmt, ...) (0)
#endif