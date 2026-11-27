// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <cstdio>
#include <string>

#include "common/path_util.h"
#include "common/thread.h"
#include "core/emulator_settings.h"

// --- common ------------------------------------------------------------------

std::string Common::GetCurrentThreadName() {
	return "main";
}

namespace {
	std::filesystem::path& LogDir() {
		static std::filesystem::path log_dir = std::filesystem::current_path() /
			Common::FS::PORTABLE_DIR / Common::FS::LOG_DIR;
		return log_dir;
	}
} // namespace

const std::filesystem::path& Common::FS::GetUserPath(PathType /*user_path*/) {
	return LogDir();
}

void Common::FS::SetUserPath(PathType /*user_path*/, const std::filesystem::path& new_path) {
	LogDir() = new_path;
}

std::string Common::FS::PathToUTF8String(const std::filesystem::path& path) {
	const auto u8 = path.u8string();
	return std::string(u8.begin(), u8.end());
}

// --- core --------------------------------------------------------------------

EmulatorSettingsImpl* EmulatorSettingsImpl::GetInstance() {
	static EmulatorSettingsImpl instance;
	return &instance;
}

