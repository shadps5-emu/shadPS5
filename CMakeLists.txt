# SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(shadPS5 CXX)

option(ENABLE_TESTS "Build unit tests (requires the externals/googletest submodule)" OFF)

# generate git revision information
include("${CMAKE_CURRENT_SOURCE_DIR}/externals/cmake-modules/GetGitRevisionDescription.cmake")
get_git_head_revision(GIT_REF_SPEC GIT_REV)
git_describe(GIT_DESC --always --long --dirty)
git_branch_name(GIT_BRANCH)
string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M:%S")

message("start git things")

# Try to get the upstream remote and branch
message("check for remote and branch")
execute_process(
  COMMAND git rev-parse --abbrev-ref --symbolic-full-name @{u}
  OUTPUT_VARIABLE GIT_REMOTE_NAME
  RESULT_VARIABLE GIT_REMOTE_RESULT
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# If there's no upstream set or the command failed, check remote.pushDefault
if (GIT_REMOTE_RESULT OR GIT_REMOTE_NAME STREQUAL "")
  message(STATUS "check default push")
  execute_process(
    COMMAND git config --get remote.pushDefault
    OUTPUT_VARIABLE GIT_REMOTE_NAME
    RESULT_VARIABLE GIT_REMOTE_RESULT
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message(STATUS "got remote: ${GIT_REMOTE_NAME}")
endif()

# If running in GitHub Actions and the above fails
if (GIT_REMOTE_RESULT OR GIT_REMOTE_NAME STREQUAL "")
  message(STATUS "check github")
  set(GIT_REMOTE_NAME "origin")

  # Retrieve environment variables
  if (DEFINED ENV{GITHUB_HEAD_REF} AND NOT "$ENV{GITHUB_HEAD_REF}" STREQUAL "")
    message(STATUS "github head ref: $ENV{GITHUB_HEAD_REF}")
    set(GITHUB_HEAD_REF "$ENV{GITHUB_HEAD_REF}")
  else()
    set(GITHUB_HEAD_REF "")
  endif()

  if (DEFINED ENV{GITHUB_REF} AND NOT "$ENV{GITHUB_REF}" STREQUAL "")
    message(STATUS "github ref: $ENV{GITHUB_REF}")
    string(REGEX REPLACE "^refs/[^/]*/" "" GITHUB_BRANCH "$ENV{GITHUB_REF}")
    string(REGEX MATCH "refs/pull/([0-9]+)/merge" MATCHED_REF "$ENV{GITHUB_REF}")
    if (MATCHED_REF)
      set(PR_NUMBER "${CMAKE_MATCH_1}")
      set(GITHUB_BRANCH "")
      message(STATUS "PR number: ${PR_NUMBER}")
    else()
      set(PR_NUMBER "")
    endif()
  else()
    set(GITHUB_BRANCH "")
    set(PR_NUMBER "")
  endif()

  if (NOT "${PR_NUMBER}" STREQUAL "" AND NOT "${GITHUB_HEAD_REF}" STREQUAL "")
    set(GIT_BRANCH "pr-${PR_NUMBER}-${GITHUB_HEAD_REF}")
  elseif (NOT "${PR_NUMBER}" STREQUAL "" AND NOT "${GITHUB_BRANCH}" STREQUAL "")
    set(GIT_BRANCH "pr-${PR_NUMBER}-${GITHUB_BRANCH}")
  elseif (NOT "${PR_NUMBER}" STREQUAL "")
    set(GIT_BRANCH "pr-${PR_NUMBER}")
  elseif ("${PR_NUMBER}" STREQUAL "" AND NOT "${GITHUB_HEAD_REF}" STREQUAL "")
    set(GIT_BRANCH "${GITHUB_HEAD_REF}")
  elseif ("${PR_NUMBER}" STREQUAL "" AND NOT "${GITHUB_BRANCH}" STREQUAL "")
    set(GIT_BRANCH "${GITHUB_BRANCH}")
  elseif ("${PR_NUMBER}" STREQUAL "" AND NOT "${GITHUB_REF}" STREQUAL "")
    set(GIT_BRANCH "${GITHUB_REF}")
  elseif("${GIT_BRANCH}" STREQUAL "")
    message(STATUS "couldn't find branch")
    set(GIT_BRANCH "detached-head")
  endif()
else()
  # Extract remote name if the output contains a remote/branch format
  string(FIND "${GIT_REMOTE_NAME}" "/" INDEX)
  if (INDEX GREATER -1)
    string(SUBSTRING "${GIT_REMOTE_NAME}" 0 "${INDEX}" GIT_REMOTE_NAME)
  elseif("${GIT_REMOTE_NAME}" STREQUAL "")
    message(STATUS "reset to origin")
    set(GIT_REMOTE_NAME "origin")
  endif()
endif()

# Get remote link
message(STATUS "getting remote link")
execute_process(
  COMMAND git config --get remote.${GIT_REMOTE_NAME}.url
  OUTPUT_VARIABLE GIT_REMOTE_URL
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set Version
set(EMULATOR_VERSION_MAJOR "0")
set(EMULATOR_VERSION_MINOR "0")
set(EMULATOR_VERSION_PATCH "1")

set_source_files_properties(src/dist/shadPS5.rc PROPERTIES COMPILE_DEFINITIONS "EMULATOR_VERSION_MAJOR=${EMULATOR_VERSION_MAJOR};EMULATOR_VERSION_MINOR=${EMULATOR_VERSION_MINOR};EMULATOR_VERSION_PATCH=${EMULATOR_VERSION_PATCH}")

set(APP_VERSION_NUM "${EMULATOR_VERSION_MAJOR}.${EMULATOR_VERSION_MINOR}.${EMULATOR_VERSION_PATCH}")
set(APP_VERSION "${APP_VERSION_NUM} WIP")
set(APP_IS_RELEASE false)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/common/scm_rev.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/src/common/scm_rev.cpp" @ONLY)

message("-- end git things, remote: ${GIT_REMOTE_NAME}, branch: ${GIT_BRANCH}, link: ${GIT_REMOTE_URL}")


# STUBS for logging lib,to be removed when the logging lib is fully implemented and integrated into the emulator
set(STUBS stubs/src/stubs.cpp
          stubs/include/common/assert.h
          stubs/include/common/thread.h
          stubs/include/core/emulator_settings.h
          stubs/include/core/file_sys/ifile.h
)

set(COMMON ${CMAKE_CURRENT_BINARY_DIR}/src/common/scm_rev.cpp
           src/common/logging/classes.cpp
           src/common/logging/classes.h
           src/common/logging/formatter.h
           src/common/logging/log.cpp
           src/common/logging/log.h
           src/common/logging/log_file_sink.h
           src/common/path_util.cpp
           src/common/path_util.h
           src/common/scm_rev.h
           src/common/types.h
)

set(EMULATOR src/main.cpp)

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(SHADPS5_WARNINGS /W4)
else()
    set(SHADPS5_WARNINGS -Wall -Wextra)
endif()

if (WIN32)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

add_subdirectory(externals)

if(NOT ENABLE_TESTS)

add_executable(shadPS5
    ${COMMON}
    ${EMULATOR}
    ${STUBS}
)

target_include_directories(shadPS5 PRIVATE src)
target_include_directories(shadPS5 PRIVATE stubs/include) #remove when stubs are removed
target_link_libraries(shadPS5 PRIVATE spdlog::spdlog)
target_link_libraries(shadPS5 PRIVATE fmt::fmt)
target_compile_options(shadPS5 PRIVATE ${SHADPS5_WARNINGS})


if (WIN32)
    target_sources(shadPS5 PRIVATE src/dist/shadPS5.rc)
endif()

install(TARGETS shadPS5 RUNTIME DESTINATION .)

else()
    enable_testing()
    add_subdirectory(tests)
endif()
