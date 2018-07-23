#.rst:
# IncludeUrl
# ----------
#
# Adds the :command:`include_url` command that useful to download and include
# other CMake modules from a given url.
#
# .. command:: include_url
#
# Downloads a file from given url and includes it::
#
#  include_url(<url>                 # Url to be downloaded
#     [DESTINATION <destination>]    # Where the file will be saved
#     [EXPECTED_HASH <ALGO=value>]   # Verify downloaded file's hash
#     [EXPECTED_MD5 <sum>]           # Short-hand for "EXPECTED_HASH MD5=sum"
#     [DOWNLOAD_ONCE]                # Download the file only once
#     [DOWNLOAD_ALWAYS]              # Download the file every time
#     [OPTIONAL]                     # Do not fail file cannot be included
#     [RESULT_VARIABLE <variable>]   # The local path for the file included
#     [RETRIES <retries>]            # Try download <retries> times (default 3)
#     [QUIET]                        # Don't print anything
#    #--Download arguments-----------
#     [INACTIVITY_TIMEOUT <timeout>] # Timeout after <timeout> seconds of inactivity
#     [TIMEOUT <timeout>]            # Timeout after <timeout> seconds
#     [STATUS <status>]              # Download status variable
#     [LOG <log>]                    # Download log variable
#     [SHOW_PROGRESS]                # Show download progress
#     [TLS_VERIFY <on|off>]          # Check certificates
#     [TLS_CAINFO <file>]            # Custom Certificate Authority file
#    #--Include arguments------------
#     [NO_POLICY_SCOPE]              # Do not manage a new policy entry
#     )
#
# The ``include_url`` macro downloads a file from given url and includes it.
# It works both in -P script mode and when configuring a CMakeLists.txt file.
#
# If ``DESTINATION`` is specified, the file is saved at the given location with
# the original  file name, if ``<destination>`` is a directory, or with the
# given file  name, if ``<destination>`` is a file name.
#
# The arguments ``EXPECTED_HASH``, ``EXPECTED_MD5`` are used to ensure that the
# file included is the one expected. If the ``<url>`` is a local file (i.e.
# starts with ``file://``) the hash check is performed also on the file
# converted to the non-native end-of-line style.  See the documentation of the
# :command:`file()` command for further information about these arguments.
#
# If the ``DOWNLOAD_ONCE`` option is specified, the file is not
# downloaded if the file already exists and the hash is correct.
# If the ``DOWNLOAD_ALWAYS`` option is specified, the file is downloaded at
# every CMake execution, and an error is raised on failure.
# If none of these two option is specifies, the default behaviour is to try to
# download the file at every CMake execution, but no error is raised if the
# download fails if a version of the file already exists. This is useful when
# CMake should try to update the file to the latest version, before including
# it.
#
# If the ``OPTIONAL`` option is specified, no error will be caused if for any
# reason the file cannot be downloaded or included.
# If ``RESULT_VARIABLE`` is given, the variable will be set to the full
# filename which has been downloaded and included or NOTFOUND if it failed.
# See the documentation of the :command:`file()` command for further information
# about these arguments.
#
# If the ``RETRIES`` option is specified, the download will be tried
# If the ``QUIET`` option is specified, the command will emit no output.
#
# The arguments ``INACTIVITY_TIMEOUT``, ``TIMEOUT``, ``STATUS``, ``LOG``,
# ``SHOW_PROGRESS``, ``TLS_VERIFY``, and ``TLS_CAINFO`` are passed to the
# :command:`file(DOWNLOAD)` command.  See the documentation of the
# :command:`file()` command for a detailed description of these arguments.
#
# The arguments ``NO_POLICY_SCOPE`` is passed to the :command:`include()`
# command.  See the documentation of the :command:`include()` and
# :command:`cmake_policy()` commands for a detailed description of this
# argument.

#=============================================================================
# Copyright 2013-2015 Istituto Italiano di Tecnologia (IIT)
# Copyright 2013-2015 Daniele E. Domenichelli <ddomenichelli@drdanz.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)


if(DEFINED __INCLUDE_URL_INCLUDED)
  return()
endif()
set(__INCLUDE_URL_INCLUDED TRUE)


include(CMakeParseArguments)

# This must be a macro and not a function in order not to enclose in a
# new scope the variables added by the included files.
macro(INCLUDE_URL _remoteFile)

  set(_downloadOptions SHOW_PROGRESS)
  set(_downloadOneValueArgs INACTIVITY_TIMEOUT
                            TIMEOUT
                            LOG
                            EXPECTED_HASH
                            EXPECTED_MD5
                            TLS_VERIFY
                            TLS_CAINFO
                            RETRIES)
  set(_includeOptions NO_POLICY_SCOPE)
  set(_includeOneValueArgs RESULT_VARIABLE)

  set(_options DOWNLOAD_ONCE
               DOWNLOAD_ALWAYS
               QUIET
               OPTIONAL
               ${_downloadOptions}
               ${_includeOptions})
  set(_oneValueArgs DESTINATION
                    STATUS
                    ${_downloadOneValueArgs}
                    ${_includeOneValueArgs})
  set(_multiValueArgs )

  cmake_parse_arguments(_IU "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" "${ARGN}")

  if(DEFINED _IU_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown arguments:\n  ${_IU_UNPARSED_ARGUMENTS}\n")
  endif()

  if(_IU_DOWNLOAD_ONCE AND _IU_DOWNLOAD_ALWAYS)
    message(FATAL_ERROR "DOWNLOAD_ONCE and DOWNLOAD_ALWAYS cannot be specified at the same time")
  endif()

  if(NOT DEFINED _IU_RETRIES)
    set(_retries 3)
  elseif(NOT "${_IU_RETRIES}" MATCHES "^[1-9][0-9]*")
    message(FATAL_ERROR "RETRIES argument should be a number greater ot equal than 1. Found \"${_IU_RETRIES}\"")
  else()
    set(_retries ${_IU_RETRIES})
  endif()

  # Find a suitable temp folder (used only in script mode)
  if(DEFINED CMAKE_SCRIPT_MODE_FILE)
    set(_tempFolder "${CMAKE_CURRENT_LIST_DIR}")
    if(CMAKE_HOST_WIN32)
      if(IS_DIRECTORY "$ENV{TMP}")
        set(_tempFolder "$ENV{TMP}")
      elseif(IS_DIRECTORY "$ENV{TEMP}")
        set(_tempFolder "$ENV{TEMP}")
      endif()
    elseif(CMAKE_HOST_APPLE)
      if(IS_DIRECTORY "$ENV{TMPDIR}")
        set(_tempFolder "$ENV{TMPDIR}")
      endif()
    else()
      if(IS_DIRECTORY "/tmp")
        set(_tempFolder "/tmp")
      endif()
    endif()
  endif()

  get_filename_component(_filename ${_remoteFile} NAME)
  if(DEFINED _IU_DESTINATION)
    if(IS_DIRECTORY ${_IU_DESTINATION})
      set(_localFile "${_IU_DESTINATION}/${_filename}")
    else()
      set(_localFile "${_IU_DESTINATION}")
    endif()
  elseif(DEFINED CMAKE_SCRIPT_MODE_FILE)
    set(_localFile "${_tempFolder}/${_filename}")
  else()
    set(_localFile ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${_filename})
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${_localFile}")
  endif()
  set(_lockFile "${_localFile}.cmake")

  if(DEFINED CMAKE_SCRIPT_MODE_FILE)
    string(RANDOM LENGTH 8 _rand)
    set(_bakDir "${_tempFolder}/${_rand}/")
  else()
    set(_bakDir ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/)
  endif()
  set(_bakFile ${_bakDir}${_filename})

  set(_downloadArgs STATUS _downloadResult)
  foreach(_arg ${_downloadOptions})
    if(_IU_${_arg})
      list(APPEND _downloadArgs ${_arg})
    endif()
  endforeach()
  foreach(_arg ${_downloadOneValueArgs})
    # If the file is local, the hash check is disabled, since this could fail
    # because of different end of line styles
    if(DEFINED _IU_${_arg} AND NOT ("${_remoteFile}" MATCHES "^file://" AND "${_arg}" MATCHES "^(EXPECTED_HASH|EXPECTED_MD5)$"))
      list(APPEND _downloadArgs ${_arg} ${_IU_${_arg}})
    endif()
  endforeach()

  set(_includeArgs )
  foreach(_arg OPTIONAL ${_includeOptions})
    if(_IU_${_arg})
      list(APPEND _includeArgs ${_arg})
    endif()
  endforeach()
  foreach(_arg ${_includeOneValueArgs})
    if(DEFINED _IU_${_arg})
      list(APPEND _includeArgs ${_arg} ${_IU_${_arg}})
    endif()
  endforeach()

  set(_shouldCheckHash 0)
  unset(_algorithm)
  unset(_expectedHash)
  if(DEFINED _IU_EXPECTED_HASH  OR  DEFINED _IU_EXPECTED_MD5)
    set(_shouldCheckHash 1)
    if(DEFINED _IU_EXPECTED_HASH)
      if("${_IU_EXPECTED_HASH}" MATCHES "^(.+)=([0-9a-fA-F]+)$")
        set(_algorithm ${CMAKE_MATCH_1})
        set(_expectedHash ${CMAKE_MATCH_2})
      else()
        message(FATAL_ERROR "include_url EXPECTED_HASH expects ALGO=value but got: ${_IU_EXPECTED_HASH}")
      endif()
      if(NOT ${_algorithm} MATCHES "^(MD5|SHA1|SHA224|SHA256|SHA384|SHA512)$")
        message(FATAL_ERROR "include_url EXPECTED_HASH given unknown ALGO: ${_algorithm}")
      endif()
    else()
      set(_algorithm MD5)
      set(_expectedHash ${_IU_EXPECTED_MD5})
    endif()
  endif()

  # Lock the file, in case 2 different processes are downloading the same file
  # at the time
  # file(LOCK) was added in CMake 3.2, therefore calling it in older version
  # will fail.
  if(NOT CMAKE_VERSION VERSION_LESS 3.2)
    file(LOCK "${_lockFile}")
  endif()

  set(_shouldDownload 0)
  set(_shouldFail 0)
  if(NOT EXISTS "${_localFile}" OR _IU_DOWNLOAD_ALWAYS)
    set(_shouldDownload 1)
    set(_shouldFail 1)
  elseif(_IU_DOWNLOAD_ONCE)
    if(_shouldCheckHash)
      file(${_algorithm} "${_localFile}" _hash)
      if(NOT "${_hash}" STREQUAL "${_expectedHash}")
        set(_shouldDownload 1)
        set(_shouldFail 1)
      endif()
    endif()
  else()
    set(_shouldDownload 1)
  endif()

  if(_IU_OPTIONAL)
    set(_shouldFail 0)
  endif()

  if(_shouldDownload)
    if(EXISTS "${_localFile}" AND NOT _IU_DOWNLOAD_ALWAYS)
      # Unless DOWNLOAD_ALWAYS is specified, a copy of an eventual
      # existing file is saved and possibly restored later.
      file(MAKE_DIRECTORY ${_bakDir})
      file(RENAME "${_localFile}" "${_bakFile}")
    endif()


    set(_attempt 0)
    set(_succeeded 0)
    while(${_attempt} LESS ${_retries} AND NOT ${_succeeded})
      math(EXPR _attempt "${_attempt}+1")
      if(NOT _IU_QUIET)
        message(STATUS "Downloading ${_filename} - Attempt ${_attempt} of ${_retries}")
      endif()
      file(DOWNLOAD ${_remoteFile} "${_localFile}" ${_downloadArgs})

      # Set the LOG and the STATUS variables if requested by the user
      if(DEFINED _IU_LOG)
        set(${_IU_LOG} ${${_IU_LOG}})
      endif()

      if(DEFINED _IU_STATUS)
        set(${_IU_STATUS} ${_downloadResult})
      endif()

      unset(_error_message)
      list(GET _downloadResult 0 _downloadResult_0)

      if(NOT _downloadResult_0 EQUAL 0)
        list(GET _downloadResult 1 _downloadResult_1)
        set(_error_message "Downloading ${_filename} - ERROR ${_downloadResult_0}: ${_downloadResult_1}")
      else()
        # CMake does not give a fatal error if hash of the downloaded file
        # has a wrong hash. A new check is required in order not to include
        # a "faulty" file (it could be a security issue).
        if(_shouldCheckHash)
          file(${_algorithm} "${_localFile}" _hash)
          if(NOT "${_hash}" STREQUAL "${_expectedHash}")
            set(_error_message
"include_url HASH mismatch
   for file: [${_localFile}]
    expected hash: [${_expectedHash}]
      actual hash: [${_hash}]
")
            if(NOT ("${_remoteFile}" MATCHES "^file://" AND "${_arg}" MATCHES "^(EXPECTED_HASH|EXPECTED_MD5)$"))
              # Check again he hash using the non-native end-of-line style
              file(READ "${_localFile}" _content)
              if(CMAKE_HOST_WIN32)
                string(REPLACE "/r/n" "/n" _content "${_content}")
              else()
                string(REPLACE "/n" "/r/n" _content "${_content}")
              endif()
              string(${_algorithm} _hash "${_content}")
              if("${_hash}" STREQUAL "${_expectedHash}")
                # This is the same file but with changed end-of-line
                # Do not print the error message
                unset(_error_message)
              endif()
            endif()
          endif()
        endif()
      endif()
      if(NOT DEFINED _error_message)
        set(_succeeded 1)
      else()
        if(${_attempt} LESS ${_retries})
          if(NOT _IU_QUIET)
            message(STATUS "${_error_message}")
          endif()
          file(REMOVE "${_localFile}")
        endif()
      endif()
    endwhile()

    if(DEFINED _error_message)
      # There was a problem during download. Remove the faulty file to be sure
      # that we'll never include it
      file(REMOVE "${_localFile}")
      if(_shouldFail  OR  (NOT EXISTS "${_bakFile}" AND NOT _IU_OPTIONAL))
        # Remove backup file if exists
        file(REMOVE "${_bakFile}")
        message(FATAL_ERROR ${_error_message})
      else()
        if(NOT _IU_QUIET)
          message(STATUS ${_error_message})
        endif()
      endif()
    else()
      if(NOT _IU_QUIET)
        message(STATUS "Downloading ${_filename} - SUCCESS")
      endif()
    endif()

    # If download failed, restore the previous version of the file.
    if(NOT EXISTS "${_localFile}" AND EXISTS "${_bakFile}")
      file(RENAME "${_bakFile}" "${_localFile}")
    else()
      file(REMOVE "${_bakFile}")
    endif()
  else()
    if(DEFINED _IU_STATUS)
      set(${_IU_STATUS} "0")
    endif()
  endif()

  # Download is finished, we can now release the lock
  if(NOT CMAKE_VERSION VERSION_LESS 3.2)
    file(LOCK "${_lockFile}" RELEASE)
  endif()

  if(NOT EXISTS "${_localFile}" AND NOT _IU_OPTIONAL)
    message(FATAL_ERROR "Downloaded file does not exist. Please report this as a bug")
  endif()
  include("${_localFile}" ${_includeArgs})

  # Set the RESULT_VARIABLE variable if requested by the user
  if(DEFINED _IU_RESULT_VARIABLE)
    set(${_IU_RESULT_VARIABLE} ${${_IU_RESULT_VARIABLE}})
  endif()

  # Unset all used variables
  unset(_downloadOptions)
  unset(_downloadOneValueArgs)
  unset(_includeOptions)
  unset(_includeOneValueArgs)
  unset(_options)
  unset(_oneValueArgs)
  unset(_multiValueArgs)
  foreach(_arg "${_options}" "${_oneValueArgs}" "${_multiValueArgs}")
    unset(_IU_${_arg})
  endforeach()
  unset(_tempFolder)
  unset(_filename)
  unset(_localFile)
  unset(_bakDir)
  unset(_bakFile)
  unset(_downloadArgs)
  unset(_downloadResult)
  unset(_downloadResult_0)
  unset(_downloadResult_1)
  unset(_includeArgs)
  unset(_shouldCheckHash)
  unset(_algorithm)
  unset(_expectedHash)
  unset(_hash)
  unset(_shouldDownload)
  unset(_shouldFail)
  unset(_content)
  unset(_error_message)
  unset(_attempt)
  unset(_succeeded)
  unset(_retries)
endmacro()
