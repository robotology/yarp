# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(CMakeDependentOption)

find_package(Doxygen)

if(DOXYGEN_FOUND)
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

  mark_as_advanced(YARP_DOXYGEN_HTML
                   YARP_DOXYGEN_HTML_DOCSET
                   YARP_DOXYGEN_HTML_HTMLHELP
                   YARP_DOXYGEN_HTML_QHP
                   YARP_DOXYGEN_HTML_ECLIPSEHELP
                   YARP_DOXYGEN_LATEX
                   YARP_DOXYGEN_XML
                   YARP_DOXYGEN_RTF
                   YARP_DOXYGEN_TAGFILE
                   YARP_DOXYGEN_DOCBOOK
                   YARP_DOXYGEN_MAN)

  set(DOX_FILE Doxyfile)

  if(YARP_DOXYGEN_HTML OR
     YARP_DOXYGEN_LATEX OR
     YARP_DOXYGEN_XML OR
     YARP_DOXYGEN_RTF OR
     YARP_DOXYGEN_TAGFILE OR
     YARP_DOXYGEN_MAN OR
     YARP_DOXYGEN_DOCBOOK)
    add_custom_target(dox SOURCES "${YARP_MODULE_DIR}/template/${DOX_FILE}.in")

    if(YARP_DOXYGEN_HTML OR
       YARP_DOXYGEN_LATEX OR
       YARP_DOXYGEN_XML OR
       YARP_DOXYGEN_RTF OR
       YARP_DOXYGEN_TAGFILE OR
       YARP_DOXYGEN_DOCBOOK)

      # Prepare configuration for normal documentation
      set(DOX_PATTERNS "*.h *.cpp *.dox *.md")
      set(DOX_GENERATE_HTML NO)
      set(DOX_GENERATE_DOCSET NO)
      set(DOX_GENERATE_HTMLHELP NO)
      set(DOX_GENERATE_QHP NO)
      set(DOX_GENERATE_ECLIPSEHELP NO)
      set(DOX_GENERATE_LATEX NO)
      set(DOX_GENERATE_XML NO)
      set(DOX_GENERATE_RTF NO)
      set(DOX_GENERATE_DOCBOOK NO)
      set(DOX_GENERATE_TAGFILE "")
      set(DOX_GENERATE_MAN NO)

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
      if(YARP_DOXYGEN_LATEX)
        set(DOX_GENERATE_LATEX YES)
      endif()
      if(YARP_DOXYGEN_XML)
        set(DOX_GENERATE_XML YES)
      endif()
      if(YARP_DOXYGEN_RTF)
        set(DOX_GENERATE_RTF YES)
      endif()
      if(YARP_DOXYGEN_DOCBOOK)
        set(DOX_GENERATE_DOCBOOK YES)
      endif()
      if(YARP_DOXYGEN_TAGFILE)
        set(DOX_GENERATE_TAGFILE "${CMAKE_BINARY_DIR}/dox/YARP.tag")
      endif()

      configure_file("${YARP_MODULE_DIR}/template/${DOX_FILE}.in"
                     "${CMAKE_BINARY_DIR}/dox/${DOX_FILE}")
      add_custom_command(TARGET dox
                         COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_BINARY_DIR}/dox/${DOX_FILE}"
                         COMMENT "Generating doxygen documentation")
    endif()

    if(YARP_DOXYGEN_MAN)

      # Prepare configuration for man documentation
      set(DOX_PATTERNS "cmd_*.dox")
      set(DOX_GENERATE_HTML NO)
      set(DOX_GENERATE_HTML_DOCSET NO)
      set(DOX_GENERATE_HTML_HTMLHELP NO)
      set(DOX_GENERATE_HTML_QHP NO)
      set(DOX_GENERATE_HTML_ECLIPSEHELP NO)
      set(DOX_GENERATE_LATEX NO)
      set(DOX_GENERATE_XML NO)
      set(DOX_GENERATE_RTF NO)
      set(DOX_GENERATE_DOCBOOK NO)
      set(DOX_GENERATE_TAGFILE "")
      set(DOX_GENERATE_MAN YES)
      configure_file(${YARP_MODULE_DIR}/template/${DOX_FILE}.in
                     ${CMAKE_BINARY_DIR}/dox/${DOX_FILE}.man)
      add_custom_command(TARGET dox
                         COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_BINARY_DIR}/dox/${DOX_FILE}.man"
                         COMMENT "Generating man pages")
    endif()
  endif()
endif()
