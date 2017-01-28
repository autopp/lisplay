(define sum (lambda (n) (if (equal? n 0) 0 (+ (sum (- n 1)) n))))
(sum 0)
(sum 4)
