#pragma once
#include "object.h"

// return the length of arr or -1 if obj is not arr
int arr_length(lisp_object* arr);

// add obj to arr and return 0 if error 
int arr_add(lisp_object* arr, lisp_object* obj);

// set arr[ind] to obj and return 0 if error 
int arr_set(lisp_object* arr, int ind, lisp_object* obj);

// return arr[ind] and NULL if error
// !!! don't forget to do del_point() if you don't use obj any more
lisp_object* arr_get(lisp_object* arr, int ind);