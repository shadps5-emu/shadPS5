// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <filesystem>
#include <string>

namespace Common::FS {

enum class PathType {
    LogDir, // Where log files are stored.
};

constexpr auto PORTABLE_DIR = "user";
constexpr auto LOG_DIR = "log";

[[nodiscard]] const std::filesystem::path& GetUserPath(PathType user_path);

void SetUserPath(PathType user_path, const std::filesystem::path& new_path);

[[nodiscard]] std::string PathToUTF8String(const std::filesystem::path& path);

} // namespace Common::FS
