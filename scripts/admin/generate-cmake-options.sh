### parameters are 
# $1: hostname (could be also buildtype)
# $2: os type (macos, winxp, lenny, etch, karmic ...)
# $3: test type: nightly continuous experimental

CMAKE_OPTIONS="\
-DTEST_MACHINE_HOSTNAME:STRING=$1 \
-DTEST_MACHINE_OS_TYPE:STRING=$2 \
-DTEST_MACHINE_TEST_TYPE:STRING=$3 \
-DENABLE_DASHBOARD_SUBMIT:BOOL=TRUE \
-DUSE_LIBDC1394=FALSE \
-DCREATE_GUIS:BOOL=TRUE \
-DCREATE_GYARPBUILDER:BOOL=TRUE \
-DCREATE_LIB_MATH:BOOL=TRUE \
-DUSE_PORT_AUTHENTICATION:BOOL=FALSE \
-DCREATE_OPTIONAL_CARRIERS=TRUE \
-DENABLE_yarpcar_tcpros_carrier=TRUE \
-DENABLE_yarpcar_xmlrpc_carrier=TRUE \
-DENABLE_yarpcar_bayer_carrier=TRUE \
-DENABLE_yarpcar_priority_carrier=TRUE \
-DCREATE_IDLS:BOOL=TRUE \
-DENABLE_yarpidl_thrift=TRUE \
-DENABLE_yarpidl_rosmsg=TRUE \
"

case $3 in
    experimental)
        # add any not-quite-ready-for-primetime options you want to test here
        CMAKE_OPTIONS="\
$CMAKE_OPTIONS \
"
        ;;
esac
