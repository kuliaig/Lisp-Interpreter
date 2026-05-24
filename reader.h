#pragma once
#include "object.h"
#include "hash.h"
#include "array.h"

// return terminal mode to usual
void mode();

// read the information that the user enters 
// and auto-completes when the tab key is pressed
char* read_input(Hash* table);