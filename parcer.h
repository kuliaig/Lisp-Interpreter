#pragma once 
#include "object.h"
#include "array.h"

// get the first lisp_object from expression p, the place of next lisp_object in remaind
lisp_object* parse_object(const char* p, const char** remaind);

// get the list of lisp_objects in p
lisp_object* parse_expr(const char *p);