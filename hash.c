#include "storage.h"
#include <stdio.h>

static int

// create new Hash table - environment 
Hash* create_Hash(Hash* Parent);

// put new lisp_object to hash
void put_Hash(Hash* table, const char* name, lisp_object* obj);

// set new value to object from Hash
void set_Hash(Hash* table, const char* name, lisp_object* obj);

// get lisp_object from hash
lisp_object* get_Hash(Hash* table, const char* name);

// delete Hash table
void del_Hash(Hash* table);