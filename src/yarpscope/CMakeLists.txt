option(CREATE_YARPSCOPE "Do you want to compile Yarp Port Scope?" OFF)

if(CREATE_YARPSCOPE)
    get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
    include_directories(${YARP_TREE_INCLUDE_DIRS})

    find_package(GtkMM REQUIRED)
    include_directories (${GTKMM_INCLUDE_DIRS})

    set(YARPSCOPE_USE_SYSTEM_GTKDATABOXMM OFF CACHE BOOL "Use system GtkDataboxMM library")
    mark_as_advanced(YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)
    if(YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)
        find_package(GtkDataboxMM REQUIRED)
        include_directories(${GTKDATABOXMM_INCLUDE_DIRS})
        link_directories(${GTKDATABOXMM_LIBRARY_DIRS})
    endif(YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)

    add_definitions("--ansi")

    add_subdirectory(src)

endif(CREATE_YARPSCOPE)
