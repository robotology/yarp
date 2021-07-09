# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# this is set in the parent directory, but for some reason it does not get inherited
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)


set(CMAKE_SWIG_FLAGS "-module;yarp;${SWIG_COMMON_FLAGS}")
find_package(PerlLibs)

get_filename_component(_CMAKE_INSTALL_PERLDIR ${PERL_LIBRARY} DIRECTORY)
file(RELATIVE_PATH _CMAKE_INSTALL_PERLDIR "${_CMAKE_INSTALL_PERLDIR}" "${PERL_VENDORARCH}")
set(_CMAKE_INSTALL_PERLDIR "${CMAKE_INSTALL_LIBDIR}/${_CMAKE_INSTALL_PERLDIR}")
set(CMAKE_INSTALL_PERLDIR "${_CMAKE_INSTALL_PERLDIR}" CACHE PATH "perl bindings (${_CMAKE_INSTALL_PERLDIR})")
mark_as_advanced(CMAKE_INSTALL_PERLDIR)
if(NOT IS_ABSOLUTE ${CMAKE_INSTALL_PERLDIR})
  set(CMAKE_INSTALL_FULL_PERLDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_PERLDIR}")
else()
  set(CMAKE_INSTALL_FULL_PERLDIR "${CMAKE_INSTALL_PERLDIR}")
endif()

set(CMAKE_SWIG_OUTDIR "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PERLDIR}")
set(SWIG_OUTFILE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

swig_add_library(yarp_perl
                 LANGUAGE perl5
                 SOURCES ${SWIG_BINDINGS_SOURCE_FILE})
swig_link_libraries(yarp_perl ${SWIG_YARP_LIBRARIES})
target_include_directories(${SWIG_MODULE_yarp_perl_REAL_NAME} SYSTEM PRIVATE ${PERL_INCLUDE_PATH})

set_target_properties(${SWIG_MODULE_yarp_perl_REAL_NAME} PROPERTIES PREFIX ""
                                                                    OUTPUT_NAME "yarp"
                                                                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PERLDIR}/auto/yarp")


# Update RPATH
if(NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH)
  file(RELATIVE_PATH _rel_path "${CMAKE_INSTALL_FULL_PERLDIR}/auto/yarp" "${CMAKE_INSTALL_FULL_LIBDIR}")
  get_target_property(_current_rpath "${SWIG_MODULE_yarp_perl_REAL_NAME}" INSTALL_RPATH)
  if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    list(APPEND _current_rpath "@loader_path/${_rel_path}")
  else()
    list(APPEND _current_rpath "\$ORIGIN/${_rel_path}")
  endif()
  set_target_properties(${SWIG_MODULE_yarp_perl_REAL_NAME} PROPERTIES INSTALL_RPATH "${_current_rpath}")
endif()

install(TARGETS ${SWIG_MODULE_yarp_perl_REAL_NAME}
        DESTINATION ${CMAKE_INSTALL_PERLDIR}/auto/yarp)
install(FILES ${CMAKE_SWIG_OUTDIR}/yarp.pm
        DESTINATION ${CMAKE_INSTALL_PERLDIR})
