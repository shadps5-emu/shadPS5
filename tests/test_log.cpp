// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "common/logging/classes.h"
#include "common/logging/log.h"
#include "common/path_util.h"
#include "core/emulator_settings.h"

namespace fs = std::filesystem;
using Common::Log::Class;
using Common::Log::Level;

namespace {

constexpr auto MAIN_LOG = "shad_log.txt";

std::vector<std::string> ReadLines(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    std::vector<std::string> lines;
    for (std::string line; std::getline(file, line);) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(std::move(line));
    }
    return lines;
}

int CountContaining(const std::vector<std::string>& lines, std::string_view text) {
    return static_cast<int>(std::ranges::count_if(
        lines, [&](const std::string& line) { return line.find(text) != std::string::npos; }));
}

// Puts the log settings at shadPS5's defaults, except the duplicate filter,
// which is off so that every line reaches the file, points the log folder at
// <temp>/shadps5_log_tests/<test name>.
class LogTest : public ::testing::Test {
protected:
    void SetUp() override {
        EmulatorSettings.SetLogAppend(false);
        EmulatorSettings.SetLogEnable(true);
        EmulatorSettings.SetLogFilter("");
        EmulatorSettings.SetLogFlushLevel("");
        EmulatorSettings.SetLogMaxSkipDuration(5'000);
        EmulatorSettings.SetLogSizeLimit(100ULL * 1024 * 1024);
        EmulatorSettings.SetLogSkipDuplicate(false);
        EmulatorSettings.SetLogSync(true);

        const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        log_dir = fs::temp_directory_path() / "shadps5_log_tests" / info->name();
        std::error_code ec;
        fs::remove_all(log_dir, ec);
        fs::create_directories(log_dir);
        Common::FS::SetUserPath(Common::FS::PathType::LogDir, log_dir);
    }

    void TearDown() override {
        if (started) {
            // Back to the levels the log has before Setup: everything on.
            EmulatorSettings.SetLogEnable(true);
            Common::Log::UpdateLogLevels("*:trace");
            Common::Log::Shutdown(); // closes the files
        }
        std::error_code ec;
        fs::remove_all(log_dir, ec);
    }

    // Setup, with every level let through.
    void Start() {
        Common::Log::Setup(MAIN_LOG);
        started = true;
        Common::Log::UpdateLogLevels("*:trace");
    }

    std::vector<std::string> MainLines() const {
        Common::Log::Flush();
        return ReadLines(log_dir / MAIN_LOG);
    }

    std::vector<std::string> SessionLines(std::string_view name) const {
        Common::Log::Flush();
        return ReadLines(log_dir / name);
    }

    fs::path log_dir;
    bool started = false;
};

} // namespace

// ── classes ─────────────────────────────────────────────────────────────────

// Every class has a name of its own; sub-classes print with a dot.
TEST(LogClassTest, Names) {
    EXPECT_TRUE(Common::Log::NameOf(Class::Log) == "Log");
    EXPECT_TRUE(Common::Log::NameOf(Class::Common) == "Common");
    EXPECT_TRUE(Common::Log::NameOf(Class::Log) == "Log");
    EXPECT_TRUE(Common::Log::NameOf(Class::Loader) == "Loader");
    EXPECT_TRUE(Common::Log::NameOf(Class::Count) == "Count");

    std::set<std::string_view> names;
    for (int i = 0; i < Common::Log::NUM_LOG_CLASSES; i++) {
        const auto name = Common::Log::NameOf(static_cast<Class>(i));
        EXPECT_TRUE(!name.empty() && name != "Unknown" && name != "Count") << "class " << i;
        EXPECT_TRUE(name.find('_') == std::string_view::npos) << name;
        names.insert(name);
    }
    EXPECT_TRUE(names.size() == static_cast<size_t>(Common::Log::NUM_LOG_CLASSES));
}

// ── setup ───────────────────────────────────────────────────────────────────

// Until Setup has run, logging does nothing and creates no file.
TEST_F(LogTest, NothingBeforeSetup) {
    EXPECT_TRUE(Common::Log::ShouldLog(Class::Log, Level::Trace));
    LOG_INFO(Log, "nobody is listening");
    LOG_CRITICAL(Loader, "still nobody");
    Common::Log::Flush();
    EXPECT_TRUE(fs::is_empty(log_dir));
}

// Setup opens <log folder>/<name>, a line is
// "[<class>] <<level>> (<thread>) <file>:<line> <function>: <message>".
TEST_F(LogTest, LineFormat) {
    Start();
    EXPECT_TRUE(fs::exists(log_dir / MAIN_LOG));
    EXPECT_TRUE(MainLines().empty());

    const int line = __LINE__ + 1;
    LOG_INFO(Log, "value {} and {:#x}", 42, 255);
    const auto lines = MainLines();
    ASSERT_TRUE(lines.size() == 1);
    EXPECT_EQ(lines[0], "[Log] <Info> (main) test_log.cpp:" + std::to_string(line) +
                            " TestBody: value 42 and 0xff");

    // A lambda's operator() is printed as "lambda".
    [] { LOG_WARNING(Loader, "from a lambda"); }();
    EXPECT_TRUE(MainLines().back().ends_with(" lambda: from a lambda"));
    EXPECT_TRUE(MainLines().back().starts_with("[Loader] <Warning> (main) test_log.cpp:"));
}

// The six levels and the names they print with. LOG_TRACE is compiled out of
// release builds.
TEST_F(LogTest, Levels) {
    Start();
    LOG_TRACE(Core, "t");
    LOG_DEBUG(Core, "d");
    LOG_INFO(Core, "i");
    LOG_WARNING(Core, "w");
    LOG_ERROR(Core, "e");
    LOG_CRITICAL(Core, "c");
    const auto lines = MainLines();
#ifdef NDEBUG
    EXPECT_TRUE(CountContaining(lines, "<Trace>") == 0);
    EXPECT_TRUE(lines.size() == 5);
#else
    EXPECT_TRUE(CountContaining(lines, "<Trace>") == 1);
    EXPECT_TRUE(lines.size() == 6);
#endif
    for (const auto* level : {"<Debug>", "<Info>", "<Warning>", "<Error>", "<Critical>"}) {
        EXPECT_TRUE(CountContaining(lines, level) == 1) << level;
    }
    EXPECT_TRUE(lines.back().starts_with("[Core] <Critical> ") && lines.back().ends_with(": c"));
}

// A second Setup on the same file adds to it.
TEST_F(LogTest, SetupAppends) {
    Start();
    LOG_INFO(Core, "first run");
    Common::Log::Shutdown();
    Start();
    LOG_INFO(Core, "second run");
    const auto lines = MainLines();
    ASSERT_TRUE(lines.size() == 2);
    EXPECT_TRUE(lines[0].ends_with(": first run") && lines[1].ends_with(": second run"));
}

// ── filter ──────────────────────────────────────────────────────────────────

// An empty filter lets Info and above through, for every class.
TEST_F(LogTest, DefaultFilterIsInfo) {
    Start();
    Common::Log::UpdateLogLevels("");
    EXPECT_TRUE(!Common::Log::ShouldLog(Class::Log, Level::Debug));
    EXPECT_TRUE(Common::Log::ShouldLog(Class::Log, Level::Info));
    LOG_DEBUG(Log, "dropped");
    LOG_INFO(Log, "kept");
    LOG_ERROR(Loader, "kept too");
    const auto lines = MainLines();
    ASSERT_TRUE(lines.size() == 2);
    EXPECT_TRUE(lines[0].ends_with(": kept") && lines[1].ends_with(": kept too"));
}

// "*:<level>" sets every class, level names are not case sensitive.
TEST_F(LogTest, FilterAllClasses) {
    Start();
    Common::Log::UpdateLogLevels("*:Error");
    for (int i = 0; i < Common::Log::NUM_LOG_CLASSES; i++) {
        EXPECT_TRUE(Common::Log::g_class_levels[i] == Level::Error);
    }
    LOG_WARNING(Core, "dropped");
    LOG_ERROR(Core, "kept");
    LOG_CRITICAL(Loader, "kept");
    EXPECT_TRUE(MainLines().size() == 2);

    Common::Log::UpdateLogLevels("*:off");
    EXPECT_TRUE(!Common::Log::ShouldLog(Class::Core, Level::Critical));
    LOG_CRITICAL(Core, "dropped");
    EXPECT_TRUE(MainLines().size() == 2);
}

// "<class>:<level>" sets one class, by its printed name, whichever side of the
// "*" entry it is on; the others take the "*" level.
TEST_F(LogTest, FilterPerClass) {
    Start();
    Common::Log::UpdateLogLevels("Log:debug *:critical Loader:warning");
    EXPECT_TRUE(Common::Log::g_class_levels[static_cast<size_t>(Class::Log)] == Level::Debug);
    EXPECT_TRUE(Common::Log::g_class_levels[static_cast<size_t>(Class::Loader)] == Level::Warning);
    EXPECT_TRUE(Common::Log::g_class_levels[static_cast<size_t>(Class::Core)] == Level::Critical);

    LOG_DEBUG(Log, "kept");
    LOG_INFO(Loader, "dropped");
    LOG_WARNING(Loader, "kept");
    LOG_ERROR(Core, "dropped");
    LOG_CRITICAL(Core, "kept");
    const auto lines = MainLines();
    EXPECT_TRUE(lines.size() == 3);
    EXPECT_TRUE(CountContaining(lines, ": kept") == 3);

    // A name that is not a class is ignored.
    Common::Log::UpdateLogLevels("*:off Nonexistent:trace");
    EXPECT_TRUE(!Common::Log::ShouldLog(Class::Log, Level::Critical));

    // A class name is not ignored.
    Common::Log::UpdateLogLevels("*:off Log:trace");
    EXPECT_TRUE(Common::Log::ShouldLog(Class::Log, Level::Trace));
}

// An entry that is not "<class>:<level>" is reported under Config and skipped;
// the rest of the filter still applies.
TEST_F(LogTest, BadFilter) {
    Start();
    Common::Log::UpdateLogLevels("nonsense Loader:error");
    const auto lines = MainLines();
    ASSERT_TRUE(lines.size() == 1);
    EXPECT_TRUE(lines[0].starts_with("[Config] <Error> ") &&
                lines[0].ends_with(": bad log filter provided"));
    EXPECT_TRUE(Common::Log::g_class_levels[static_cast<size_t>(Class::Loader)] == Level::Error);
    EXPECT_TRUE(Common::Log::g_class_levels[static_cast<size_t>(Class::Core)] == Level::Info);
}

// With the log disabled in the settings, the filter is not read at all.
TEST_F(LogTest, Disabled) {
    Start();
    EmulatorSettings.SetLogEnable(false);
    Common::Log::UpdateLogLevels("*:trace");
    for (int i = 0; i < Common::Log::NUM_LOG_CLASSES; i++) {
        EXPECT_TRUE(Common::Log::g_class_levels[i] == Level::Off);
    }
    LOG_CRITICAL(Core, "dropped");
    EXPECT_TRUE(MainLines().empty());

    EmulatorSettings.SetLogEnable(true);
    Common::Log::UpdateLogLevels("*:trace");
    LOG_INFO(Core, "back");
    EXPECT_TRUE(MainLines().size() == 1);
}

// ── session file ────────────────────────────────────────────────────────────

// Switch opens a game's log file and applies the filter from the settings;
// from then on lines go there and no longer to the main file.
TEST_F(LogTest, SwitchToSessionFile) {
    Start();
    LOG_INFO(Core, "before the game");
    EmulatorSettings.SetLogFilter("*:warning");
    Common::Log::Switch("PPSA01473.log", false);

    LOG_INFO(Core, "dropped by the settings' filter");
    LOG_WARNING(Core, "in the game");
    const auto main_lines = MainLines();
    const auto session = SessionLines("PPSA01473.log");
    ASSERT_TRUE(main_lines.size() == 1 && session.size() == 1);
    EXPECT_TRUE(main_lines[0].ends_with(": before the game"));
    EXPECT_TRUE(session[0].ends_with(": in the game"));
}

// A session file is emptied when it is opened, unless the caller or the
// settings ask to append.
TEST_F(LogTest, SwitchTruncatesOrAppends) {
    Start();
    for (const auto* name : {"truncated.log", "appended.log", "appended_by_setting.log"}) {
        std::ofstream(log_dir / name, std::ios::binary) << "an earlier session\n";
    }

    Common::Log::Switch("truncated.log", false);
    LOG_INFO(Core, "new");
    EXPECT_TRUE(SessionLines("truncated.log").size() == 1);

    Common::Log::Switch("appended.log", true);
    LOG_INFO(Core, "new");
    auto lines = SessionLines("appended.log");
    ASSERT_TRUE(lines.size() == 2);
    EXPECT_TRUE(lines[0] == "an earlier session" && lines[1].ends_with(": new"));

    EmulatorSettings.SetLogAppend(true);
    Common::Log::Switch("appended_by_setting.log", false);
    LOG_INFO(Core, "new");
    EXPECT_TRUE(SessionLines("appended_by_setting.log").size() == 2);
}

// The size limit stops a session file growing, critical lines are written
// regardless. The main file has no limit.
TEST_F(LogTest, SessionSizeLimit) {
    EmulatorSettings.SetLogSizeLimit(1); // would stop the main file at once
    Start();
    for (int i = 0; i < 20; i++) {
        LOG_INFO(Core, "main file line {}", i);
    }
    EXPECT_TRUE(MainLines().size() == 20);

    EmulatorSettings.SetLogSizeLimit(200);
    Common::Log::Switch("limited.log", false);
    for (int i = 0; i < 20; i++) {
        LOG_INFO(Core, "session line {}", i);
    }
    const auto limited = SessionLines("limited.log");
    EXPECT_TRUE(!limited.empty() && limited.size() < 20);
    // Writing stops with the first line that takes the file to the limit.
    EXPECT_TRUE(fs::file_size(log_dir / "limited.log") >= 200);
    EXPECT_TRUE(fs::file_size(log_dir / "limited.log") < 200 + limited.back().size() + 2);

    LOG_ERROR(Core, "over the limit");
    LOG_CRITICAL(Core, "over the limit, but critical");
    const auto after = SessionLines("limited.log");
    ASSERT_TRUE(after.size() == limited.size() + 1);
    EXPECT_TRUE(after.back().ends_with(": over the limit, but critical"));
}

// ── sinks ───────────────────────────────────────────────────────────────────

// With the duplicate filter on, a repeated line is written once, and the
// number of repeats when the next different line arrives.
TEST_F(LogTest, SkipDuplicates) {
    EmulatorSettings.SetLogSkipDuplicate(true);
    Start();
    for (int i = 0; i < 5; i++) {
        LOG_INFO(Core, "again and again");
    }
    LOG_INFO(Core, "something else");
    const auto lines = MainLines();
    ASSERT_TRUE(lines.size() == 3);
    EXPECT_TRUE(lines[0].ends_with(": again and again"));
    EXPECT_TRUE(lines[1] == "Skipped 4 duplicate messages..");
    EXPECT_TRUE(lines[2].ends_with(": something else"));

    // UpdateSinks picks the setting up again.
    EmulatorSettings.SetLogSkipDuplicate(false);
    Common::Log::UpdateSinks();
    for (int i = 0; i < 5; i++) {
        LOG_INFO(Core, "again and again");
    }
    EXPECT_TRUE(MainLines().size() == 8);
}

// With sync off the lines go through the log thread, all of them are in the
// file, in order, once the log has shut down.
TEST_F(LogTest, Async) {
    EmulatorSettings.SetLogSync(false);
    Start();
    for (int i = 0; i < 200; i++) {
        LOG_INFO(Core, "queued {}", i);
    }
    Common::Log::Shutdown();
    started = false;

    const auto lines = ReadLines(log_dir / MAIN_LOG);
    ASSERT_TRUE(lines.size() == 200);
    for (int i = 0; i < 200; i++) {
        EXPECT_TRUE(lines[i].ends_with(": queued " + std::to_string(i))) << lines[i];
    }
}

// A flush level writes the file out as soon as a line of that level is logged.
TEST_F(LogTest, FlushLevel) {
    Start();
    Common::Log::UpdateLogFlushLevel(""); // empty: left as it is
    Common::Log::UpdateLogFlushLevel("error");
    LOG_INFO(Core, "buffered");
    LOG_ERROR(Core, "flushes");
    // Read without Common::Log::Flush().
    const auto lines = ReadLines(log_dir / MAIN_LOG);
    ASSERT_TRUE(lines.size() == 2);
    EXPECT_TRUE(lines[1].ends_with(": flushes"));
}
