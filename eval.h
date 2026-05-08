#pragma once
#include "object.h"
#include "hash.h"

// calculate the expr in environment table
lisp_object* eval(lisp_object* expr, Hash* table);