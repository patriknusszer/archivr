#ifndef dirmapper_h
#define dirmapper_h

#pragma once

#if (defined(WIN32) || defined(_WIN32) || defined (__WIN32__))
#include <windows.h>
#ifdef UNICODE
#include <locale>
#include <codecvt>
#endif
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

typedef struct SUBS SUBS;

SUBS* subsof(char* path);
char** subsdirs(SUBS* subs, int* dcnt);
char** subsfiles(SUBS* subs, int* fcnt);

#if (defined(WIN32) || defined(_WIN32) ||defined (__WIN32__))
    //Helper functions related to windows path
    char* toWindowsPath(char* path);
#endif
#endif
