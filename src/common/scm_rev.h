// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Emulator Project
// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later
// History:
//   2026-11-16  Copied from shadPS4 Emulator Project

#pragma once

#include <string>

namespace Common {

extern const char g_version[];
extern const bool g_is_release;

extern const char g_scm_rev[];
extern const char g_scm_branch[];
extern const char g_scm_desc[];
extern const char g_scm_remote_name[];
extern const char g_scm_remote_url[];
extern const char g_scm_date[];

const std::string GetRemoteNameFromLink();

} // namespace Common
