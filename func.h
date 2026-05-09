#pragma once
#include "object.h"

// add mathematical functions to table (+-*//<>=)
void module_math(Hash* table);

// add cons functions to table (car, cdr, cons, caar, cadr, cdar, cddr)
void module_cons(Hash* table);