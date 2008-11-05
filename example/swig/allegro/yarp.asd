; -*- Mode: Lisp; Syntax: ANSI-Common-Lisp; Base: 10 -*-

;(in-package "LOGGING-DB")

;; *****************************************************************************
;; ** LOGGING-DB                                                              **
;; *****************************************************************************

(asdf:defsystem yarp
  :name "yarp"
  :author "Lorenz Mösenlechner <moesenle@cs.tum.edu>"
  :version "1.0"
  :maintainer "Lorenz Mösenlechner <moesenle@cs.tum.edu>"
  :licence "GPL v2"
  :description "Library for Pose transformation"
  :long-description "Library for Pose transformation"

  :depends-on ()

  :components
    ( (:library-directory "lib/")
      (:module
        "src"
        :default-component-class asdf:lisp-source-file
        :components
          ( (:file "yarp") )) ))

