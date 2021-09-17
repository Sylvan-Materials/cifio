#!/usr/bin/guile -s
!#
(use-modules (gnutls))
(gnutls-version)
;;;(use-modules (system ./libcifio))
(load-extension "./libguile-cifio" "init_cifio")
(use-modules (web client))
(use-modules (ice-9 receive))
(use-modules (ice-9 iconv))
(use-modules (ice-9 format))
(use-modules (rnrs bytevectors))
(display "Hello, world!")
(newline)

(define cifContent #f)
(receive (response-status response-body)
    (http-get "https://files.rcsb.org/download/3sgs.cif" )
              #:headers '((Content-Type . "text/plain"))
              #:decode-body? #t
  (display response-status)
  (set! cifContent (utf8->string response-body))
)
;;;(display cifContent)
;;;(populate_graph cifContent)
(define graph (populate_graph cifContent))
(newline)

(display (j0 2))
(newline)
;;;constitute graph 3sgs
;;;protonate 3sgs

(string->utf8 "café")
(display (utf8->string (u8-list->bytevector '(99 97 102 101))))


