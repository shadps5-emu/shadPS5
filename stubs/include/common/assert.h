// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
// SPDX-License-Identifier: GPL-2.0-or-later
// STUB: ASSERT_MSG prints the formatted message and aborts.

#pragma once

#include <cstdio>
#include <cstdlib>
#include <format>
#include <string>

#define ASSERT_MSG(_a_, ...)                                                                       \
    do {                                                                                           \
        if (!(_a_)) {                                                                              \
            std::fprintf(stderr, "Assertion failed: %s\n", std::format(__VA_ARGS__).c_str());      \
            std::abort();                                                                          \
        }                                                                                          \
    } while (false)
