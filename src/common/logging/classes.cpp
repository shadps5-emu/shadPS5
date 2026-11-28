// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Emulator Project
// SPDX-FileCopyrightText: Copyright 2026 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later
// History:
//   2026-11-26  Copied from shadPS4 Emulator Project (Logging classes reduced to neccesary ones)

#include "common/logging/classes.h"

namespace Common::Log {

std::string_view NameOf(Class log_class) {
    switch (log_class) {
    case Class::Log:
        return "Log";
    case Class::Common:
        return "Common";
    case Class::Common_Filesystem:
        return "Common.Filesystem";
    case Class::Core:
        return "Core";
    case Class::Config:
        return "Config";
    case Class::Debug:
        return "Debug";
    case Class::Kernel:
        return "Kernel";
    case Class::Loader:
        return "Loader";
    case Class::Tty:
        return "Tty";
    case Class::Count:
        return "Count";
    }
    return "Unknown";
}

} // namespace Common::Log