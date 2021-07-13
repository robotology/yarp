# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

include(CMakeDependentOption)

find_file(DOXYGEN_PLANTUM_JAR
  NAMES plantuml.jar
  PATHS /usr/share
  PATH_SUFFIXES plantuml
  DOC "PlantUML tool for using Doxygen"
)
if(NOT DOXYGEN_PLANTUM_JAR)
  set(DOXYGEN_PLANTUM_JAR "")
endif()

if(DOXYGEN_FOUND)

  if("${DOXYGEN_VERSION}" MATCHES "([0-9]+)\.([0-9]+)\.([0-9]+)")
    set(DOXYGEN_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(DOXYGEN_VERSION_MINOR ${CMAKE_MATCH_2})
    set(DOXYGEN_VERSION_PATCH ${CMAKE_MATCH_3})
  else()
    set(DOXYGEN_VERSION_MAJOR 0)
    set(DOXYGEN_VERSION_MINOR 0)
    set(DOXYGEN_VERSION_PATCH 0)
  endif()

  option(YARP_DOXYGEN_HTML "Generate doxygen HTML output" ON)
  cmake_dependent_option(YARP_DOXYGEN_HTML_DOCSET "Generate doxygen docset output (for Apple's Xcode 3)" OFF
                         YARP_DOXYGEN_HTML OFF)
  cmake_dependent_option(YARP_DOXYGEN_HTML_HTMLHELP "Generate doxygen htmlhelp output (for Microsoft's HTML Help Workshop)" OFF
                         YARP_DOXYGEN_HTML OFF)
  cmake_dependent_option(YARP_DOXYGEN_HTML_QHP "Generate doxygen Qt Compressed Help output (for QtCreator and KDevelop)" OFF
                         YARP_DOXYGEN_HTML OFF)
  cmake_dependent_option(YARP_DOXYGEN_HTML_ECLIPSEHELP "Generate doxygen Eclipse help plugin output (for Eclipse)" OFF
                         YARP_DOXYGEN_HTML OFF)
  option(YARP_DOXYGEN_LATEX "Generate doxygen LATEX output" OFF)
  option(YARP_DOXYGEN_XML "Generate doxygen XML output, for use by SWIG" OFF)
  option(YARP_DOXYGEN_RTF "Generate doxygen RTF output" OFF)
  option(YARP_DOXYGEN_TAGFILE "Generate doxygen tag file (see http://www.doxygen.nl/manual/external.html)" OFF)
  option(YARP_DOXYGEN_DOCBOOK "Generate doxygen DOCBOOK output" OFF)
  option(YARP_DOXYGEN_MAN "Generate doxygen MAN output" OFF)

  mark_as_advanced(
    YARP_DOXYGEN_HTML
    YARP_DOXYGEN_HTML_DOCSET
    YARP_DOXYGEN_HTML_HTMLHELP
    YARP_DOXYGEN_HTML_QHP
    YARP_DOXYGEN_HTML_ECLIPSEHELP
    YARP_DOXYGEN_LATEX
    YARP_DOXYGEN_XML
    YARP_DOXYGEN_RTF
    YARP_DOXYGEN_TAGFILE
    YARP_DOXYGEN_DOCBOOK
    YARP_DOXYGEN_MAN
  )

  set(DOX_FILE Doxyfile)
  set(DOX_DESTINATION "${CMAKE_BINARY_DIR}/dox")

  macro(_reset_variables)
    set(DOX_PATTERNS "*.h *.cpp *.dox *.md")
    set(DOX_GENERATE_HTML NO)
    set(DOX_GENERATE_DOCSET NO)
    set(DOX_GENERATE_HTMLHELP NO)
    set(DOX_GENERATE_QHP NO)
    set(DOX_GENERATE_ECLIPSEHELP NO)
    set(DOX_GENERATE_XML NO)
    set(DOX_GENERATE_MAN NO)
    set(DOX_GENERATE_LATEX NO)
    set(DOX_GENERATE_RTF NO)
    set(DOX_GENERATE_DOCBOOK NO)
    set(DOX_GENERATE_TAGFILE "")
    set(DOX_GENERATE_TODOLIST NO)
    set(DOX_GENERATE_TESTLIST NO)
    set(DOX_GENERATE_BUGLIST NO)
    set(DOX_GENERATE_DEPRECATEDLIST NO)
  endmacro()

  if(YARP_DOXYGEN_HTML OR
     YARP_DOXYGEN_LATEX OR
     YARP_DOXYGEN_XML OR
     YARP_DOXYGEN_RTF OR
     YARP_DOXYGEN_TAGFILE OR
     YARP_DOXYGEN_MAN OR
     YARP_DOXYGEN_DOCBOOK)
    add_custom_target(dox SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${DOX_FILE}.in")
    set_property(TARGET dox PROPERTY FOLDER "Documentation")


    # HTML Documentation
    if(YARP_DOXYGEN_HTML OR
       YARP_DOXYGEN_TAGFILE)

      # Prepare configuration for normal documentation
      _reset_variables()
      if(YARP_DOXYGEN_HTML)
        set(DOX_GENERATE_HTML YES)
        if(YARP_DOXYGEN_HTML_DOCSET)
          set(DOX_GENERATE_DOCSET YES)
        endif()
        if(YARP_DOXYGEN_HTML_HTMLHELP)
          set(DOX_GENERATE_HTMLHELP YES)
        endif()
        if(YARP_DOXYGEN_HTML_QHP)
          set(DOX_GENERATE_QHP YES)
        endif()
        if(YARP_DOXYGEN_HTML_ECLIPSEHELP)
          set(DOX_GENERATE_ECLIPSEHELP YES)
        endif()
      endif()
      if(YARP_DOXYGEN_TAGFILE)
        set(DOX_GENERATE_TAGFILE "${DOX_DESTINATION}/YARP.tag")
      endif()
      set(DOX_GENERATE_TODOLIST YES)
      set(DOX_GENERATE_TESTLIST YES)
      set(DOX_GENERATE_BUGLIST YES)
      set(DOX_GENERATE_DEPRECATEDLIST YES)

      set(DOX_WARN_LOGFILE "${CMAKE_CURRENT_BINARY_DIR}/doxygen.log")

      configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${DOX_FILE}.in"
                     "${CMAKE_CURRENT_BINARY_DIR}/${DOX_FILE}")
      add_custom_command(TARGET dox
                         COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/${DOX_FILE}"
                         COMMENT "Generating html documentation")
    endif()

    # XML
    if(YARP_DOXYGEN_XML)

      # Prepare configuration for xml documentation
      _reset_variables()
      set(DOX_GENERATE_XML YES)

      set(DOX_WARN_LOGFILE "${CMAKE_CURRENT_BINARY_DIR}/doxygen.xml.log")

      configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${DOX_FILE}.in"
                     "${CMAKE_CURRENT_BINARY_DIR}/${DOX_FILE}.xml")
      add_custom_command(TARGET dox
                         COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/${DOX_FILE}.xml"
                         COMMENT "Generating xml documentation")
    endif()

    # Man pages
    if(YARP_DOXYGEN_MAN)

      # Prepare configuration for man documentation
      _reset_variables()
      set(DOX_PATTERNS "cmd_*.dox")
      set(DOX_GENERATE_MAN YES)

      set(DOX_WARN_LOGFILE "${CMAKE_CURRENT_BINARY_DIR}/doxygen.man.log")

      configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DOX_FILE}.in
                     ${CMAKE_CURRENT_BINARY_DIR}/${DOX_FILE}.man)
      add_custom_command(TARGET dox
                         COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/${DOX_FILE}.man"
                         COMMENT "Generating man pages")
    endif()

    # Manual
    if(YARP_DOXYGEN_LATEX OR
       YARP_DOXYGEN_RTF OR
       YARP_DOXYGEN_DOCBOOK)

      # Prepare configuration for YARP manual
      _reset_variables()
      if(YARP_DOXYGEN_LATEX)
        set(DOX_GENERATE_LATEX YES)
      endif()
      if(YARP_DOXYGEN_RTF)
        set(DOX_GENERATE_RTF YES)
      endif()
      if(YARP_DOXYGEN_DOCBOOK)
        set(DOX_GENERATE_DOCBOOK YES)
      endif()

      set(DOX_WARN_LOGFILE "${CMAKE_CURRENT_BINARY_DIR}/doxygen.manual.log")

      configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${DOX_FILE}.in"
                     "${CMAKE_CURRENT_BINARY_DIR}/${DOX_FILE}.manual")
      add_custom_command(TARGET dox
                         COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/${DOX_FILE}.manual"
                         COMMENT "Generating manual")

    endif()

  endif()
endif()
