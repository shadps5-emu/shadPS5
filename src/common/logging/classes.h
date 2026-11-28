// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Emulator Project
// SPDX-FileCopyrightText: Copyright 2025-2026 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later
// History:
//   2026-11-26  Copied from shadPS4 Emulator Project (Logging classes reduced to neccesary ones)

#pragma once

#include <array>
#include <string_view>

namespace Common::Log {

/**
 * Specifies the sub-system that generated the log message.
 */
enum class Class {
    Log,               ///< Messages about the log system itself
    Common,            ///< Library routines
    Common_Filesystem, ///< FileSystem
    Core,              ///< LLE emulation core
    Config,            ///< Emulator configuration (including commandline)
    Debug,             ///< Debugging tools
    Kernel,            ///< The HLE implementation of the PS5 kernel.
    Loader,            ///< ROM loader
    Tty,               ///< Debug output from emu
    Count              ///< Total number of logging classes
};
static constexpr int NUM_LOG_CLASSES = static_cast<int>(Class::Count);

std::string_view NameOf(Class log_class);

} // namespace Common::Log
