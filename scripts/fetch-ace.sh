#!/bin/bash

cwd=`pwd`

# fetch minimum part of ACE for YARP
if [ ! -e ace ]; then
    # *should* be safe to remove revision - please periodically update it
    # to known good revisions.
    svn co -r 90429 svn://svn.dre.vanderbilt.edu/DOC/Middleware/trunk/ACE/ace ace
else
    cd ace; svn up; cd ..
fi

mkdir -p $cwd/ace4yarp/src
mkdir -p $cwd/ace4yarp/include/ace
# Some .cpp files are sources and some are just templates.
# We separate them now in order to make building without MPC easier.
for f in `cat $cwd/ace/ace.mpc | sed '1,/Source_Files.ACE_COMPONENTS/ d' | sed '/\}/,$ d'`; do
    cp -u $cwd/ace/$f $cwd/ace4yarp/src
done
for f in `cat $cwd/ace/ace.mpc | sed '1,/Template_Files/ d' | sed '/\}/,$ d'`; do
    cp -u $cwd/ace/$f $cwd/ace4yarp/include/ace
done
for f in `cat $cwd/ace/ace.mpc | sed '1,/Inline_Files/ d' | sed '/\}/,$ d'`; do
    cp -u $cwd/ace/$f $cwd/ace4yarp/include/ace
done
for f in `cat $cwd/ace/ace.mpc | sed '1,/Header_Files/ d' | sed '/\}/,$ d' | grep "\.h" | grep -v "config\.h"`; do
    cp -u $cwd/ace/$f $cwd/ace4yarp/include/ace
done
cp -u $cwd/ace/*.h $cwd/ace4yarp/include/ace
cp -u $cwd/ace/*.inl $cwd/ace4yarp/include/ace
for f in `cat $cwd/ace/svcconf.mpb | sed '1,/Source_Files.ACE_COMPONENTS/ d' | sed '/\}/,$ d'`; do
    cp -u $cwd/ace/$f $cwd/ace4yarp/src
done
cp -u -R $cwd/ace/os_include $cwd/ace4yarp/include/ace/os_include

# configure for Linux and YARP usage.
cd $cwd/ace4yarp/include/ace
(
cat <<EOF
#ifndef ACE4YARP_CONFIG_H
#define ACE4YARP_CONFIG_H


#ifdef WIN32
#include <ace/config-win32.h>
#define ACE4YARP_DONE
#endif


// default: Linux
#ifndef ACE4YARP_DONE
#define ACE_HAS_NONSTATIC_OBJECT_MANAGER
#define ACE_DOESNT_INSTANTIATE_NONSTATIC_OBJECT_MANAGER
#define ACE_LACKS_ACE_SVCCONF
#include <ace/config-linux.h>
#if (__GNUC__ > 3)
#ifdef ACE_HAS_CUSTOM_EXPORT_MACROS
# undef ACE_HAS_CUSTOM_EXPORT_MACROS
# undef ACE_Proper_Export_Flag
# undef ACE_Proper_Import_Flag
# undef ACE_EXPORT_SINGLETON_DECLARATION
# undef ACE_EXPORT_SINGLETON_DECLARE
# undef ACE_IMPORT_SINGLETON_DECLARATION
# undef ACE_IMPORT_SINGLETON_DECLARE
#endif
#endif  /* __GNU__ > 3 */
/* eliminate all stray usage of C++ header files in ACE */
#ifdef ACE_HAS_STDCPP_STL_INCLUDES
#undef ACE_HAS_STDCPP_STL_INCLUDES
#endif
#ifndef ACE_LACKS_IOSTREAM_TOTALLY 
#define ACE_LACKS_IOSTREAM_TOTALLY 
#endif
#ifdef ACE_HAS_CPP98_IOSTREAMS
#undef ACE_HAS_CPP98_IOSTREAMS
#endif
#ifdef ACE_USES_CLASSIC_SVC_CONF
#undef ACE_USES_CLASSIC_SVC_CONF
#endif
#define ACE_USES_CLASSIC_SVC_CONF 0
/* done eliminating all stray usage of C++ header files in ACE */
#endif



#endif
EOF
) > config.h

