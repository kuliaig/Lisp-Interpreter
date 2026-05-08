#include <string.h>
#include <stdlib.h>
#include "object.h"

void new_point(lisp_object* obj)
{
    if (obj != NULL)
    {
        obj->point_count++;
    }
}

void del_point(lisp_object* obj)
{
    if (obj == NULL)
    {
        return;
    }
    obj->point_count--;
    if (obj->point_count <= 0)
    {
        if (obj->type == LISP_SYMB || obj->type == LISP_STR)
        {
            free(obj->data.str.chars);
        }
        else if (obj->type == LISP_CONS)
        {
            del_point(obj->data.cons.car);
            del_point(obj->data.cons.cdr);
        }
        else if (obj->type == LISP_ARR)
        {
            for (int i = 0; i < obj->data.arr.size; i++) {
                    del_point(obj->data.arr.elements[i]);
                }
                free(obj->data.arr.elements);
        }

        free(obj);
    }
}

lisp_object* create_num(const long long value)
{
    lisp_object* new = malloc(sizeof(lisp_object));
    if (new == NULL) 
    {
        return NULL;
    }
    new->type = LISP_NUM;
    new->data.num_val = value;
    new->point_count = 1;
    return new;
}

lisp_object* create_symb(const char* name)
{
    lisp_object* new = malloc(sizeof(lisp_object));
    if (new == NULL) 
    {
        return NULL;
    }
    new->type = LISP_SYMB;
    new->data.str.length = strlen(name);
    new->data.str.chars = malloc(new->data.str.length + 1);
    if (new->data.str.chars == NULL) 
    {
        free(new);
        return NULL;
    }
    strcpy(new->data.str.chars, name);
    new->point_count = 1;
    return new;
}

lisp_object* create_nil()
{
    static lisp_object* nil = NULL;
    if (nil == NULL) {
        nil = malloc(sizeof(lisp_object));
        if (nil == NULL) 
        {
            return NULL;
        }
        nil->type = LISP_NIL;
        nil->point_count = 1;
    }
    new_point(nil);
    return nil;
}

lisp_object* create_str(const char* str)
{
    lisp_object* new = malloc(sizeof(lisp_object));
    if (new == NULL) 
    {
        return NULL;
    }
    new->type = LISP_STR;
    new->data.str.length = strlen(str);
    new->data.str.chars = malloc(new->data.str.length + 1);
    if (new->data.str.chars == NULL) 
    {
        free(new);
        return NULL;
    }
    strcpy(new->data.str.chars, str);
    new->point_count = 1;
    return new;
}

lisp_object* create_bool(const int value)
{
    lisp_object* new = malloc(sizeof(lisp_object));
    if (new == NULL) 
    {
        return NULL;
    }
    new->type = LISP_BOOL;
    new->data.bool_val = value;
    new->point_count = 1;
    return new;
}

lisp_object* create_cons(lisp_object* car, lisp_object* cdr)
{
    lisp_object* new = malloc(sizeof(lisp_object));
    if (new == NULL) 
    {
        return NULL;
    }
    new->type = LISP_CONS;
    new->point_count = 1;
    new_point(car);
    new_point(cdr);
    new->data.cons.car = car;
    new->data.cons.cdr = cdr;

    return new;
}

lisp_object* create_arr()
{
    lisp_object* new = malloc(sizeof(lisp_object));
    if (new == NULL) 
    {
        return NULL;
    }
    new->type = LISP_ARR;
    new->data.arr.capacity = 8;
    new->data.arr.size = 0;
    new->data.arr.elements = malloc(sizeof(lisp_object*) * 8);
    if (!new->data.arr.elements) {
        free(new);
        return NULL;
    }
    new->point_count = 1;
    return new;
}

lisp_object* create_inside(lisp_object* (*inside_func)(lisp_object* args), const char* name)
{
    lisp_object* new = malloc(sizeof(lisp_object));
    if (new == NULL)
    {
        return NULL;
    }
    new->type = LISP_FUNC;
    new->data.func.ftype = FUNC_INSIDE;
    new->data.func.inside.name = malloc(strlen(name) + 1);
    if (new->data.func.inside.name == NULL)
    {
        return NULL;
    }
    strcpy(new->data.func.inside.name, name);
    new->data.func.inside.inside_func = inside_func;
    new->point_count = 1;
    return new;
}

lisp_object* create_user(lisp_object* args, lisp_object* body, struct Hash* table)
{
    lisp_object* new = malloc(sizeof(lisp_object));
    if (new == NULL)
    {
        return NULL;
    }
    new->type = LISP_FUNC;
    new->data.func.ftype = FUNC_USER;
    new->data.func.user.args = args;
    new_point(args);
    new->data.func.user.body = body;
    new_point(body);
    new->data.func.user.table = table;
    new->point_count = 1;
    return new;
}

lisp_object* create_void()
{
    static lisp_object* nil = NULL;
    if (nil == NULL) {
        nil = malloc(sizeof(lisp_object));
        if (nil == NULL)
        {
            return NULL;
        }
        nil->type = LISP_VOID;
        nil->point_count = 1;
    }
    new_point(nil);
    return nil;
}
