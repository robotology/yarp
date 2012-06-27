set(gPortScope_SRCS
    main.cpp
    MainWindow.cpp
)

set(gPortScope_HDRS
    MainWindow.h
)

add_executable(gPortScope ${gPortScope_SRCS})

target_link_libraries(gPortScope ${YARP_LIBRARIES}
                                 ${GTKMM_LIBRARIES}
                                 ${GTKDATABOXMM_LIBRARIES}
)
