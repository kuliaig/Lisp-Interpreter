#include "parcer.h"
#include "array.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void skip_spaces(const char **p)
{
    while (**p == ' ' || **p == '\t' || **p == '\n' || **p == '\r')
    {
        (*p)++;
    }
}

static lisp_object* parse_cons(const char* p, const char** remaind)
{
    skip_spaces(&p);
    
    if (*p == ')')
    {
        p++;
        *remaind = p;
        return create_nil();
    }

    lisp_object* car = parse_object(p, &p);
    if (car == NULL)
    {
        return NULL;
    }

    skip_spaces(&p);
    // work with . only if '(a . b), not just (a . b)
    if (*p == '.')
    {
        p++;
        skip_spaces(&p);
        lisp_object* cdr = parse_object(p, &p);
        if (cdr == NULL)
        {
            del_point(car);
            return NULL;
        }
        skip_spaces(&p);
        if (*p != ')')
        {
            del_point(car);
            del_point(cdr);
            return NULL;
        }

        p++;
        *remaind = p;
        return create_cons(car, cdr);
    }

    lisp_object* cdr = parse_cons(p, &p);
    *remaind = p;

    return create_cons(car, cdr);
}

static lisp_object* parse_string(const char* p, const char** remaind)
{
    const char* start = p;
    while (*p != '\"')
    {
        p++;
    }
    int length = p - start;
    char* str = malloc(length + 1);
    
    if (str == NULL)
    {
        return NULL;
    }

    memcpy(str, start, length);
    str[length] = '\0';

    p++;
    *remaind = p;
    lisp_object* new = create_str(str);
    free(str);
    return new;
}

static lisp_object* parse_quote(const char* p, const char** remaind)
{
    lisp_object* next = parse_object(p, &p);
    if (next == NULL)
    {
        return NULL;
    }
    lisp_object* quote = create_symb("quote");
    lisp_object* res = create_cons(quote, create_cons(next, create_nil()));

    *remaind = p;
    return res;
}

static lisp_object* parse_bool_or_arr(const char* p, const char** remaind)
{
    if (*p == '(')
    {
        p++;
        lisp_object* arr = create_arr();
        while (*p != ')')
        {
            skip_spaces(&p);
            lisp_object* new = parse_object(p, &p);
            if (new == NULL)
            {
                del_point(arr);
                return NULL;
            }
            arr_add(arr, new);
            del_point(new);
        }
        p++;
        *remaind = p;
        return arr;
    }
    else 
    {
        lisp_object* bull = NULL;
        if (*p == 't')
        {
            bull = create_bool(1);
            p++;
        }
        else if (*p == 'f')
        {
            bull = create_bool(0);
            p++;
        }
        if (bull == NULL)
        {
            return NULL;
        }

        *remaind = p;
        return bull;
    }
}

static lisp_object* parse_number(const char* p, const char** remaind)
{
    char oper = '+';
    if (*p == '-')
    {
        oper = '-';
        p++;
    }
    else if (*p == '+')
    {
        p++;
    }

    long long res = 0;
    while (*p != ' ' && *p != '\0' && *p != '\r' && *p != '\n' && *p != '\t'
       && *p != ')' && *p != '(')
    {
        if (*p > '9' || *p < '0')
        {
            return NULL;
        }
        res = res * 10 + (long long)(*p - '0');
        p++;
    }

    if (oper == '-')
    {
        res = -res;
    }

    *remaind = p;
    return create_num(res);
}

static lisp_object* parse_symbol(const char* p, const char** remaind)
{
    const char* start = p;

    while (*p && *p != ' ' && *p != '\0' && *p != '\r' && *p != '\n' && *p != '\t'
    && *p != '(' && *p !=')' && *p != '\"' && *p != '\'' && *p != '#')
    {
        p++;
    }

    int length = p - start;
    char* name = malloc(length + 1);
    
    if (name == NULL)
    {
        return NULL;
    }

    memcpy(name, start, length);
    name[length] = '\0';

    *remaind = p;
    lisp_object* new = create_symb(name);
    free(name);
    return new;
}

lisp_object* parse_object(const char* p, const char** remaind)
{
    skip_spaces(&p);
    if (*p == '\0') {
        *remaind = p;
        return create_nil();
    }
    lisp_object* obj;
    if ((*p >= '0' && *p <= '9') || ((*p == '-' || *p == '+') && p[1] >= '0' && p[1] <= '9'))
    {
        const char* temp = p;
        obj = parse_number(p, remaind);
        if (obj == NULL)
        {
            obj = parse_symbol(temp, remaind);
            p = temp;
        }
    } 
    else if (*p == '\"')
    {
        p++;
        obj = parse_string(p, remaind);
    }
    else if (*p == '#')
    {
        p++;
        obj = parse_bool_or_arr(p, remaind);
    }
    else if (*p == '(')
    {
        p++;
        obj = parse_cons(p, remaind);
    }
    else if (*p == '\'')
    {
        p++;
        obj = parse_quote(p, remaind);
    }
    else
    {
        obj = parse_symbol(p, remaind);
    }
    return obj;
}