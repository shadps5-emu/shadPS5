// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <cstdio>
#include <string>
#include <fstream>

#include "common/path_util.h"
#include "common/thread.h"
#include "core/emulator_settings.h"
#include "core/file_sys/ifile.h"

// --- common ------------------------------------------------------------------

std::string Common::GetCurrentThreadName() {
	return "main";
}

// --- core --------------------------------------------------------------------

EmulatorSettingsImpl* EmulatorSettingsImpl::GetInstance() {
	static EmulatorSettingsImpl instance;
	return &instance;
}

bool Core::FileSys::IsZArchiveFile(const std::filesystem::path& path) {
	std::error_code ec;
	return std::filesystem::is_regular_file(path, ec) && path.extension() == ".zar";
}

std::optional<std::vector<u8>> Core::FileSys::ReadGameFile(const std::filesystem::path& game_root,
	std::string_view rel_path) {
	std::error_code ec;
	if (!std::filesystem::is_directory(game_root, ec)) {
		return std::nullopt; // no ZArchive support
	}
	std::ifstream file(game_root / rel_path, std::ios::binary);
	if (!file) {
		return std::nullopt;
	}
	return std::vector<u8>(std::istreambuf_iterator<char>(file), {});
}
