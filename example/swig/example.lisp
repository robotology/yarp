
;; Copyright: (C) 2008 Lorenz Mosenlechner
;; CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
(in-package :cl-user)


(let ( (p nil) )
  (defun prepare-yarp ()
    (yarp.yarp.os:network-init)
    (setf p (yarp.yarp.os:new-buffered-port-bottle))
    (yarp.yarp.os:buffered-port-bottle-open p "/allegro"))
  
  (defun test-yarp ()
    (let ( (bottle (yarp.yarp.os:buffered-port-bottle-prepare p)) )
      (yarp.yarp.os:bottle-clear bottle)
      (yarp.yarp.os:bottle-add-string bottle "Hello from Lisp ;)")
      (format t "Sending '~a'~%" (yarp.yarp.os:bottle-to-string bottle))
      (yarp.yarp.os:buffered-port-bottle-write p)))

  (defun finish-yarp ()
    (yarp.yarp.os:network-fini)))


