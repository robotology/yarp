(declare (uses cyarp))

; Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
; Copyright (C) 2006-2010 RobotCub Consortium
; All rights reserved.
;
; This software may be modified and distributed under the terms of the
; BSD-3-Clause license. See the accompanying LICENSE file for details.

(Network-init)

(define p (new-BufferedPortBottle))

(BufferedPortBottle-open p "/chicken")

(define top 100)
(do ((i 1 (+ i 1)))
    ((> i top) #f) ; return #f at the end
  (let ((bottle (BufferedPortBottle-prepare p)))
    (Bottle-clear bottle)
    (Bottle-addString bottle "count")
    (Bottle-addInt bottle i)
    (Bottle-addString bottle "of")
    (Bottle-addInt bottle top)
    (display "Sending ")
    (display (Bottle-toString bottle))
    (newline)
    (BufferedPortBottle-write p)
    (Time-delay 0.5)))

(Network-fini)

