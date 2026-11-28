# SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
# SPDX-License-Identifier: GPL-2.0-or-later

include(GoogleTest)

set(TEST_TARGETS
    shadps5_log_test
    shadps5_path_util_test
)


set(LOG_TEST_SOURCES
    # Under test
    ${CMAKE_SOURCE_DIR}/src/common/logging/classes.cpp
    ${CMAKE_SOURCE_DIR}/src/common/logging/log.cpp
    ${CMAKE_SOURCE_DIR}/src/common/path_util.cpp

    # Stubs that replace dependencies
    ${CMAKE_SOURCE_DIR}/stubs/src/stubs.cpp

    # Tests
    test_log.cpp
)

add_executable(shadps5_log_test ${LOG_TEST_SOURCES})
target_link_libraries(shadps5_log_test PRIVATE GTest::gtest_main spdlog::spdlog fmt::fmt)


set(PATH_UTIL_TEST_SOURCES
    # Under test
    ${CMAKE_SOURCE_DIR}/src/common/path_util.cpp

    # Needed by path_util
    ${CMAKE_SOURCE_DIR}/src/common/logging/classes.cpp
    ${CMAKE_SOURCE_DIR}/src/common/logging/log.cpp

    # Stubs that replace dependencies
    ${CMAKE_SOURCE_DIR}/stubs/src/stubs.cpp

    # Tests
    test_path_util.cpp
)

add_executable(shadps5_path_util_test ${PATH_UTIL_TEST_SOURCES})
target_link_libraries(shadps5_path_util_test PRIVATE GTest::gtest_main spdlog::spdlog fmt::fmt)

foreach(target IN LISTS TEST_TARGETS)
    target_include_directories(${target} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/stubs/include
    )
    target_compile_options(${target} PRIVATE ${SHADPS5_WARNINGS})
    gtest_discover_tests(${target}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DISCOVERY_TIMEOUT 60
    )
endforeach()
