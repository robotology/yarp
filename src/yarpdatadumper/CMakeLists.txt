# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpdatadumper)
  add_executable(yarpdatadumper)

  set(yarpdatadumper_SRCS
    main.cpp
  )

  target_sources(yarpdatadumper
    PRIVATE
      ${yarpdatadumper_SRCS})

  target_link_libraries(yarpdatadumper
    PRIVATE
      YARP::YARP_os
      YARP::YARP_init
      YARP::YARP_sig
  )

  if(YARP_HAS_OpenCV)
    target_compile_definitions(yarpdatadumper PRIVATE ADD_VIDEO)
    target_link_libraries(yarpdatadumper PRIVATE ${OpenCV_LIBRARIES})
    target_link_libraries(yarpdatadumper PRIVATE YARP::YARP_cv)
  endif()

  install(
    TARGETS yarpdatadumper
    COMPONENT utilities
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )

  set_property(TARGET yarpdatadumper PROPERTY FOLDER "Command Line Tools")

  if(CMAKE_HOST_WIN32)
    # install the yarpdatadumperAppGenerator.py helper script
    yarp_install(
      PROGRAMS yarpdatadumperAppGenerator.py
      COMPONENT utilities
      DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
  else()
    yarp_install(
      PROGRAMS yarpdatadumperAppGenerator.py
      COMPONENT utilities
      DESTINATION "${YARP_DATA_INSTALL_DIR}/scripts/"
    )
    file(RELATIVE_PATH _relpath "/${CMAKE_INSTALL_BINDIR}" "/${YARP_DATA_INSTALL_DIR}/scripts/yarpdatadumperAppGenerator.py")
    if (NOT EXISTS "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
      execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${_relpath}" "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator")
    install(
      CODE
 "message(STATUS \"Symbolic link: \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator to ${_relpath}\")
  if (NOT EXISTS \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/\")
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/\")
  endif()
  execute_process(COMMAND \${CMAKE_COMMAND} -E create_symlink \"${_relpath}\"
                                                               \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/yarpdatadumperAppGenerator\")"
      COMPONENT utilities
    )
  endif()
endif()
