### parameters are 
# $1: hostname (could be also buildtype)
# $2: os (macos, winxp, lenny, etch, karmic ...)
# $3: test type: nightly continuous experimental

CMAKE_OPTIONS="\
-DTEST_MACHINE_HOSTNAME:STRING=$1 \
-DTEST_MACHINE_OS_TYPE:STRING=$2 \
-DTEST_MACHINE_TEST_TYPE:STRING=$3 \
-DENABLE_DASHBOARD_SUBMIT:BOOL=TRUE \
-DCMAKE_SKIP_INSTALL_RPATH:BOOL=TRUE \
-DYARP_COMPILE_TESTS:BOOL=ON \
-DCREATE_GUIS:BOOL=TRUE \
-DCREATE_YARPBUILDER:BOOL=TRUE \
-DCREATE_LIB_MATH:BOOL=TRUE \
-DCREATE_OPTIONAL_CARRIERS:BOOL=TRUE \
-DENABLE_yarpcar_tcpros_carrier:BOOL=TRUE \
-DENABLE_yarpcar_rossrv_carrier:BOOL=TRUE \
-DENABLE_yarpcar_xmlrpc_carrier:BOOL=TRUE \
-DENABLE_yarpcar_bayer_carrier:BOOL=TRUE \
-DENABLE_yarpcar_priority_carrier:BOOL=TRUE \
-DENABLE_yarpcar_human_carrier:BOOL=TRUE \
-DCREATE_DEVICE_LIBRARY_MODULES:BOOL=TRUE \
-DENABLE_yarpmod_fakebot:BOOL=TRUE \
-DTEST_yarpidl_rosmsg:BOOL=TRUE \
-DTEST_yarpidl_thrift:BOOL=TRUE \
"
OS_TYPE=""
echo "$2" | grep -iq "Debian\|Ubuntu"
if [ "$?" == "0" ]; then
  OS_TYPE="linux"
fi

echo "$2" | grep -iq "Windows"
if [ "$?" == "0" ]; then
  OS_TYPE="windows"
fi

echo "$2" | grep -iq "MacOSX"
if [ "$?" == "0" ]; then
  OS_TYPE="macosx"
fi

case $3 in
   "Experimental" )
      CMAKE_OPTIONS=" \
        $CMAKE_OPTIONS \
      " 
      ;;
   "Continuous" )
     CMAKE_OPTIONS=" \
        $CMAKE_OPTIONS \
      " 
      ;;
   "Nightly" )
      CMAKE_OPTIONS=" \
        $CMAKE_OPTIONS \
      "
      if [ "$OS_TYPE" == "linux" ]; then
        CMAKE_OPTIONS=" \
          $CMAKE_OPTIONS \
          #-DYARP_VALGRIND_TESTS=ON
        "
      fi
     ;;
esac
