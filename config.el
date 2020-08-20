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

(defun disc-golf-add-vectors-from-clipboard ()
  "Add tee and hole vectors from clipboard to current rule.
Expects the clipboard to have the player's variables copied from
the inspector as rules."
  (interactive)
  (let* (tee hole)
    (with-temp-buffer
      (yank)
      (goto-char (point-min))
      (search-forward "Event Player.POINT_Tee = ")
      (set-mark (point))
      (goto-char (1- (point-at-eol)))
      (setf tee (buffer-substring (region-beginning) (region-end)))
      (search-forward "Event Player.POINT_Goal = ")
      (set-mark (point))
      (goto-char (1- (point-at-eol)))
      (setf hole (buffer-substring (region-beginning) (region-end))))
    (search-backward "Global.VECTORS_Tee_Positions = Array(")
    (search-forward ");")
    (backward-char 2)
    (insert ", " tee)
    (search-forward "Global.VECTORS_Hole_Positions = Array(")
    (search-forward ");")
    (backward-char 2)
    (insert ", " hole)))

;; Settings I use when not using my main config.

(menu-bar-mode -1)
(tool-bar-mode -1)
(electric-pair-mode 1)

(set-face-font 'default "Lucida Console")

(require 'package)
(setq package-archives '(("gnu" . "https://elpa.gnu.org/packages/")
                         ("melpa" . "https://melpa.org/packages/")))
(package-initialize)
