#pragma once 
#include "object.h"

// skip spaces in str
void skip_spaces(const char** p);

// get the first lisp_object from expression p, the place of next lisp_object in remaind
lisp_object* parse_object(const char* p, const char** remaind);