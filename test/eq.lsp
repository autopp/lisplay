(define x '(a b))

(eq () ())
(eq #t #t)
(eq #f #f)
(eq 'a 'a)
(eq x x)

(eq #t #f)
(eq 'a 'b)
(eq x '(a b))
(eq '(a b) '(a b))
