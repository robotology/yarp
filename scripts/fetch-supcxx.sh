
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

svn checkout svn://gcc.gnu.org/svn/gcc/$base/gcc gcctmp --depth empty
cd gcctmp
svn up unwind-pe.h
cd ..
mv gcctmp/unwind-pe.h libsupc++
rm -rf gcctmp

#svn checkout svn://gcc.gnu.org/svn/gcc/$base/libiberty libiberty
#cd libiberty
#cp cp-demangle.c cp-demangle.h demangle.h libiberty.h ansidecl.h ../libsupc++
#cd ..


