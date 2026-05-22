#pragma once
#include "object.h"
#include "hash.h"

// please add #include "library.h" to your library

// in Linux functions are already visible
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else 
#define EXPORT
#endif

// plugin must be:
// EXP plugin* init_plugin(void);
// returns array of {name, func} pairs
// ends by {NULL, NULL}
// you can find an example in arraylib.c
typedef struct plugin
{
	const char* name;
	lisp_object* (*func)(lisp_object* args);
} plugin;