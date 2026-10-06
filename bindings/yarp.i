// SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
// SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
// SPDX-License-Identifier: BSD-3-Clause

//////////////////////////////////////////////////////////////////////////
//
// This is a configuration file to explain YARP to SWIG
//
// SWIG, for the most part, understands YARP auto-magically.
// There are a few things that need to be explained:
//  + use of multiple inheritance
//  + use of names that clash with special names in Java/Python/Perl/...
//  + use of templates

%module(directors="1") yarp

%define SWIG_PREPROCESSOR_SHOULD_SKIP_THIS %enddef

// YARP_conf
%{
#include <yarp/conf/version.h>
#include <yarp/conf/compiler.h>
#include <yarp/conf/system.h>
#include <yarp/conf/api.h>
#include <yarp/conf/numeric.h>
#include <yarp/conf/string.h>
#include <yarp/conf/environment.h>
#include <yarp/conf/dirs.h>
%}

%include "yarp/conf/version.h"
%import "yarp/conf/compiler.h"
%import "yarp/conf/system.h"
%import "yarp/conf/api.h"
%import "yarp/conf/numeric.h"
%include "yarp/conf/string.h"
%include "yarp/conf/environment.h"
%include "yarp/conf/dirs.h"

%feature("autodoc", "1");

#if defined (SWIGPYTHON)
  %include <argcargv.i>
  %apply (int ARGC, char **ARGV) { (int argc, char *argv[]) }
#elif defined (SWIGLUA)
  %include "lua/argcargv.i"
  %apply (int ARGC, char **ARGV) { (int argc, char *argv[]) }
#endif

%include <stdint.i>
%include <std_vector.i>

// Try to translate std::string and std::pair to native equivalents
%include "std_string.i"
%include "std_pair.i"

#if defined(SWIGCSHARP)
    // Get .NET pointers instead of swig generated types (useful when dealing with images)
    %typemap(ctype)  unsigned char * "unsigned char *"
    %typemap(imtype) unsigned char * "System.IntPtr"
    %typemap(cstype) unsigned char * "System.IntPtr"
    %typemap(csin)   unsigned char * "$csinput"
    %typemap(in)     unsigned char * %{ $1 = $input; %}
    %typemap(out)    unsigned char * %{ $result = $1; %}
    %typemap(csout)  unsigned char * { return $imcall; }

    // there's a big CSHARP virtual/override muddle
    // we just bypass the issue for now
    %csmethodmodifiers write "public new";
    %csmethodmodifiers check "public new";
    %csmethodmodifiers check "public new";
    %csmethodmodifiers find "public new";
    %csmethodmodifiers findGroup "public new";
    %csmethodmodifiers toString "public new";
    %csmethodmodifiers lastRead "public new";
    %csmethodmodifiers isClosed "public new";
    %csmethodmodifiers read "public new";
    %csmethodmodifiers setReplier "public new";
    %csmethodmodifiers onRead "public virtual";
    %csmethodmodifiers getPendingReads "public new";
    %csmethodmodifiers setStrict "public new";
    %csmethodmodifiers useCallback "public new";
    %csmethodmodifiers onCommencement "public virtual";
    %csmethodmodifiers disableCallback "public virtual";
    %csmethodmodifiers acquire "public virtual";
    %csmethodmodifiers release "public virtual";
    %csmethodmodifiers isNull "public virtual";
    %csmethodmodifiers setTargetPeriod "public new";
#endif


#if defined(SWIGCSHARP)
    // there's a big CSHARP virtual/override muddle
    // we just bypass the issue for now
    %csmethodmodifiers write "public new";
    %csmethodmodifiers check "public new";
    %csmethodmodifiers check "public new";
    %csmethodmodifiers find "public new";
    %csmethodmodifiers findGroup "public new";
    %csmethodmodifiers toString "public new";
    %csmethodmodifiers lastRead "public new";
    %csmethodmodifiers isClosed "public new";
    %csmethodmodifiers read "public new";
    %csmethodmodifiers setReplier "public new";
    %csmethodmodifiers onRead "public new";
    %csmethodmodifiers getPendingReads "public new";
    %csmethodmodifiers setStrict "public new";
    %csmethodmodifiers useCallback "public new";
#endif

// Deal with method name conflicts
#ifndef SWIGJAVA
    %rename(toString_c) *::toString() const;
#endif

// python conflict
#ifdef SWIGPYTHON
    %rename(yield_c) *::yield();
#endif

// java conflict
#ifdef SWIGJAVA
    %rename(wait_c) *::wait();
    %rename(clone_c) *::clone() const;
    %rename(toString_c) *::toString() const;
#endif

#ifdef SWIGTCL
    %rename(configure_c) *::configure();
#endif

//////////////////////////////////////////////////////////////////////////
// Clean up a few unimportant things that give warnings

// operator= does not get translated well
%ignore *::operator=;
%rename(toString) std::string::operator const char *() const;
%rename(isEqual) *::operator==;
%rename(notEqual) *::operator!=;
%rename(access) *::operator();
//%ignore yarp::os::PortReader::read;



//////////////////////////////////////////////////////////////////////////
// C++ headers needed by the generated wrapper code.
// They must be included here, before any yarp_*.i file: SWIG emits the code of
// an %extend at the point where the extended class is declared, so for
// example the extensions of yarp::os::Port and yarp::os::Things (declared
// in yarp_os.i) need the YARP_sig headers.

// Deal with some clash in perl involving the name "seed"
%{
#define _SEARCH_H // strange perl clash
// careful shuffling to deal with perl clash on seed name
#ifdef seed
#define seed_c seed
#undef seed
#endif
#include <yarp/os/Random.h>
#ifdef seed_c
#define seed seed_c
#endif

// Bring in the header files that are important to us
#include <vector>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>


// Sometimes ACE redefines main() - we don't want that
#ifdef main
#undef main
#endif

// Bring in the main important namespace
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;
using namespace yarp::dev;
%}

// Define macros for handling the multiple analog sensors interfaces
%include macrosForMultipleAnalogSensors.i

//////////////////////////////////////////////////////////////////////////
// YARP_os
%include "yarp_os.i"

//////////////////////////////////////////////////////////////////////////
// YARP_sig
%include "yarp_sig.i"


//////////////////////////////////////////////////////////////////////////
// YARP_dev
%include "yarp_dev.i"


%template(DVector) std::vector<double>;
%template(BVector) std::vector<bool>;
%template(SVector) std::vector<std::string>;
%template(IVector) std::vector<int>;
%template(ShortVector) std::vector<short int>;
%template() std::pair<std::string, std::string>;
%template(SPairVector) std::vector<std::pair<std::string, std::string>>;

#ifdef SWIGMATLAB
  // Extend IVector for handling conversion of vectors from and to Matlab
  %include "matlab/vectors_fromTo_matlab.i"
#endif

// yarp::sig::Vector and yarp::sig::VectorInt must be wrapped after the std::vector<double>
// and std::vector<int> templates (see yarp_sig_vector.i)
%include "yarp_sig_vector.i"


//////////////////////////////////////////////////////////////////////////
// Just in Python (and in yarp bindings itself, not in downstream bindings
// that include yarp.i) add some code to automatically call
// add_dll_directory as necessary
// See https://github.com/robotology/robotology-superbuild/issues/1268
// for more details
#if defined(SWIGPYTHON) && defined(SWIG_GENERATING_YARP_BINDINGS)
%include <swig_python_windows_preable.i>
#endif
