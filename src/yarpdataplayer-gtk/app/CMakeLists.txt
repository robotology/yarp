# Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
# Authors: Vadim Tikhanoff
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

set(appname dataSetPlayer)
icub_app(${appname})

file(GLOB conf ${CMAKE_CURRENT_SOURCE_DIR}/conf/*.ini)
file(GLOB scripts ${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.template)

icub_app_install(${appname} 
                 FILES ${conf}
                 DESTINATION conf)
				 
icub_app_install(${appname} 
                 FILES ${scripts}
                 DESTINATION scripts)
