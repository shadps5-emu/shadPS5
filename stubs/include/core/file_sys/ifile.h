// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

#include "common/types.h"

namespace Core::FileSys {

	// True if path is a regular file with a ".zar" extension
	[[nodiscard]] bool IsZArchiveFile(const std::filesystem::path& path);

	// Reads rel_path from an unpacked game folder; std::nullopt for an archive or
	// a file that can't be read.
	[[nodiscard]] std::optional<std::vector<u8>> ReadGameFile(const std::filesystem::path& game_root,
		std::string_view rel_path);

} // namespace Core::FileSys
