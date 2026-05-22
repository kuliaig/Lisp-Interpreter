#include "object.h"

// return the length of vector arr or -1 if error
int arr_length(lisp_object* arr);

// return 1 if ok 0 either, do the arr larger with new_capacity
int arr_enlarge(lisp_object* arr, int new_capacity);

// return 1 if ok 0 either, adds new obj to vector arr
int arr_add(lisp_object* arr, lisp_object* obj);

// return obj with index ind from vector arr or NULL if error
lisp_object* arr_get(lisp_object* arr, int ind);

// sets the vector arr[ind] to obj
// return 1 if ok 0 either
int arr_set(lisp_object* arr, int ind, lisp_object* obj);
