
#base=trunk
gccver=`gcc -dumpversion | sed "s/\./_/g"`
base=tags/gcc_${gccver}_release
echo Working with gcc source $base

rm -rf libsupc++ gcctmp
svn checkout svn://gcc.gnu.org/svn/gcc/$base/libstdc++-v3/libsupc++
sed -i "s/<exception>/<exception>\n#include <cstddef>\n/" libsupc++/new
# to avoid having to figure out how to support demangling; no
# internal exceptions thrown by ace or yarp anyway...
sed -i "s|dem = __cxa_demangle|//dem = __cxa_demangle|" libsupc++/vterminate.cc

svn export svn://gcc.gnu.org/svn/gcc/$base/gcc/unwind-pe.h libsupc++/unwind-pe.h
