;; Helpful commands for editing Overwatch custom games as text.

(defun insert-vector (x y z)
  (interactive (list (read-number "X: ")
		     (read-number "Y: ")
		     (read-number "Z: ")))
  (insert "Vector("
	  (number-to-string x)
	  ", "
	  (number-to-string y)
	  ", "
	  (number-to-string z)
	  ")"))

(defun goto-vector (n)
  (interactive (list (read-number "Element number: ")))
  (search-backward "Array(")
  (cl-loop while (search-forward "Vector(")
	   do (search-forward ")")
	   repeat n))
