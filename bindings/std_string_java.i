// This is java/std_string.i as of commit 12a9671440e9408a07d59a8936e75da096fea5fc
// on https://github.com/swig/swig , to be used with SWIG versions <2.0.7
// to be able to use the typemaps also for other strings than std::string
// According to http://www.swig.org/legal.html , this code was permissively licensed
// to be redistributed without restriction.

// Copyright: (C) 1995-2011 The SWIG developers
// CopyPolicy: permissive licence, BSD- and GPL-compatible

/* -----------------------------------------------------------------------------
 * std_string.i
 *
 * Typemaps for std::string and const std::string&
 * These are mapped to a Java String and are passed around by value.
 *
 * To use non-const std::string references use the following %apply.  Note
 * that they are passed by value.
 * %apply const std::string & {std::string &};
 * ----------------------------------------------------------------------------- */

%{
#include <string>
%}

namespace std {

%naturalvar string;

class string;

// string
%typemap(jni) string "jstring"
%typemap(jtype) string "String"
%typemap(jstype) string "String"
%typemap(javadirectorin) string "$jniinput"
%typemap(javadirectorout) string "$javacall"

%typemap(in) string
%{ if(!$input) {
     SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null string");
     return $null;
    }
    const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0);
    if (!$1_pstr) return $null;
    $1.assign($1_pstr);
    jenv->ReleaseStringUTFChars($input, $1_pstr); %}

%typemap(directorout) string
%{ if(!$input) {
     SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null string");
     return $null;
   }
   const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0);
   if (!$1_pstr) return $null;
   $result.assign($1_pstr);
   jenv->ReleaseStringUTFChars($input, $1_pstr); %}

%typemap(directorin,descriptor="Ljava/lang/String;") string
%{ $input = jenv->NewStringUTF($1.c_str()); %}

%typemap(out) string
%{ $result = jenv->NewStringUTF($1.c_str()); %}

%typemap(javain) string "$javainput"

%typemap(javaout) string {
    return $jnicall;
  }

%typemap(typecheck) string = char *;

%typemap(throws) string
%{ SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, $1.c_str());
   return $null; %}

// const string &
%typemap(jni) const string & "jstring"
%typemap(jtype) const string & "String"
%typemap(jstype) const string & "String"
%typemap(javadirectorin) const string & "$jniinput"
%typemap(javadirectorout) const string & "$javacall"

%typemap(in) const string &
%{ if(!$input) {
     SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null string");
     return $null;
   }
   const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0);
   if (!$1_pstr) return $null;
   $*1_ltype $1_str($1_pstr);
   $1 = &$1_str;
   jenv->ReleaseStringUTFChars($input, $1_pstr); %}

%typemap(directorout,warning=SWIGWARN_TYPEMAP_THREAD_UNSAFE_MSG) const string &
%{ if(!$input) {
     SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null string");
     return $null;
   }
   const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0);
   if (!$1_pstr) return $null;
   /* possible thread/reentrant code problem */
   static $*1_ltype $1_str;
   $1_str = $1_pstr;
   $result = &$1_str;
   jenv->ReleaseStringUTFChars($input, $1_pstr); %}

%typemap(directorin,descriptor="Ljava/lang/String;") const string &
%{ $input = jenv->NewStringUTF($1.c_str()); %}

%typemap(out) const string &
%{ $result = jenv->NewStringUTF($1->c_str()); %}

%typemap(javain) const string & "$javainput"

%typemap(javaout) const string & {
    return $jnicall;
  }

%typemap(typecheck) const string & = char *;

%typemap(throws) const string &
%{ SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, $1.c_str());
   return $null; %}

}

