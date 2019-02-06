;; Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
;; Copyright (C) 2008 Lorenz Mosenlechner
;;
;; This library is free software; you can redistribute it and/or
;; modify it under the terms of the GNU Lesser General Public
;; License as published by the Free Software Foundation; either
;; version 2.1 of the License, or (at your option) any later version.
;;
;; This library is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;; Lesser General Public License for more details.
;;
;; You should have received a copy of the GNU Lesser General Public
;; License along with this library; if not, write to the Free Software
;; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

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


