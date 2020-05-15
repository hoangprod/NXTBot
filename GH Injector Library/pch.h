#pragma once

//winapi shit
#include <Windows.h>

//enum shit
#include <TlHelp32.h>
#include <Psapi.h>

//string shit
#include <strsafe.h>
#include <tchar.h>

//file shit
#include <fstream>

//dank shit
#include <vector>
#include <ctime>

//session shit
#include <wtsapi32.h>
#include <iostream>
#include <random>

#pragma warning(disable: 6001) //uninitialized memory (bug with SAL notation)
#pragma warning(disable: 6031) //ignored return value warning
#pragma warning(disable: 6258) //TerminateThread warning

// Second by 100 ns
#define _SECOND (10000000)
#define _MINUTE (60 * _SECOND)
#define _HOUR   (60 * _MINUTE)
#define _DAY    (24 * _HOUR)

// Standard Seconds
#define SECOND_ (1)
#define MINUTE_ (60 * SECOND_)
#define HOUR_   (60 * MINUTE_)
#define DAY_    (24 * HOUR_)