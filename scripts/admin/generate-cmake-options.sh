### parameters are 
# $1: hostname (could be also buildtype)
# $2: os type (macos, winxp, lenny, etch, karmic ...)
# $3: test type: nightly continuous experimental

CMAKE_OPTIONS="\
-DTEST_MACHINE_HOSTNAME:STRING=$1 \
-DTEST_MACHINE_OS_TYPE:STRING=$2 \
-DTEST_MACHINE_TEST_TYPE:STRING=$3 \
-DENABLE_DASHBOARD_SUBMIT:BOOL=TRUE \
-DYARP_COMPILE_TESTS:BOOL=ON \
-DUSE_LIBDC1394:BOOL=FALSE \
-DCREATE_GUIS:BOOL=TRUE \
-DCREATE_YARPBUILDER:BOOL=TRUE \
-DCREATE_LIB_MATH:BOOL=TRUE \
-DUSE_PORT_AUTHENTICATION:BOOL=FALSE \
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
     ;;
esac
