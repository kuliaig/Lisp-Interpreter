#pragma once
// from hash.h
typedef struct Hash Hash;

typedef enum lisp_type 
{
    LISP_NUM,
    LISP_SYMB,
    LISP_NIL,
    LISP_STR,
    LISP_BOOL,
    LISP_CONS,
    LISP_ARR,
    LISP_FUNCTION
} lisp_type;

typedef enum func_type
{
    FUNC_INSIDE,
    FUNC_USER
} func_type;

typedef struct lisp_object
{
    lisp_type type;

    union 
    {
        // LISP_NUM
        long long num_val;

        // LISP_SYMB, LISP_STR
        struct 
        {
            char* chars;
            int length;
        } str;

        // LISP_BOOL
        int bool_val;

        // LISP_CONS
        struct 
        {
            struct lisp_object* car;
            struct lisp_object* cdr;
        } cons;

        // LISP_ARR
        struct 
        {
            struct lisp_object** elements;
            int size;
            int capacity;
        } arr;

        // LISP_FUNC

        struct
        {
            func_type ftype;

            struct
            {
                lisp_object* (*c_func)(lisp_object* args);
                char* name;
            } inside;

            struct
            {
                lisp_object* args;
                lisp_object* body;
                struct Hash* table;
            } user;
        } func;
    } data;

    // count of pointers on this object
    int point_count;
} lisp_object;

// increase count of pointers 
void new_point(lisp_object* obj);

//decrease count of pointers and delete obj if it = 0 
void del_point(lisp_object* obj);

// create lisp object with type NUMBER
lisp_object* create_num(const long long value);

// create lisp object with type SYMBOL
lisp_object* create_symb(const char* name);

// create lisp object with type NILL (it's the only one in lisp)
lisp_object* create_nil();

// create lisp object with type STR
lisp_object* create_str(const char* str);

// create lisp object with type BOOL
lisp_object* create_bool(const int value);

// create lisp object with type CONS
lisp_object* create_cons(lisp_object* car, lisp_object* cdr);

// create lisp object with type ARR (it's empty)
lisp_object* create_arr();

// create lisp object with type FUNCTION (inside)
lisp_object* create_inside(lisp_object* (*c_func)(lisp_object* args), const char* name);

// create lisp object with type FUNCTION (user)
lisp_object* create_user(lisp_object* args, lisp_object* body, struct Hash* table);