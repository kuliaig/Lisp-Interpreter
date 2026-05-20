#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "parcer.h"
#include "printer.h"
#include "hash.h"
#include "eval.h"
#include "func.h"
#include "reader.h"

void help()
{
    printf("Usage: lisp.exe [options] file...\n");
    printf("Options:\n");
    printf("  --help  Display this information");
    printf("  --help  Display this information"); // CHANGE WHEN FILE
}

int main(int argc, char** argv)
{
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
                    del_point(res); 
                }
                del_point(obj);
                p = rem;
                skip_spaces(&p);
            }
            free(input);
            del_point_Hash(table);
            return 0;
        }
    }

    Hash* table = create_Hash(NULL);

    module_math(table);
    module_cons(table);
    module_check(table);

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
            del_point(obj);

            p = rem;
            skip_spaces(&p);
        }

        free(input);
    }

    del_point_Hash(table);
    return 0;
}