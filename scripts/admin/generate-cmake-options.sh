### parameters are 
# $1: hostname (could be also buildtype)
# $2: os type (macos, winxp, lenny, etch, karmic ...)

case $2 in
# no special condition for macos any more
#	macos)
#	CMAKE_OPTIONS="-DCREATE_GUIS:BOOL=FALSE -DCREATE_LIB_MATH:BOOL=TRUE -DENABLE_DASHBOARD_SUBMIT:BOOL=TRUE -DUSE_PORT_AUTHENTICATION:BOOL=TRUE"
#	;;
	*)
	CMAKE_OPTIONS="-DCREATE_IDLS:BOOL=TRUE -DCREATE_GUIS:BOOL=TRUE -DCREATE_LIB_MATH:BOOL=TRUE -DENABLE_DASHBOARD_SUBMIT:BOOL=TRUE -DUSE_PORT_AUTHENTICATION:BOOL=TRUE -DCREATE_OPTIONAL_CARRIERS=TRUE -DENABLE_yarpcar_tcpros_carrier=TRUE -DENABLE_yarpcar_xmlrpc_carrier=TRUE -DENABLE_yarpcar_bayer_carrier=TRUE -DUSE_LIBDC1394=FALSE -DCREATE_IDLS=TRUE -DENABLE_yarpidl_thrift=TRUE"
esac
	
