#pragma once
#include "object.h"

// Hash use the FNV-1a function

typedef struct Node 
{
    char* name;
    lisp_object* value;
    struct Node* next;
} Node;

typedef struct Hash
{
    Node** arr;
    int size;
    int capacity;
    struct Hash* parent;
} Hash;

// create new Hash table - environment 
Hash* create_Hash(Hash* Parent);

// put new lisp_object to hash, return 0 if error
int put_Hash(Hash* table, const char* name, lisp_object* obj);

// set new value to object from Hash, return 0 if error
int set_Hash(Hash* table, const char* name, lisp_object* obj);

// get lisp_object from hash
lisp_object* get_Hash(Hash* table, const char* name);

// delete Hash table
void del_Hash(Hash* table);