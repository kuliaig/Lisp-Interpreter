(define (square x) (* x x))
(define (fact n) (if (= n 0) 1 (* n (fact (- n 1)))))

(square 5)
(fact 5)

(define x 10)
(set! x 20)
x

'(1 2 3)
(car '(1 2 3))
(cdr '(1 2 3))

(if (= 5 5) "yes" "no")
(if #f 1 2)

"hello world"