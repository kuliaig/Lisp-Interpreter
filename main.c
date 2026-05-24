#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "object.h"
#include "parcer.h"
#include "printer.h"
#include "hash.h"
#include "eval.h"
#include "func.h"
#include "reader.h"
#include "library.h"
#ifdef _WIN32
#include <windows.h>
#else 
#include <dlfcn.h>
#endif

Hash* table = NULL;

static void bye(int sign)
{
    printf("\nBye-bye! :)\n");
    exit(0);
}

static lisp_object* load_plugin(lisp_object* args)
{
    if (args == NULL || args->type != LISP_CONS)
    {
        fprintf(stderr, "ERROR: load-plugin expects 1 argument :(\n");
        return NULL;
    }

    const char* path = args->data.cons.car->data.str.chars;

#ifdef _WIN32
    HMODULE handle = LoadLibraryA(path);
#else
    void* handle = dlopen(path, RTLD_NOW);
#endif 

    if (handle == NULL)
    {
        fprintf(stderr, "ERROR: cannot load plugin %s :(\n", path);
        return NULL;
    }

#ifdef _WIN32
    plugin* (*init)(void) = (void*)GetProcAddress(handle, "init_plugin");
#else
    plugin* (*init)(void) = dlsym(handle, "init_plugin");
#endif 

    if (init == NULL)
    {
        fprintf(stderr, "ERROR: plugin has no init_plugin, please add it :(\n");
        return NULL;
    }

    plugin* funcs = init();
    for (int i = 0; funcs[i].name != NULL && funcs[i].func != NULL; i++)
    {
        put_Hash(table, funcs[i].name, create_inside(funcs[i].func, funcs[i].name));
    }

    return create_void();
}


static void help()
{
    printf("Usage: lisp.exe [options] file...\n");
    printf("Options:\n");
    printf("  --help                Display this information\n");
    printf("  open <file>           Execute script from file\n");
    printf("Examples:\n");
    printf(" WINDOWS:\n");
    printf("  .\\lisp                  Start interactive REPL\n");
    printf("  .\\lisp open file.scm    Execute script from file\n");
    printf("  .\\lisp --help           Display this information\n");
    printf(" LINUX:\n");
    printf("  ./lisp                  Start interactive REPL\n");
    printf("  ./lisp open file.scm    Execute script from file\n");
    printf("  ./lisp --help           Display this information\n");
    printf("Built-in special forms:\n");
    printf("  define define-no-mem lambda lambda no-mem\n");
    printf("  if quote set! and or map \n");
    printf("Built-in functions:\n");
    printf("  + - * / = < > <= >= not\n");
    printf("  car cdr cons caar cadr cdar cddr\n");
    printf("  null? pair? list? number? symbol? boolean?\n");
    printf("  string? procedure? vector?zero? positive? negative?\n");
    printf("  load-plugin \"plugin\"\n");
    printf("Plugin API:\n");
    printf("  See liblisp.h, README.md and an example arraylib.c\n");
}

int main(int argc, char** argv)
{
    signal(SIGINT, bye);
    if (argc >= 2)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            help();
            return 0;
        }

        if (strcmp(argv[1], "open") == 0)
        {
            if (argc == 2 || argc > 3)
            {
                fprintf(stderr, "ERROR: my lisp can take only one file at one time :(\n");
                return 1;
            }

            FILE* f = fopen(argv[2], "r");
            if (f == NULL)
            {
                fprintf(stderr, "ERROR: cannot open file %s :(\n", argv[2]);
                return 1;
            }

            fseek(f, 0, SEEK_END);
            long long size = ftell(f);
            fseek(f, 0, SEEK_SET);
            char* input = malloc(size + 1);
            fread(input, 1, size, f);
            input[size] = '\0';
            fclose(f);

            Hash* table = create_Hash(NULL);

            module_math(table);
            module_cons(table);
            module_check(table);

            const char* p = input;
            while (*p)
            {
                const char* rem;
                lisp_object* obj = parse_object(p, &rem);
                if (obj == NULL)
                {
                    fprintf(stderr, "ERROR: your file have bad syntax :(\n");
                    return 1;
                }
                lisp_object* res = eval(obj, table);
                if (res == NULL) 
                { 
                    del_point(obj); 
                    return 1;
                }
                if (res->type != LISP_VOID) 
                { 
                    print_object(res); 
                    printf("\n"); 
                }
                if (res != obj)
                {
                    del_point(obj);
                }
                p = rem;
                skip_spaces(&p);
            }
            free(input);
            del_point_Hash(table);
            return 0;
        }

        printf("Bad arguments, please use .\\lisp --help :)\n");
        return 1;
    }

    table = create_Hash(NULL);

    module_math(table);
    module_cons(table);
    module_check(table);
    put_Hash(table, "load-plugin", create_inside(load_plugin, "load-plugin"));
    

    while (1)
    {
        char* input = read_input(table);

        if (input == NULL)
        {
            continue;
        }
        const char* p = input;
        while (*p)
        {
            const char* rem;
            lisp_object* obj = parse_object(p, &rem);
            if (obj == NULL)
            {
                fprintf(stderr, "ERROR: bad syntax :(\n");
                break;
            }

            lisp_object* res = eval(obj, table);

            if (res == NULL)
            {
                del_point(obj);
                break;
            }

            if (res != NULL)
            {
                if (res->type != LISP_VOID)
                {
                    print_object(res);
                    printf("\n");
                }
                del_point(res);
            }
            if (res != obj)
            {
                del_point(obj);
            }

            p = rem;
            skip_spaces(&p);
        }

        free(input);
    }

    del_point_Hash(table);
    return 0;
}