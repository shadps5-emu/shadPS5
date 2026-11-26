# SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
# SPDX-License-Identifier: GPL-2.0-or-later

set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)
set_directory_properties(PROPERTIES
    EXCLUDE_FROM_ALL ON
    SYSTEM ON
)

# Set CMP0069 policy to "NEW" in order to ensure consistent behavior when building external targets with LTO enabled
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

if (MSVC)
    # Silence "deprecation" warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
endif()

# fmt
set(FMT_INSTALL ON CACHE BOOL "" FORCE)
set(FMT_MODULE OFF)
set(FMT_DOC OFF)
set(FMT_TEST OFF)
add_subdirectory(fmt)
set(fmt_DIR "${CMAKE_CURRENT_BINARY_DIR}/fmt" CACHE PATH "" FORCE)

# spdlog (v2.x branch)
set(SPDLOG_NO_EXCEPTIONS ON)
set(SPDLOG_DISABLE_GLOBAL_LOGGER ON)
if (TARGET fmt::fmt)
    set(SPDLOG_FMT_EXTERNAL ON)
endif()
add_subdirectory(spdlog EXCLUDE_FROM_ALL SYSTEM)
target_compile_definitions(spdlog PUBLIC SPDLOG_FUNCTION=__func__)

# GoogleTest
if (NOT TARGET GTest::gtest_main)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(googletest)
endif()
