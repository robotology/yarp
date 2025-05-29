# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpdatadumper)

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
