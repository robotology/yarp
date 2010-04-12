find_program(DOXYGEN_EXE NAMES doxygen)
mark_as_advanced(DOXYGEN_EXE)
if (DOXYGEN_EXE)
  configure_file(${CMAKE_SOURCE_DIR}/conf/doxygen/Doxyfile.part.template
                 ${CMAKE_BINARY_DIR}/dox/Doxyfile.part IMMEDIATE)
  add_custom_target(dox COMMAND ${DOXYGEN_EXE} ${CMAKE_BINARY_DIR}/dox/Doxyfile.part)
endif ()
