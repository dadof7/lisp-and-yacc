(progn (setq defun '(macro (f params &rest body)
			   (list 'setq f (list 'quote (list 'lambda  params
							    (cons 'progn body))))))
       'defun)


(progn  
  (setq defmacro
    '(macro (m params &rest body)
       (list 'progn (list 'setq m (list 'quote (list 'macro params (cons 'list (cons ''progn body))))))))
  'defmacro)


;; (progn
;;   (defmacro if (b x &rest y)
;;     (list '_if b x (cons 'progn y)))
;;   'if)

(progn
  (defmacro unless (b x &rest y)
    (list 'if (list 'not b) x (cons 'progn y)))
  'unless)

;; (progn (setq let '(macro (b &rest e)
;; 			 (cons
;; 			  (list
;; 			   'quote
;; 			   (list
;; 			    'lambda
;; 			    (mapcar car b)
;; 			    (cons 'progn e)
;; 			    ))
;; 			  (mapcar car (mapcar cdr b)))))
;;        'let)

;; (progn (setq let* '(macro (b &rest e)
;; 			  (let ((f '(lambda (b e)
;; 				      (unless b
;; 					(cons 'progn e)
;; 					(list (list 'quote (list
;; 							    'lambda
;; 							    (list (car (car b)))
;; 							    (f (cdr b) e)))
;; 					      (car (cdr (car b))))))))
;; 			    (f b e))))
;;        'let*)

(progn
  (defun av (sy lst)
    (cdr (assoc sy lst)))
  'av)


(progn (setq fac '(lambda (n) (if (= n 0) 1 (mul2 n (fac (1- n)))))) 'fac)

(progn
  (defun append (l1 l2)
    (if (null l1)	l2
      (cons (car l1) (append (cdr l1) l2))))
  'append)

(progn 
  (defun reverse (l)
    (let1 rev (lambda (l res)
		(if (null l) res
		  (rev (cdr l) (cons (car l) res))))
      (rev l nil)))
  'reverse)

(progn (defun length (l)
	 (unless l 0
		 (add2 1 (length (cdr l)))))
       'length)

(progn (defun nth (n l)
	 (cond ((not l) nil)
	       ((= n 1) (car l))
	       (t (nth (1- n) (cdr l)))))
       'nth)

(progn 
  (defun + (&rest l)
    (let ((f '(lambda (l) (unless l 0 (add2 (car l) (f (cdr l)))))))
      (f l)))

  (defun * (&rest l)
    (let ((f '(lambda (l) (unless l 1 (mul2 (car l) (f (cdr l)))))))
      (f l)))

  (defun - (a b &rest l)
    (let ((f '(lambda (l) (unless l 0 (add2 (car l) (f (cdr l)))))))
      (sub2 (sub2 a b) (f l))))

  (defun / (a b &rest l)
    (let ((f '(lambda (l) (unless l 1 (mul2 (car l) (f (cdr l)))))))
      (div2 (div2 a b) (f l))))

  (list '+ '* '- '/))

(progn
  (defun filter (f l)
    (if (null l) nil
      (if (f (car l)) (cons (car l) (filter f (cdr l)))
	(filter f (cdr l)))))
  'filter)

(progn
  (defmacro let1 (x v &rest args)
    (list 'let (list (list x v))
	  (cons 'progn args)))

  (defmacro let2 (x v y w &rest args)
    (list 'let (list
		(list x v)
		(list y w)	      
		)
	  (cons 'progn args)))

  (defmacro let2* (x v y w &rest args)
    (list 'let* (list
		 (list x v)
		 (list y w)	      
		 )
	  (cons 'progn args)))
  'let1-2*
  )

(progn
  (defmacro when (b &rest args)
    (list 'if b (cons 'progn args)))
  'when)

;; (progn
;;   (defun range (a b)
;;     (if (>= a b) nil
;;       (cons a (range (1+ a) b))))
;;   'range)

(progn
  (defun equal (a b)
    (cond ((and (atom a) (atom b)) (= a b))
	  ((atom a) nil)
	  ((atom b) nil)
	  (t (and (equal (car a) (car b)) (equal (cdr a) (cdr b))))))
  'equal)

(progn 
  (setq hanoi '(lambda (n start goal work)
		 (cond ((= n 1) (cons (list 'move start 'to goal) nil))
		       (t (append (hanoi (1- n) start work goal)
				   (cons (list 'move  start 'to goal)
					 (hanoi (1- n) work goal start))))))) 
  'hanoi)

;(hanoi 3 'S 'G 'W)

(progn
  (defmacro 1++ (x)
    `(setq ,x (1+ ,x)))

  (defmacro 1-- (x)
    `(setq ,x (1- ,x)))

  (defmacro while (b &rest args)
    (let1 f (gensym)
      `(let ((,f (lambda ()
		   (if (null ,b) nil
		     ,@args
		     (,f)))))
	 (,f))))

  (defmacro for (i l &rest args)
    (let1 f (gensym)
      `(let ((,f (lambda (ll)
		   (if (null ll) nil
		     (let ((,i (car ll)))
		       ,@args
		       (,f (cdr ll))))))
	     )
	 (if (intp ,l) (,f (range 0 ,l)) (,f ,l)))))
  'while)

(progn 
  (defun cat (&rest args)
    (if (null args) ""
      (concatenate (2str (car args))
		   (apply cat (cdr args)))))
  'cat)

(gc)
