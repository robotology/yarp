### parameters are 
# $1: hostname (could be also buildtype)
# $2: os type (macos, winxp, lenny, etch, karmic ...)
# $3: test type: nightly continuous experimental

case $2 in
# no special condition for macos any more
#	macos)
#	CMAKE_OPTIONS="-DCREATE_GUIS:BOOL=FALSE -DCREATE_LIB_MATH:BOOL=TRUE -DENABLE_DASHBOARD_SUBMIT:BOOL=TRUE -DUSE_PORT_AUTHENTICATION:BOOL=TRUE"
#	;;
	*)
	CMAKE_OPTIONS="-DCREATE_IDLS:BOOL=TRUE -DCREATE_GUIS:BOOL=TRUE -DCREATE_LIB_MATH:BOOL=TRUE -DENABLE_DASHBOARD_SUBMIT:BOOL=TRUE -DUSE_PORT_AUTHENTICATION:BOOL=TRUE -DCREATE_OPTIONAL_CARRIERS=TRUE -DENABLE_yarpcar_tcpros_carrier=TRUE -DENABLE_yarpcar_xmlrpc_carrier=TRUE -DENABLE_yarpcar_bayer_carrier=TRUE -DUSE_LIBDC1394=FALSE -DENABLE_yarpcar_priority_carrier=TRUE -DCREATE_IDLS=TRUE -DENABLE_yarpidl_thrift=TRUE -DEXPERIMENTAL_ACEFIND=TRUE"
esac

case $3 in
experimental)
     CMAKE_OPTIONS="$CMAKE_OPTIONS -DCREATE_YBUILDER:BOOL=TRUE"
esac

