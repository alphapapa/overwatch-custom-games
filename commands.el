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

;; Settings I use when not using my main config.

(electric-pair-mode 1)
(set-face-font 'default "Lucida Console")

(require 'package)
(setq package-archives '(("gnu" . "https://elpa.gnu.org/packages/")
                         ("melpa" . "https://melpa.org/packages/")))

(package-initialize)
