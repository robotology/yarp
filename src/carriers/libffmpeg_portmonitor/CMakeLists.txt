
yarp_prepare_plugin(libffmpeg TYPE FfmpegMonitorObject
                        INCLUDE ffmpegPortmonitor.h
                        CATEGORY portmonitor
                        DEPENDS "ENABLE_yarpcar_portmonitor")#;YARP_HAS_ZFP")

if(NOT SKIP_libffmpeg)
  yarp_add_plugin(yarp_pm_libffmpeg)

  target_sources(yarp_pm_libffmpeg PRIVATE ffmpegPortmonitor.cpp
                                     ffmpegPortmonitor.h)

  target_link_libraries(yarp_pm_libffmpeg PRIVATE YARP::YARP_os
                                            YARP::YARP_sig)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig)

  target_include_directories(yarp_pm_libffmpeg SYSTEM PRIVATE ${FFMPEG_INCLUDE_DIR})
  target_link_libraries(yarp_pm_libffmpeg PRIVATE ${FFMPEG_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ZFP) (not using targets)

  yarp_install(TARGETS yarp_pm_libffmpeg
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_pm_libffmpeg PROPERTY FOLDER "Plugins/Port Monitor")
endif()
