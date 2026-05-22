#include <stdio.h>
#include <stdlib.h>
#include "array.h"

int arr_length(lisp_object* arr)
{
    if (arr != NULL && arr->type == LISP_ARR)
    {
        return arr->data.arr.size;
    }
    return -1;
}

int arr_enlarge(lisp_object* arr, int new_capacity)
{
    if (arr == NULL || arr->type != LISP_ARR || new_capacity <= arr->data.arr.capacity)
    {
        return 1;
    }
    lisp_object** new_elements = realloc(arr->data.arr.elements, sizeof(lisp_object*) * new_capacity);
    if (new_elements == NULL)
    {
        return 0;
    }

    arr->data.arr.elements = new_elements;
    arr->data.arr.capacity = new_capacity;
    return 1;
}

int arr_add(lisp_object* arr, lisp_object* obj)
{
    if (obj == NULL || arr == NULL || arr->type != LISP_ARR)
    {
        return 0;
    }
    if (arr->data.arr.size >= arr->data.arr.capacity)
    {
        int res = arr_enlarge(arr, arr->data.arr.capacity * 2 + 1);
        if (res == 0)
        {
            return 0;
        }
    }
    arr->data.arr.elements[arr->data.arr.size++] = obj;
    new_point(obj);
    return 1;
}

int arr_set(lisp_object* arr, int ind, lisp_object* obj)
{
    if (obj == NULL || arr == NULL || arr->type != LISP_ARR || ind >= arr->data.arr.size || ind < 0)
    {
        return 0;
    }
    del_point(arr->data.arr.elements[ind]);
    arr->data.arr.elements[ind] = obj;
    new_point(obj);
    return 1;
}

lisp_object* arr_get(lisp_object* arr, int ind)
{
    if (arr == NULL || arr->type != LISP_ARR || ind >= arr->data.arr.size || ind < 0)
    {
        return NULL;
    }
    lisp_object* obj = arr->data.arr.elements[ind];
    if (obj == NULL)
    {
        return NULL;
    }
    new_point(obj);
    return obj;
}
