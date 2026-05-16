#pragma once
#include "object.h"

// add mathematical functions to table (+-*//<>=)
void module_math(Hash* table);

// add cons functions to table (car, cdr, cons, caar, cadr, cdar, cddr)
void module_cons(Hash* table);

// add check functions to table (null? pair? number? symbol? 
// boolean? string? procedure? list? vector? zero? positive? negative?)
void module_check(Hash* table);

// add logical functions to table (not, and, or)
void module_logic(Hash* table);