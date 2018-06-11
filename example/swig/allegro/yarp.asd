; -*- Mode: Lisp; Syntax: ANSI-Common-Lisp; Base: 10 -*-

;; Copyright: (C) 2008 Lorenz Mosenlechner
;; CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE

;(in-package "LOGGING-DB")

;; *****************************************************************************
;; ** LOGGING-DB                                                              **
;; *****************************************************************************

(asdf:defsystem yarp
  :name "yarp"
  :author "Lorenz Mösenlechner <moesenle@cs.tum.edu>"
  :version "1.0"
  :maintainer "Lorenz Mösenlechner <moesenle@cs.tum.edu>"
  :licence "LGPL v2.1 or later"
  :description "YARP wrappers"
  :long-description "YARP wrappers"

  :depends-on ()

  :components
    ( (:library-directory "lib/")
      (:module
        "src"
        :default-component-class asdf:lisp-source-file
        :components
          ( (:file "yarp") )) ))

