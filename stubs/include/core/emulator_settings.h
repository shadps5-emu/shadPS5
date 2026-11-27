// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <mutex>
#include <ranges>
#include <unordered_map>
#include <vector>

#include <filesystem>
#include <string>

#include "common/types.h"

class EmulatorSettingsImpl {
public:
    static EmulatorSettingsImpl* GetInstance();

    std::filesystem::path GetSysModulesDir() const {
        return m_sys_modules_dir;
    }
    void SetSysModulesDir(const std::filesystem::path& dir) {
        m_sys_modules_dir = dir;
    }

    bool IsDevKit() const {
        return m_dev_kit;
    }
    void SetDevKit(bool value) {
        m_dev_kit = value;
    }

    // Log settings
    bool IsLogAppend() const {
        return m_log_append;
    }
    void SetLogAppend(bool value) {
        m_log_append = value;
    }
    bool IsLogEnable() const {
        return m_log_enable;
    }
    void SetLogEnable(bool value) {
        m_log_enable = value;
    }
    std::string GetLogFilter() const {
        return m_log_filter;
    }
    void SetLogFilter(const std::string& value) {
        m_log_filter = value;
    }
    std::string GetLogFlushLevel() const {
        return m_log_flush_level;
    }
    void SetLogFlushLevel(const std::string& value) {
        m_log_flush_level = value;
    }
    u32 GetLogMaxSkipDuration() const {
        return m_log_max_skip_duration;
    }
    void SetLogMaxSkipDuration(u32 value) {
        m_log_max_skip_duration = value;
    }
    unsigned long long GetLogSizeLimit() const {
        return m_log_size_limit;
    }
    void SetLogSizeLimit(unsigned long long value) {
        m_log_size_limit = value;
    }
    bool IsLogSkipDuplicate() const {
        return m_log_skip_duplicate;
    }
    void SetLogSkipDuplicate(bool value) {
        m_log_skip_duplicate = value;
    }
    bool IsLogSync() const {
        return m_log_sync;
    }
    void SetLogSync(bool value) {
        m_log_sync = value;
    }
    // Windows only upstream: "wincolor" for the console, anything else for the debugger output.
    std::string GetLogType() const {
        return m_log_type;
    }
    void SetLogType(const std::string& value) {
        m_log_type = value;
    }

private:
    std::filesystem::path m_sys_modules_dir;
    bool m_dev_kit = false;

    bool m_log_append = false;
    bool m_log_enable = true;
    std::string m_log_filter;
    std::string m_log_flush_level;
    u32 m_log_max_skip_duration = 5'000;
    unsigned long long m_log_size_limit = 100ULL * 1024 * 1024;
    bool m_log_skip_duplicate = true;
    bool m_log_sync = true;
    std::string m_log_type = "wincolor";
};

#define EmulatorSettings (*EmulatorSettingsImpl::GetInstance())
