#include <stdio.h>
#include "object.h"
#include "parcer.h"
#include "printer.h"
#include "hash.h"
#include "eval.h"
#include "func.h"

int main()
{
    Hash* table = create_Hash(NULL);

    module_math(table);
    module_cons(table);

    char input[1024];

    while (printf("> ") && fgets(input, 1024, stdin))
    {
        const char* rem;
        lisp_object* obj = parse_object(input, &rem);
        if (obj)
        {
            lisp_object* res = eval(obj, table);
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
        }
    }

    del_Hash(table);
    return 0;
}