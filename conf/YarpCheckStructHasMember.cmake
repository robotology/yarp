# - Check if the given struct or class has the specified member variable
# CHECK_STRUCT_HAS_MEMBER (STRUCT MEMBER HEADER VARIABLE)
#
#  STRUCT - the name of the struct or class you are interested in
#  MEMBER - the member which existence you want to check
#  HEADER - the header(s) where the prototype should be declared
#  VARIABLE - variable to store the result
#
# The following variables may be set before calling this macro to
# modify the way the check is run:
#
#  CMAKE_REQUIRED_FLAGS = string of compile command line flags
#  CMAKE_REQUIRED_DEFINITIONS = list of macros to define (-DFOO=bar)
#  CMAKE_REQUIRED_INCLUDES = list of include directories
#
# Example: CHECK_STRUCT_HAS_MEMBER("struct timeval" tv_sec sys/select.h HAVE_TIMEVAL_TV_SEC)
#
# Original file: <cmake>/Modules/CheckStructHasMember.cmake
# Ported to C++

# Copyright: 2007-2009 Kitware, Inc.
# CopyPolicy: 3-clause BSD

include(CheckCXXSourceCompiles)

macro(YARP_CHECK_STRUCT_HAS_MEMBER _STRUCT _MEMBER _HEADER _RESULT)
    set(_INCLUDE_FILES)
    foreach(it ${_HEADER})
        set(_INCLUDE_FILES "${_INCLUDE_FILES}#include <${it}>\n")
    endforeach(it)

    set(_CHECK_STRUCT_MEMBER_SOURCE_CODE "
${_INCLUDE_FILES}
int main()
{
    ${_STRUCT}* tmp;
    tmp->${_MEMBER}();
    return 0;
}
")
    check_cxx_source_compiles("${_CHECK_STRUCT_MEMBER_SOURCE_CODE}" ${_RESULT})

endmacro(YARP_CHECK_STRUCT_HAS_MEMBER)
