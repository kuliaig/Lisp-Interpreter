#include <stdio.h>
#include "object.h"
#include "parcer.h"
#include "printer.h"

int main()
{
    char input[1024];
    
    while (printf("> ") && fgets(input, 1024, stdin)) {
        const char* rem;
        lisp_object* obj = parse_object(input, &rem);
        if (obj) {
            print_object(obj);
            printf("\n");
            del_point(obj);
        }
    }
    return 0;
}