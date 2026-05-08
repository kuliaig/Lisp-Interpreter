#include <stdi-o.h>
#include "object.h"
#include "parcer.h"
#include "printer.h"
#include "hash.h"
#include "eval.h"

int main()
{
    Hash* table = create_Hash(NULL);

    char input[1024];

    while (printf("> ") && fgets(input, 1024, stdin))
    {
        const char* rem;
        lisp_object* obj = parse_object(input, &rem);
        if (obj)
        {
            lisp_object* res = eval(obj, table);
            if (res != NULL && res->type != LISP_VOID)
            {
                print_object(res);
                printf("\n");
                del_point(res);
            }
            del_point(obj);
        }
    }

    del_Hash(table);
    return 0;+
}