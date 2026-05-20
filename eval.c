#include "eval.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_DEPTH 5000

// function for memo-Hash
static char* args_to_string(lisp_object* args)
{
    size_t size = 256;
    size_t len = 0;
    char* buf = malloc(size);
    if (buf == NULL) return NULL;
    buf[0] = '\0';

    lisp_object* cur = args;
    while (cur != NULL && cur->type == LISP_CONS)
    {
        lisp_object* val = cur->data.cons.car;
        char* tmp = NULL;
        size_t needed = 0;

        if (val->type == LISP_NUM)
        {
            needed = snprintf(NULL, 0, "%lld ", val->data.num_val) + 1;
            tmp = malloc(needed);
            if (tmp)
            {
                snprintf(tmp, needed, "%lld ", val->data.num_val);
            }
        }
        else if (val->type == LISP_SYMB)
        {
            needed = snprintf(NULL, 0, "%s ", val->data.str.chars) + 1;
            tmp = malloc(needed);
            if (tmp)
            {
                snprintf(tmp, needed, "%s ", val->data.str.chars);
            }
        }
        else if (val->type == LISP_NIL)
        {
            tmp = strdup("() ");
        }
        else if (val->type == LISP_BOOL)
        {
            tmp = strdup(val->data.bool_val ? "#t " : "#f ");
        }
        else
        {
            needed = snprintf(NULL, 0, "%p ", (void*)val) + 1;
            tmp = malloc(needed);
            if (tmp)
            {
                snprintf(tmp, needed, "%p ", (void*)val);
            }
        }

        if (tmp == NULL) 
        { 
            free(buf); 
            return NULL; 
        }

        size_t tmp_len = strlen(tmp);
        while (len + tmp_len + 1 > size)
        {
            size *= 2;
            char* new_buf = realloc(buf, size);
            if (new_buf == NULL) 
            { 
                free(buf); 
                free(tmp); 
                return NULL; 
            }
            buf = new_buf;
        }

        strcat(buf, tmp);
        len += tmp_len;
        free(tmp);
        cur = cur->data.cons.cdr;
    }
    return buf;
}

static lisp_object* cons_sec(lisp_object* cons)
{
    if (cons == NULL || cons->type != LISP_CONS)
    {
        return NULL;
    }

    lisp_object* cdr = cons->data.cons.cdr;
    if (cdr == NULL)
    {
        return NULL;
    }

    if (cdr->type == LISP_CONS)
    {
        return cdr->data.cons.car;
    }
    else if (cdr->type == LISP_NIL)
    {
        return NULL;
    }
    else
    {
        return cdr;
    }
}

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

static lisp_object* quote(lisp_object* args)
{
    if (args == NULL)
    {
        fprintf(stderr, "ERROR: bad syntax, quote takes 1 argument :(\n");
        return NULL;
    }

    lisp_object* first = args->data.cons.car;
    lisp_object* second = args->data.cons.cdr;

    if (second != NULL && second->type != LISP_NIL)
    {
        fprintf(stderr, "ERROR: bad syntax, quote takes 1 argument :(");
        return NULL;
    }

    return first;
}

static lisp_object* ifelse(lisp_object* args, Hash* table)
{
    if (args == NULL || args->type != LISP_CONS)
    {
        fprintf(stderr, "ERROR: bad syntax, if takes 3 arguments :(\n");
        return NULL;
    }

    lisp_object* cond = args->data.cons.car;
    lisp_object* ways = args->data.cons.cdr;

    if (ways == NULL || args->type != LISP_CONS)
    {
        fprintf(stderr, "ERROR: bad syntax, if takes at least 2 arguments :(\n");
        return NULL;
    }

    lisp_object* first = ways->data.cons.car;
    lisp_object* second = cons_sec(ways);
    lisp_object* third = cons_third(ways);

    if (first == NULL || third != NULL)
    {
        fprintf(stderr, "ERROR: bad syntax, if takes 2 or 3 arguments :(\n");
        return NULL;
    }

    lisp_object* res = eval(cond, table);

    if (res == NULL)
    {
        return NULL;
    }

    if (res->type == LISP_NIL || (res->type == LISP_BOOL && res->data.bool_val == 0))
    {
        if (second == NULL)
        {
            return create_void();
        }
        return eval(second, table);
    }
    else
    {
        return eval(first, table);
    }
}

static lisp_object* define_var(lisp_object* var, lisp_object* value, lisp_object* third, Hash* table)
{
    if (var == NULL || value == NULL || third != NULL)
    {
        fprintf(stderr, "ERROR: bad syntax, define takes 2 arguments :(\n");
        return NULL;
    }
    if (var->type != LISP_SYMB)
    {
        fprintf(stderr, "ERROR: bad syntax, first argument is not a symbol :(\n");
        return NULL;
    }

    int res = put_Hash(table, var->data.str.chars, value);
    if (res == 0)
    {
        fprintf(stderr, "ERROR: not enough memory to define new var :(\n");
        return NULL;
    }

    return create_void();
}

static lisp_object* define_func(lisp_object* func, lisp_object* value, Hash* table, int ismemo)
{
    if (func == NULL || value == NULL)
    {
        fprintf(stderr, "ERROR: bad syntax, define takes 2 arguments :(\n");
        return NULL;
    }

    if (func->type != LISP_CONS || func->data.cons.car == NULL)
    {
        fprintf(stderr, "ERROR: bad syntax, first argument is not a function :(");
        return NULL;
    }

    lisp_object* f = func->data.cons.car;
    if (f->type != LISP_SYMB)
    {
        fprintf(stderr, "ERROR: bad syntax, first argument is not a function :(");
        return NULL;
    }

    lisp_object* var = func->data.cons.cdr;
    while (var != NULL && var->type == LISP_CONS)
    {
        if (var->data.cons.car->type != LISP_SYMB)
        {
            fprintf(stderr, "ERROR: bad syntax, parametres must be nill or symbols :(\n");
            return NULL;
        }
        var = var->data.cons.cdr;
    }
    if (var != NULL && var->type != LISP_NIL)
    {
        fprintf(stderr, "ERROR: bad syntax :(\n");
        return NULL;
    }

    lisp_object* params = func->data.cons.cdr;
    lisp_object* lam = create_user(params, value, table, ismemo);
    if (lam == NULL)
    {
        fprintf(stderr, "ERROR: not enough memory to define new function :(\n");
        return NULL;
    }

    int res = put_Hash(table, f->data.str.chars, lam);
    if (res == 0)
    {
        fprintf(stderr, "ERROR: not enough memory to define new function :(\n");
        return NULL;
    }
    return create_void();
}

static lisp_object* lambda(lisp_object* args, Hash* table, int ismemo)
{
    if (args == NULL || args->type != LISP_CONS)
    {
        fprintf(stderr, "ERROR: bad syntax, lambda takes at least 2 arguments :(\n");
        return NULL;
    }

    lisp_object* var = args->data.cons.car;
    lisp_object* body = args->data.cons.cdr;
    lisp_object* params = var;

    if (body == NULL || body->type == LISP_NIL)
    {
        fprintf(stderr, "ERROR: bad syntax, lambda takes at least 2 arguments :(\n");
        return NULL;
    }

    if (var != NULL && var->type == LISP_SYMB)
    {

    }
    else if (var != NULL && var->type == LISP_CONS)
    {
        while (var != NULL && var->type == LISP_CONS)
        {
            if (var->data.cons.car->type != LISP_SYMB)
            {
                fprintf(stderr, "ERROR: bad syntax, parametres must be nill or symbols :(\n");
                return NULL;
            }
            var = var->data.cons.cdr;
        }
        if (var != NULL && var->type != LISP_NIL)
        {
            fprintf(stderr, "ERROR: bad syntax :(\n");
            return NULL;
        }
    }
    else if (var != NULL && var->type != LISP_NIL)
    {
        fprintf(stderr, "ERROR: bad syntax, parametres must be nill or symbols :(\n");
        return NULL;
    }

    return create_user(params, body, table, ismemo);
}

static lisp_object* set(lisp_object* var, lisp_object* value, lisp_object* third, Hash* table)
{
    if (var == NULL || value == NULL || third != NULL)
    {
        fprintf(stderr, "ERROR: bad syntax, set! takes 2 arguments :(\n");
        return NULL;
    }

    if (var->type == LISP_SYMB)
    {
        int res = set_Hash(table, var->data.str.chars, value);
        if (res == 0)
        {
            fprintf(stderr, "ERROR: cannot set! %s before its definition :(\n", var->data.str.chars);
            return NULL;
        }
        return create_void();
    }

    fprintf(stderr, "ERROR: bad syntax, set! takes only variable as a first argument :(\n");
    return NULL;
}

static lisp_object* eval_args(lisp_object* args, Hash* table)
{
    if (args == NULL || args->type == LISP_NIL)
    {
        return create_nil();
    }

    lisp_object* first = eval(args->data.cons.car, table);
    if (first == NULL)
    {
        return NULL;
    }
    lisp_object* rest = eval_args(args->data.cons.cdr, table);
    if (rest == NULL)
    {
        del_point(first);
        return NULL;
    }

    return create_cons(first, rest);
}

static lisp_object* call_func(lisp_object* func, lisp_object* args, Hash* table)
{
    if (func == NULL)
    {
        fprintf(stderr, "ERROR: function is NULL :(\n");
    }
    lisp_object* ev = eval_args(args, table);
    if (ev == NULL)
    {
        return NULL;
    }
    if (func->data.func.ftype == FUNC_INSIDE)
    {
        return func->data.func.inside.inside_func(ev);
    }

    lisp_object* params = func->data.func.user.args;
    lisp_object* body = func->data.func.user.body;
    Hash* new_table = create_Hash(func->data.func.user.table);

    lisp_object* p = params;
    lisp_object* a = ev;
    while (p != NULL && p->type == LISP_CONS && a != NULL && a->type == LISP_CONS)
    {
        put_Hash(new_table, p->data.cons.car->data.str.chars, a->data.cons.car);
        p = p->data.cons.cdr;
        a = a->data.cons.cdr;
    }

    if ((p != NULL && p->type != LISP_NIL) || (a != NULL && a->type != LISP_NIL))
    {
        fprintf(stderr, "ERROR: wrong number of arguments :(\n");
        del_point_Hash(new_table);
        return NULL;
    }

    char* key = NULL;
    if (func->data.func.user.ismemo)
    {
        key = args_to_string(ev);
        lisp_object* cached = get_Hash(func->data.func.user.memo, key);
        if (cached != NULL)
        {
            free(key);
            del_point_Hash(new_table);
            return cached;
        }
    }

    lisp_object* res = NULL;
    lisp_object* cur = body;
    
    if (body->type == LISP_CONS && body->data.cons.car->type == LISP_SYMB)
    {
        res = eval(body, new_table);
    }
    else
    {
        while (cur != NULL && cur->type == LISP_CONS)
        {
            res = eval(cur->data.cons.car, new_table);
            if (res == NULL)
            {
                del_point_Hash(new_table);
                free(key);
                return NULL;
            }
            cur = cur->data.cons.cdr;
        }
    }

    if (func->data.func.user.ismemo && res != NULL)
    {
        int r = put_Hash(func->data.func.user.memo, key, res);
        if (r == 0)
        {
            fprintf(stderr, "ERROR: not enough memory to do memoization :(\n");
        }
    }
    free(key);

    del_point_Hash(new_table);
    return res;
}

static lisp_object* case_and(lisp_object* args, Hash* table)
{
    lisp_object* cur = args;
    lisp_object* res = create_bool(1);

    while (cur != NULL && cur->type == LISP_CONS)
    {
        res = eval(cur->data.cons.car, table);
        if (res == NULL) return NULL;

        if (res->type == LISP_BOOL && res->data.bool_val == 0)
            return create_bool(0);

        cur = cur->data.cons.cdr;
    }
    return res;
}

static lisp_object* case_or(lisp_object* args, Hash* table)
{
    lisp_object* cur = args;
    lisp_object* res = create_bool(0);

    while (cur != NULL && cur->type == LISP_CONS)
    {
        res = eval(cur->data.cons.car, table);
        if (res == NULL) return NULL;

        if (!(res->type == LISP_BOOL && res->data.bool_val == 0))
            return res;

        cur = cur->data.cons.cdr;
    }
    return res;
}

static lisp_object* case_map(lisp_object* args, Hash* table)
{
    lisp_object* func_expr = args->data.cons.car;
    lisp_object* list_expr = cons_sec(args);
    lisp_object* third = cons_third(args);

    if (func_expr == NULL || list_expr == NULL || third != NULL)
    {
        fprintf(stderr, "ERROR: map expects 2 arguments :(\n");
        return NULL;
    }

    lisp_object* func = eval(func_expr, table);
    lisp_object* list = eval(list_expr, table);

    if (func == NULL || func->type != LISP_FUNC)
    {
        fprintf(stderr, "ERROR: map expects a function :(\n");
        return NULL;
    }
    if (list == NULL || (list->type != LISP_CONS && list->type != LISP_NIL))
    {
        fprintf(stderr, "ERROR: map expects a list :(\n");
        return NULL;
    }

    lisp_object* result = create_nil();
    lisp_object* last = NULL;
    lisp_object* cur = list;

    while (cur != NULL && cur->type == LISP_CONS)
    {
        lisp_object* call_args = create_cons(cur->data.cons.car, create_nil());
        lisp_object* val = call_func(func, call_args, table);
        del_point(call_args);

        if (val == NULL) 
        { 
            del_point(result); 
            return NULL; 
        }

        lisp_object* cell = create_cons(val, create_nil());
        if (last == NULL) 
        { 
            result = cell; 
            last = cell; 
        }
        else 
        { 
            last->data.cons.cdr = cell; 
            last = cell; 
        }

        cur = cur->data.cons.cdr;
    }

    return result;
}

lisp_object* eval(lisp_object* expr, Hash* table)
{
    static int depth = 0;
    depth++;
    if (depth > MAX_DEPTH)
    {
        depth--;
        fprintf(stderr, "ERROR: sorry, recursion is too deep :(\n");
        return NULL;
    }

    if (expr == NULL || table == NULL)
    {
        fprintf(stderr, "ERROR IN EVAL\n");
        return NULL;
    }

    lisp_object* res = NULL;

	switch (expr->type)
	{
        case LISP_SYMB:
        {
            res = get_Hash(table, expr->data.str.chars);
            if (res == NULL)
            {
                fprintf(stderr, "ERROR: %s is not defined :(\n", expr->data.str.chars);
                break;
            }
            break;
        }

        case LISP_CONS:
        {
            lisp_object* oper = expr->data.cons.car;
            lisp_object* func = NULL;

            if (oper != NULL && oper->type == LISP_SYMB)
            {
                func = get_Hash(table, oper->data.str.chars);
            }
            else if (oper != NULL)
            {
                func = eval(oper, table);
                if (func == NULL)
                {
                    res = NULL;
                    break;
                }
            }

            if (func != NULL && func->type == LISP_FUNC)
            {
                res = call_func(func, expr->data.cons.cdr, table);
                break;
            }

            if (oper != NULL && oper->type == LISP_SYMB)
            {
                if (strcmp(oper->data.str.chars, "quote") == 0)
                {
                    res = quote(expr->data.cons.cdr);
                    break;
                }
                else if (strcmp(oper->data.str.chars, "if") == 0)
                {
                    res = ifelse(expr->data.cons.cdr, table);
                    break;
                }
                else if (strcmp(oper->data.str.chars, "define") == 0)
                {
                    lisp_object* args = expr->data.cons.cdr;
                    lisp_object* first = args->data.cons.car;
                    lisp_object* val_expr = cons_sec(args);
                    lisp_object* third = cons_third(args);

                    if (first != NULL && first->type == LISP_SYMB)
                    {
                        lisp_object* value = eval(val_expr, table);
                        if (value == NULL)
                        {
                            res = NULL;
                            break;
                        }
                        res = define_var(first, value, third, table);
                        break;
                    }
                    else if (first != NULL && first->type == LISP_CONS)
                    {
                        lisp_object* val_expr = args->data.cons.cdr;
                        res = define_func(first, val_expr, table, 1);
                        break;
                    }
                    else
                    {
                        fprintf(stderr, "ERROR: bad syntax in define :(\n");
                        res = NULL;
                        break;
                    }
                }
                else if (strcmp(oper->data.str.chars, "define-no-mem") == 0)
                {
                    lisp_object* args = expr->data.cons.cdr;
                    lisp_object* first = args->data.cons.car;
                    lisp_object* val_expr = cons_sec(args);
                    lisp_object* third = cons_third(args);

                    if (first != NULL && first->type == LISP_CONS)
                    {
                        lisp_object* val_expr = args->data.cons.cdr;
                        res = define_func(first, val_expr, table, 0);
                        break;
                    }
                    else
                    {
                        fprintf(stderr, "ERROR: bad syntax in define-no-mem :(\n");
                        res = NULL;
                        break;
                    }
                }
                else if (strcmp(oper->data.str.chars, "lambda") == 0)
                {
                    res = lambda(expr->data.cons.cdr, table, 1);
                    break;
                }
                else if (strcmp(oper->data.str.chars, "lambda-no-mem") == 0)
                {
                    res = lambda(expr->data.cons.cdr, table, 0);
                    break;
                }
                else if (strcmp(oper->data.str.chars, "set!") == 0)
                {
                    lisp_object* args = expr->data.cons.cdr;
                    lisp_object* var = args->data.cons.car;
                    lisp_object* val_expr = cons_sec(args);
                    lisp_object* third = cons_third(args);
                    lisp_object* value = eval(val_expr, table);
                    if (value == NULL)
                    {
                        res = NULL; 
                        break;
                    }
                    res = set(args->data.cons.car, value, third, table);
                    break;
                }
                else if (strcmp(oper->data.str.chars, "and") == 0)
                {
                    res = case_and(expr->data.cons.cdr, table);
                    break;
                }
                else if (strcmp(oper->data.str.chars, "or") == 0)
                {
                    res = case_or(expr->data.cons.cdr, table);
                    break;
                }
                else if (strcmp(oper->data.str.chars, "map") == 0)
                {
                    res = case_map(expr->data.cons.cdr, table);
                    break;
                }
            }

            fprintf(stderr, "ERROR: no such operation defined yet :(\n");
            res = NULL;
            break;
        }
        default:
        {
            res = expr;
            break;
        }
	}
    
    depth--;
    if (res == NULL)
    {
        return NULL;
    }
    return res;
}