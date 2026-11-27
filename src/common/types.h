// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>

using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using VAddr = std::uintptr_t;

#if defined(__x86_64__) || defined(_M_X64)
#define PS5_SYSV_ABI __attribute__((sysv_abi))
#else
#define PS5_SYSV_ABI
#endif
