;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((text-mode . ((eval . (when (string-suffix-p ".txt" buffer-file-name) (fundamental-mode) (prism-mode))))))
