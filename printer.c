#include "printer.h"
#include <stdio.h>

static void print_num(lisp_object* obj)
{
    printf("%lld", obj->data.num_val);
}

static void print_symb(lisp_object* obj)
{
    printf("%s", obj->data.str.chars);
}

static void print_nil()
{
    printf("()");
}

static void print_bool(lisp_object* obj)
{
    if (obj->data.bool_val == 1)
    {
        printf("#t");
    }
    else
    {
        printf("#f");
    }
}

static void print_cons(lisp_object* obj)
{
    printf("(");
    
    while (1) {
        print_object(obj->data.cons.car);
        
        lisp_object* cdr = obj->data.cons.cdr;
        
        if (cdr->type == LISP_NIL) {
            break;
        }
        else if (cdr->type == LISP_CONS) {
            printf(" ");
            obj = cdr;
        }
        else {
            printf(" . ");
            print_object(cdr);
            break;
        }
    }
    printf(")");
}

static void print_arr(lisp_object* obj)
{
    printf("#(");
    int len = obj->data.arr.size;
    if (len > 0)
    {
        print_object(obj->data.arr.elements[0]);
        for (int i = 1; i < len; i++)
        {
            printf(" ");
            print_object(obj->data.arr.elements[i]);
        }
    }
    printf(")");
}

static void print_str(lisp_object* obj)
{
    printf("\"%s\"", obj->data.str.chars);
}

static void print_func(lisp_object* obj)
{
    if (obj->data.func.ftype == FUNC_INSIDE)
    {
        printf("#<procedure:%s>", obj->data.func.inside.name);
    }
    else
    {
        printf("#<procedure>");
    }
}

void print_object(lisp_object* obj)
{
    switch (obj->type)
    {
    case LISP_NUM:
        print_num(obj);
        break;

    case LISP_SYMB:
        print_symb(obj);
        break;
    
    case LISP_STR:
        print_str(obj);
        break;
    
    case LISP_NIL:
        print_nil();
        break;
    
    case LISP_BOOL:
        print_bool(obj);
        break;
    
    case LISP_CONS:
        print_cons(obj);
        break;
    
    case LISP_ARR:
        print_arr(obj);
        break;

    case LISP_FUNC:
        print_func(obj);
        break;

    case LISP_VOID:
        break;
    }
}