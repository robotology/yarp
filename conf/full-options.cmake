### A set of desirable yarp options.
## Assuming your build directory is in yarp/build, use as:
## cd build
## cmake -C ../conf/full-options.cmake ../ 

#libYARP_math
option(CREATE_LIB_MATH "Math library" TRUE)


# GUIS
option(CREATE_GUIS "Do you want to compile GUIs" ON)
option(CREATE_YARPMANAGER_PP "Do you want to create the new yarpmanager" ON)
option(CREATE_YARPBUILDER "Create old builder" OFF)

# robot interface
option(CREATE_YARPROBOTINTERFACE "Do you want to compile yarprobotinterface?" ON)

# useful carriers
option(CREATE_OPTIONAL_CARRIERS "Compile some optional carriers" TRUE)
option(ENABLE_yarpcar_tcpros_carrier "tcpros carrier" TRUE)
option(ENABLE_yarpcar_rossrv_carrier "rossrv carrier" TRUE)
option(ENABLE_yarpcar_xmlrpc_carrier "xmlrpc carrier" TRUE)
option(ENABLE_yarpcar_portmonitor_carrier "portmonitor carrier" TRUE)
option(ENABLE_yarpcar_priority_carrier "priority carrier" TRUE)
option(ENABLE_yarpcar_humand_carrier "priority carrier" TRUE)



