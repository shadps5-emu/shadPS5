# SPDX-FileCopyrightText: Copyright 2025 shadPS5 Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(shadPS5)

add_subdirectory(externals)

add_executable(shadps5 main.cpp)

target_link_libraries(shadps5 PRIVATE nlohmann_json::nlohmann_json)



