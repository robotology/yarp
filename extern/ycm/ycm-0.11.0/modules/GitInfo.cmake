#.rst:
# GitInfo
# -------
#
# Extract information from a git repository.
#
#
# .. command :: git_commit_info
#
#  Extract information about one commit from one git repository in
#  ``SOURCE DIR``::
#
#    git_commit_info([SOURCE DIR <dir>]
#                    [PREFIX <prefix>]
#                    [REVISION <rev>]
#                    [FATAL])
#
#  If ``SOURCE_DIR`` is a git repository, it checks the given
#  ``REVISION`` and sets the following variables:
#
#  ``<PREFIX>_GIT_COMMIT_DESCRIBE``
#    The output of ``git describe <ref>``.
#  ``<PREFIX>_GIT_COMMIT_DESCRIBE_CONTAINS``
#    The output of ``git describe --contains <ref>``.
#  ``<PREFIX>_GIT_COMMIT_TAG``
#    The most recent tag that is reachable from a commit.
#  ``<PREFIX>_GIT_COMMIT_REVISION``
#    The number of commits since the beginning of the git history.
#  ``<PREFIX>_GIT_COMMIT_TAG_REVISION``
#    The number of commits since the last tag.
#  ``<PREFIX>_GIT_COMMIT_DATE_REVISION``
#    The number of commits since the beginning of the day.
#  ``<PREFIX>_GIT_COMMIT_AUTHOR_DATE``
#    The commit author date.
#  ``<PREFIX>_GIT_COMMIT_AUTHOR_TIME``
#    The commit author time.
#  ``<PREFIX>_GIT_COMMIT_AUTHOR_TZ``
#    The commit author time zone.
#  ``<PREFIX>_GIT_COMMIT_AUTHOR_NAME``
#    The commit author name.
#  ``<PREFIX>_GIT_COMMIT_AUTHOR_EMAIL``
#    The commit author e-mail.
#  ``<PREFIX>_GIT_COMMIT_COMMITTER_DATE``
#    The commit committer date.
#  ``<PREFIX>_GIT_COMMIT_COMMITTER_TIME``
#    The commit author time.
#  ``<PREFIX>_GIT_COMMIT_COMMITTER_TZ``
#    The commit author time zone.
#  ``<PREFIX>_GIT_COMMIT_COMMITTER_NAME``
#    The commit committer name.
#  ``<PREFIX>_GIT_COMMIT_COMMITTER_EMAIL``
#    The commit committer email.
#  ``<PREFIX>_GIT_COMMIT_HASH``
#    The commit hash.
#  ``<PREFIX>_GIT_COMMIT_HASH_SHORT``
#    The abbreviated commit hash.
#  ``<PREFIX>_GIT_COMMIT_SUBJECT``
#    The commit log message subject line.
#  ``<PREFIX>_GIT_COMMIT_BODY``
#    The commit log message body.
#
#  If ``SOURCE_DIR`` is not set, then the ``PROJECT_SOURCE_DIR`` cmake
#  variable is used.
#
#  If ``PREFIX`` is not set, then the ``PROJECT_NAME`` cmake variable is
#  used.
#
#  ``REVISION`` can be a commit hash, a tag, a branch, or anything that
#  git can parse as a revision. If ``REVISION` is not set, then ``HEAD``
#  is used.
#
#  If ``FATAL`` is set, a fatal error is emitted when the source dir
#  is not a git repository, or when git was not found. This is disabled
#  by default to allow downloads from non-git sources (archives,
#  wrappers, etc.), but can be enabled if required.
#
#
#
#
# .. command :: git_wt_info
#
#  Extract information about current working tree from one git
#  repository in ``SOURCE DIR``::
#
#    git_wt_info([SOURCE DIR <dir>]
#                [PREFIX <prefix>]
#                [FATAL])
#
#  If ``SOURCE_DIR`` is a git repository, it checks current revision and
#  sets the following variables:
#
#  ``<PREFIX>_GIT_WT_DESCRIBE``
#    The output of ``git describe HEAD``.
#  ``<PREFIX>_GIT_WT_DESCRIBE_CONTAINS``
#    The output of ``git describe --contains HEAD``.
#  ``<PREFIX>_GIT_WT_TAG``
#    The most recent tag that is reachable from current commit.
#  ``<PREFIX>_GIT_WT_REVISION``
#    The number of commits since the beginning of the git history.
#  ``<PREFIX>_GIT_WT_TAG_REVISION``
#    The number of commits since the last tag.
#  ``<PREFIX>_GIT_WT_DATE_REVISION``
#    The number of commits since the beginning of the day.
#  ``<PREFIX>_GIT_WT_AUTHOR_DATE``
#    The current commit author date.
#  ``<PREFIX>_GIT_WT_AUTHOR_TIME``
#    The current commit author time.
#  ``<PREFIX>_GIT_WT_AUTHOR_TZ``
#    The current commit author time zone.
#  ``<PREFIX>_GIT_WT_AUTHOR_NAME``
#    The current commit author name.
#  ``<PREFIX>_GIT_WT_AUTHOR_EMAIL``
#    The current commit author e-mail.
#  ``<PREFIX>_GIT_WT_COMMITTER_DATE``
#    The current commit committer date.
#  ``<PREFIX>_GIT_WT_COMMITTER_TIME``
#    The current commit author time.
#  ``<PREFIX>_GIT_WT_COMMITTER_TZ``
#    The current commit author time zone.
#  ``<PREFIX>_GIT_WT_COMMITTER_NAME``
#    The current commit committer name.
#  ``<PREFIX>_GIT_WT_COMMITTER_EMAIL``
#    The current commit committer email.
#  ``<PREFIX>_GIT_WT_HASH``
#    The current commit hash.
#  ``<PREFIX>_GIT_WT_HASH_SHORT``
#    The abbreviated commit hash.
#  ``<PREFIX>_GIT_WT_SUBJECT``
#    The current commit log message subject line.
#  ``<PREFIX>_GIT_WT_BODY``
#    The current commit log message body.
#  ``<PREFIX>_GIT_WT_DIRTY``
#    Whether the current working tree is clean or not.
#
#
#  If ``SOURCE_DIR`` is not set, then the ``PROJECT_SOURCE_DIR`` cmake
#  variable is used.
#
#  If ``PREFIX`` is not set, then the ``PROJECT_NAME`` cmake variable is
#  used.
#
#  If ``FATAL`` is set, a fatal error is emitted when the source dir
#  is not a git repository, or when git was not found. This is disabled
#  by default to allow downloads from non-git sources (archives,
#  wrappers, etc.), but can be enabled if required.

#=============================================================================
# Copyright 2000-2014 Kitware, Inc.
# Copyright 2014 Istituto Italiano di Tecnologia (IIT)
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)


if(DEFINED __GIT_INFO_INCLUDED)
  return()
endif()
set(__GIT_INFO_INCLUDED 1)

include(CMakeParseArguments)

macro(_check_git_and_repo _source_dir _fatal)
  # Check repository
  if(NOT EXISTS "${_source_dir}/.git")
    # This is not a git folder.
    if(${_fatal})
      message(FATAL_ERROR "Source dir \"${_source_dir}\" is not a git repository.")
    else()
      if(GIT_INFO_DEBUG)
        message(STATUS "Source dir \"${_source_dir}\" is not a git repository.")
      endif()
      return()
    endif()
  endif()

  # Check Git executable
  find_package(Git QUIET)
  if(NOT GIT_FOUND)
    # Cannot extract version without git
    if(${_fatal})
      message(FATAL_ERROR "Git not found. Cannot extract version without git.")
    else()
      if(GIT_INFO_DEBUG)
        message(STATUS "Git not found. Cannot extract version without git.")
      endif()
      return()
    endif()
  endif()
endmacro()

function(git_commit_info)

  # Parse arguments and check values
  set(_options FATAL)
  set(_oneValueArgs SOURCE_DIR
                    PREFIX
                    REVISION)
  set(_multiValueArgs)
  cmake_parse_arguments(_GCI "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" "${ARGN}")

  if(NOT DEFINED _GCI_SOURCE_DIR)
    if(NOT DEFINED PROJECT_NAME)
      message(FATAL_ERROR "SOURCE_DIR parameter is missing and project not defined.")
    endif()
  endif()

  if(NOT DEFINED _GCI_PREFIX)
    if(NOT DEFINED PROJECT_NAME)
      message(AUTHOR_WARNING "PREFIX parameter is missing and project not defined. The variable prefix will be empty.")
    endif()
  endif()

  if(NOT DEFINED _GCI_REVISION)
    set(_GCI_REVISION HEAD)
  endif()

  if(DEFINED _GCI_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown arguments: ${_GCI_UNPARSED_ARGUMENTS}")
  endif()

  # Check repository
  _check_git_and_repo("${_GCI_SOURCE_DIR}" ${_GCI_FATAL})

  # Check revision and get HASH
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --verify -q ${_GCI_REVISION}
    OUTPUT_VARIABLE ${_GCI_PREFIX}_GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}"
    RESULT_VARIABLE err
    ERROR_QUIET)
  if(err)
    if(_GCI_FATAL)
      message(FATAL_ERROR "\"${_GCI_REVISION}\" is not a valid revision.")
    else()
      message(AUTHOR_WARNING "\"${_GCI_REVISION}\" is not a valid revision.")
      return()
    endif()
  endif()

  # Get HASH_SHORT
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --verify -q --short ${_GCI_REVISION}
    OUTPUT_VARIABLE ${_GCI_PREFIX}_GIT_COMMIT_HASH_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}")

  # Get DESCRIBE
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe ${_GCI_REVISION}
    OUTPUT_VARIABLE ${_GCI_PREFIX}_GIT_COMMIT_DESCRIBE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}"
    RESULT_VARIABLE err
    ERROR_QUIET)
  if(err)
    set(${_GCI_PREFIX}_GIT_COMMIT_DESCRIBE "")
  endif()

  # Get DESCRIBE_CONTAINS
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --contains ${_GCI_REVISION}
    OUTPUT_VARIABLE ${_GCI_PREFIX}_GIT_COMMIT_DESCRIBE_CONTAINS
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}"
    RESULT_VARIABLE err
    ERROR_QUIET)
  if(err)
    set(${_GCI_PREFIX}_GIT_COMMIT_DESCRIBE_CONTAINS "")
  endif()

  # Get TAG
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0 ${_GCI_REVISION}
    OUTPUT_VARIABLE ${_GCI_PREFIX}_GIT_COMMIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}"
    RESULT_VARIABLE err
    ERROR_QUIET)
  if(err)
    set(${_GCI_PREFIX}_GIT_COMMIT_TAG "")
  endif()

  # Get REVISION
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-list --count ${_GCI_REVISION}
    OUTPUT_VARIABLE ${_GCI_PREFIX}_GIT_COMMIT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}")

  # Get TAG_REVISION
  if("${${_GCI_PREFIX}_GIT_COMMIT_TAG}" STREQUAL "")
    set(${_GCI_PREFIX}_GIT_COMMIT_TAG_REVISION "${${_GCI_PREFIX}_GIT_COMMIT_REVISION}")
  endif()
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-list --count ${${_GCI_PREFIX}_GIT_COMMIT_TAG}..${_GCI_REVISION}
    OUTPUT_VARIABLE ${_GCI_PREFIX}_GIT_COMMIT_TAG_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}")

  # Get AUTHOR and COMMITTER info
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log -1 --date=iso --pretty=fuller ${_GCI_REVISION}
    OUTPUT_VARIABLE _log_message
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}")

  string(REGEX MATCH "AuthorDate: +([0-9-]+) ([0-9:]+) ([+-][0-9]+)" _unused ${_log_message})
  set(${_GCI_PREFIX}_GIT_COMMIT_AUTHOR_DATE "${CMAKE_MATCH_1}")
  set(${_GCI_PREFIX}_GIT_COMMIT_AUTHOR_TIME "${CMAKE_MATCH_2}")
  set(${_GCI_PREFIX}_GIT_COMMIT_AUTHOR_TZ "${CMAKE_MATCH_3}")

  string(REGEX MATCH "Author: +([^\n]+) <([^\n]+)>" _unused ${_log_message})
  set(${_GCI_PREFIX}_GIT_COMMIT_AUTHOR_NAME "${CMAKE_MATCH_1}")
  set(${_GCI_PREFIX}_GIT_COMMIT_AUTHOR_EMAIL "${CMAKE_MATCH_2}")

  string(REGEX MATCH "CommitDate: +([0-9-]+) ([0-9:]+) ([+-][0-9]+)" _unused ${_log_message})
  set(${_GCI_PREFIX}_GIT_COMMIT_COMMITTER_DATE "${CMAKE_MATCH_1}")
  set(${_GCI_PREFIX}_GIT_COMMIT_COMMITTER_TIME "${CMAKE_MATCH_2}")
  set(${_GCI_PREFIX}_GIT_COMMIT_COMMITTER_TZ "${CMAKE_MATCH_3}")

  string(REGEX MATCH "Commit: +([^\n]+) <([^\n]+)>" _unused ${_log_message})
  set(${_GCI_PREFIX}_GIT_COMMIT_COMMITTER_NAME "${CMAKE_MATCH_1}")
  set(${_GCI_PREFIX}_GIT_COMMIT_COMMITTER_EMAIL "${CMAKE_MATCH_2}")

  # Get DATE_REVISION
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-list --count "--since=\"${${_GCI_PREFIX}_GIT_COMMIT_AUTHOR_DATE} 1 day\"" ${_GCI_REVISION}
    OUTPUT_VARIABLE ${_GCI_PREFIX}_GIT_COMMIT_DATE_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}")

  # Get SUBJECT
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log -1 --format=%s ${_GCI_REVISION}
    OUTPUT_VARIABLE ${_GCI_PREFIX}_GIT_COMMIT_SUBJECT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}")

  # Get BODY
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log -1 --format=%b ${_GCI_REVISION}
    OUTPUT_VARIABLE ${_GCI_PREFIX}_GIT_COMMIT_BODY
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GCI_SOURCE_DIR}")

  # Export variables to the parent scope
  foreach(_var DESCRIBE
               DESCRIBE_CONTAINS
               TAG
               REVISION
               TAG_REVISION
               DATE_REVISION
               AUTHOR_DATE
               AUTHOR_TIME
               AUTHOR_TZ
               AUTHOR_NAME
               AUTHOR_EMAIL
               COMMITTER_DATE
               COMMITTER_TIME
               COMMITTER_TZ
               COMMITTER_NAME
               COMMITTER_EMAIL
               HASH
               HASH_SHORT
               SUBJECT
               BODY)
    set(${_GCI_PREFIX}_GIT_COMMIT_${_var} "${${_GCI_PREFIX}_GIT_COMMIT_${_var}}" PARENT_SCOPE)
    if(GIT_INFO_DEBUG OR GIT_INFO_DEBUG_${_GCI_PREFIX})
      message(STATUS "${_GCI_PREFIX}_GIT_COMMIT_${_var} = \"${${_GCI_PREFIX}_GIT_COMMIT_${_var}}\"")
    endif()
  endforeach()
endfunction()


function(git_wt_info)
  # Parse arguments and check values
  set(_options FATAL)
  set(_oneValueArgs SOURCE_DIR
                    PREFIX)
  set(_multiValueArgs)
  cmake_parse_arguments(_GWTI "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" "${ARGN}")

  if(NOT DEFINED _GWTI_SOURCE_DIR)
    if(NOT DEFINED PROJECT_NAME)
      message(FATAL_ERROR "SOURCE_DIR parameter is missing and project not defined.")
    endif()
  endif()

  if(NOT DEFINED _GWTI_PREFIX)
    if(NOT DEFINED PROJECT_NAME)
      message(AUTHOR_WARNING "PREFIX parameter is missing and project not defined. The variable prefix will be empty.")
    endif()
  endif()

  if(DEFINED _GWTI_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown arguments: ${_GWTI_UNPARSED_ARGUMENTS}")
  endif()

  unset(_fatal)
  if(_GWTI_FATAL)
    set(${_fatal} FATAL)
  endif()

  # Check repository
  _check_git_and_repo("${_GWTI_SOURCE_DIR}" ${_GWTI_FATAL})

  # Get info about the HEAD commit
  git_commit_info(SOURCE_DIR "${_GWTI_SOURCE_DIR}"
                  PREFIX ${_GWTI_PREFIX}
                  REVISION HEAD
                  ${_fatal})
  foreach(_var DESCRIBE
               DESCRIBE_CONTAINS
               TAG
               REVISION
               TAG_REVISION
               DATE_REVISION
               AUTHOR_DATE
               AUTHOR_TIME
               AUTHOR_TZ
               AUTHOR_NAME
               AUTHOR_EMAIL
               COMMITTER_DATE
               COMMITTER_TIME
               COMMITTER_TZ
               COMMITTER_NAME
               COMMITTER_EMAIL
               HASH
               HASH_SHORT
               SUBJECT
               BODY)
    set(${_GWTI_PREFIX}_GIT_WT_${_var} "${${_GWTI_PREFIX}_GIT_COMMIT_${_var}}")
  endforeach()

  # Get more info
  set(${_GWTI_PREFIX}_GIT_WT_DIRTY 0)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} update-index -q --refresh
    WORKING_DIRECTORY "${_GWTI_SOURCE_DIR}")

  execute_process(
    COMMAND ${GIT_EXECUTABLE} diff-index --name-only HEAD --
    OUTPUT_VARIABLE _dirty
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${_GWTI_SOURCE_DIR}")
  if(_dirty)
    set(${_GWTI_PREFIX}_GIT_WT_DIRTY 1)
  endif()

  # Export variables to the parent scope

  foreach(_var DESCRIBE
               DESCRIBE_CONTAINS
               TAG
               REVISION
               TAG_REVISION
               DATE_REVISION
               AUTHOR_DATE
               AUTHOR_TIME
               AUTHOR_TZ
               AUTHOR_NAME
               AUTHOR_EMAIL
               COMMITTER_DATE
               COMMITTER_TIME
               COMMITTER_TZ
               COMMITTER_NAME
               COMMITTER_EMAIL
               HASH
               HASH_SHORT
               SUBJECT
               BODY
               DIRTY)
    set(${_GWTI_PREFIX}_GIT_WT_${_var} "${${_GWTI_PREFIX}_GIT_WT_${_var}}" PARENT_SCOPE)
    if(GIT_INFO_DEBUG OR GIT_INFO_DEBUG_${_GWTI_PREFIX})
      message(STATUS "${_GWTI_PREFIX}_GIT_WT_${_var} = \"${${_GWTI_PREFIX}_GIT_WT_${_var}}\"")
    endif()
  endforeach()

endfunction()
