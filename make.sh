gcc main.c array.c eval.c hash.c object.c parcer.c printer.c func.c -o lisp -Wall -Wextra

> (define (make-counter x) (lambda () (set! x (+ x 1)) x))
> (define c1 (make-counter 0))
> (c1)
1
> (c1)