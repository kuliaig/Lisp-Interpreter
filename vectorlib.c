#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include "array.h"
#include "object.h"

static lisp_object* cons_third(lisp_object* cons)
{
    if (cons == NULL || cons->type != LISP_CONS)
    {
        return NULL;
    }

    lisp_object* cdr = cons->data.cons.cdr;
    if (cdr == NULL || cdr->type != LISP_CONS)
    {
        return NULL;
    }

    lisp_object* th = cdr->data.cons.cdr;
    if (th == NULL)
    {
        return NULL;
    }

    if (th->type == LISP_CONS)
    {
        return th->data.cons.car;
    }
    else if (th->type == LISP_NIL)
    {
        return NULL;
    }
    else
    {
        return th;
    }
}

static lisp_object* vector(lisp_object* args)
{
    if (args == NULL)
    {
        fprintf(stderr, "ERROR: vector needs correct args :(\n");
        return NULL;
    }

    lisp_object* vec = create_arr();
    lisp_object* cur = args;

    while (cur != NULL && cur->type == LISP_CONS)
    {
        lisp_object* arg = cur->data.cons.car;
        if (arg == NULL)
        {
            fprintf(stderr, "ERROR: vector needs correct args :(\n");
            return NULL;
        }

        cur = cur->data.cons.cdr;
        int res = arr_add(vec, arg);

        if (res == 0)
        {
            fprintf(stderr, "ERROR: can't add new arg to vector");
            return NULL;
        }
    }

    return vec;
}

static lisp_object* vector_ref(lisp_object* args)
{
    if (args == NULL || args->type != LISP_CONS)
    {
        fprintf(stderr, "ERROR: vector-ref needs correct args :(\n");
        return NULL;
    }

    lisp_object* vec = args->data.cons.car;
    lisp_object* temp = args->data.cons.cdr;
    lisp_object* ost = cons_third(args);

    if (temp == NULL || vec == NULL || vec->type != LISP_ARR || temp->type != LISP_CONS || ost != NULL)
    {
        fprintf(stderr, "ERROR: vector-ref needs correct args :(\n");
        return NULL;
    }

    lisp_object* ind = temp->data.cons.car;
    if (ind->type != LISP_NUM)
    {
        fprintf(stderr, "ERROR: vector-ref needs correct args :(\n");
        return NULL;
    }

    lisp_object* res = arr_get(vec, ind->data.num_val);

    if (res == NULL)
    {
        fprintf(stderr, "ERROR: error in vector-ref :(\n");
        return NULL;
    }

    return res;
}

static lisp_object* vector_set(lisp_object* args)
{
    if (args == NULL || args->type != LISP_CONS)
    {
        fprintf(stderr, "ERROR: vector-set! needs correct args :(\n");
        return NULL;
    }

    lisp_object* vec = args->data.cons.car;
    lisp_object* temp = args->data.cons.cdr;

    if (temp == NULL || vec == NULL || vec->type != LISP_ARR || temp->type != LISP_CONS)
    {
        fprintf(stderr, "ERROR: vector-set! needs 3 correct args :(\n");
        return NULL;
    }

    lisp_object* ind = temp->data.cons.car;
    lisp_object* rest = temp->data.cons.cdr;
    lisp_object* obj = rest->data.cons.car;
    lisp_object* extra = rest->data.cons.cdr;

    if (vec == NULL || vec->type != LISP_ARR || obj == NULL || (extra != NULL && extra->type != LISP_NIL))
    {
        fprintf(stderr, "ERROR: vector-set! needs 3 correct args :(\n");
        return NULL;
    }

    if (ind->type != LISP_NUM)
    {
        fprintf(stderr, "ERROR: vector-set! needs number as an index :(\n");
        return NULL;
    }

    int res = arr_set(vec, ind->data.num_val, obj);

    if (res == 0)
    {
        fprintf(stderr, "ERROR: error in vector-set! :(\n");
        return NULL;
    }

    return create_void();
}

static lisp_object* vector_length(lisp_object* args)
{
    if (args == NULL || args->type != LISP_CONS)
    {
        fprintf(stderr, "ERROR: vector-length expects 1 argument :(\n");
        return NULL;
    }

    lisp_object* vec = args->data.cons.car;
    lisp_object* rest = args->data.cons.cdr;

    if (vec == NULL || vec->type != LISP_ARR || (rest != NULL && rest->type != LISP_NIL))
    {
        fprintf(stderr, "ERROR: vector-length expects 1 argument :(\n");
        return NULL;
    }

    long long res = (long long)arr_length(vec);

    if (res == -1)
    {
        fprintf(stderr, "ERROR: vector-length error :(\n");
        return NULL;
    }

    return create_num(res);
}

EXPORT plugin* init_plugin(void)
{
    static plugin funcs[] = 
    {
        {"vector", vector},
        {"vector-ref", vector_ref},
        {"vector-set!", vector_set},
        {"vector-length", vector_length},
        {NULL, NULL}
    };
    return funcs;
}