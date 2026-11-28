// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "common/path_util.h"

namespace fs = std::filesystem;
using Common::FS::PathType;

namespace {

constexpr auto GAME_ID = "PPSA01234";

// Creates an empty file, making its parent folders first.
void Touch(const fs::path& path) {
    fs::create_directories(path.parent_path());
    std::ofstream{path};
}

// Lays out an unpacked PS5 game: <root>/sce_sys/param.json and <root>/eboot.bin.
void MakeGame(const fs::path& root, bool with_param = true, bool with_eboot = true) {
    fs::create_directories(root);
    if (with_param) {
        Touch(root / "sce_sys" / "param.json");
    }
    if (with_eboot) {
        Touch(root / "eboot.bin");
    }
}

// Gives each test an empty folder at <temp>/shadps5_path_util_tests/<test name>.
class TempDirTest : public ::testing::Test {
protected:
    void SetUp() override {
        const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        root = fs::temp_directory_path() / "shadps5_path_util_tests" / info->name();
        fs::remove_all(root);
        fs::create_directories(root);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(root, ec);
    }

    fs::path root;
};

// Puts the user paths back after a test that changes them.
class UserPathTest : public TempDirTest {
protected:
    void SetUp() override {
        TempDirTest::SetUp();
        saved_user = Common::FS::GetUserPath(PathType::UserDir);
        saved_log = Common::FS::GetUserPath(PathType::LogDir);
    }

    void TearDown() override {
        Common::FS::SetUserPath(PathType::UserDir, saved_user);
        Common::FS::SetUserPath(PathType::LogDir, saved_log);
        TempDirTest::TearDown();
    }

    fs::path saved_user;
    fs::path saved_log;
};

using FindGameTest = TempDirTest;

} // namespace

// --- ValidatePath ------------------------------------------------------------

TEST(ValidatePathTest, EmptyPathIsInvalid) {
    EXPECT_FALSE(Common::FS::ValidatePath(fs::path{}));
}

TEST(ValidatePathTest, OrdinaryPathsAreValid) {
    EXPECT_TRUE(Common::FS::ValidatePath("eboot.bin"));
    EXPECT_TRUE(Common::FS::ValidatePath(fs::path("games") / GAME_ID / "eboot.bin"));
    EXPECT_TRUE(Common::FS::ValidatePath(fs::current_path()));
}

TEST(ValidatePathTest, LengthLimit) {
    // 260 UTF-16 code units on Windows, 1024 UTF-8 code units elsewhere; the limit
    // itself is already too long.
#ifdef _WIN32
    constexpr std::size_t limit = 260;
#else
    constexpr std::size_t limit = 1024;
#endif
    EXPECT_TRUE(Common::FS::ValidatePath(std::string(limit - 1, 'a')));
    EXPECT_FALSE(Common::FS::ValidatePath(std::string(limit, 'a')));
    EXPECT_FALSE(Common::FS::ValidatePath(std::string(limit * 2, 'a')));
}

// --- PathToUTF8String --------------------------------------------------------

TEST(PathToUTF8StringTest, Ascii) {
    EXPECT_EQ(Common::FS::PathToUTF8String(fs::path("eboot.bin")), "eboot.bin");
    EXPECT_EQ(Common::FS::PathToUTF8String(fs::path{}), "");
}

TEST(PathToUTF8StringTest, NonAsciiRoundTrips) {
    // "παιχνίδι" (Greek) and "ゲーム" (Japanese), written as UTF-8 bytes.
    const std::string utf8 = "\xCF\x80\xCE\xB1\xCE\xB9\xCF\x87\xCE\xBD\xCE\xAF\xCE\xB4\xCE\xB9/"
                             "\xE3\x82\xB2\xE3\x83\xBC\xE3\x83\xA0";
    const fs::path path{std::u8string(utf8.begin(), utf8.end())};
    EXPECT_EQ(Common::FS::PathToUTF8String(path), utf8);
    EXPECT_EQ(Common::FS::PathToUTF8String(path.filename()),
              "\xE3\x82\xB2\xE3\x83\xBC\xE3\x83\xA0");
}

// --- User paths --------------------------------------------------------------

TEST(UserPathDefaultsTest, PortableFolderInWorkingDirectory) {
    // The tests run with the build folder as their working directory.
    const auto& user = Common::FS::GetUserPath(PathType::UserDir);
    EXPECT_EQ(user, fs::current_path() / Common::FS::PORTABLE_DIR);
    EXPECT_TRUE(fs::is_directory(user));
}

TEST(UserPathDefaultsTest, LogFolderInsideUserFolder) {
    const auto& log = Common::FS::GetUserPath(PathType::LogDir);
    EXPECT_EQ(log, Common::FS::GetUserPath(PathType::UserDir) / Common::FS::LOG_DIR);
    EXPECT_TRUE(fs::is_directory(log));
}

TEST(UserPathDefaultsTest, StringMatchesPath) {
    for (const auto type : {PathType::UserDir, PathType::LogDir}) {
        EXPECT_EQ(Common::FS::GetUserPathString(type),
                  Common::FS::PathToUTF8String(Common::FS::GetUserPath(type)));
    }
}

TEST_F(UserPathTest, SetToExistingFolder) {
    Common::FS::SetUserPath(PathType::LogDir, root);
    EXPECT_EQ(Common::FS::GetUserPath(PathType::LogDir), root);
    EXPECT_EQ(Common::FS::GetUserPathString(PathType::LogDir), Common::FS::PathToUTF8String(root));
    // Only the path that was set changes.
    EXPECT_EQ(Common::FS::GetUserPath(PathType::UserDir), saved_user);
}

TEST_F(UserPathTest, MissingFolderIsIgnored) {
    Common::FS::SetUserPath(PathType::LogDir, root / "missing");
    EXPECT_EQ(Common::FS::GetUserPath(PathType::LogDir), saved_log);
    EXPECT_FALSE(fs::exists(root / "missing"));
}

TEST_F(UserPathTest, FileIsIgnored) {
    Touch(root / "not_a_folder");
    Common::FS::SetUserPath(PathType::LogDir, root / "not_a_folder");
    EXPECT_EQ(Common::FS::GetUserPath(PathType::LogDir), saved_log);
}

// --- FindGameByID ------------------------------------------------------------

TEST_F(FindGameTest, GameFolderInSearchFolder) {
    MakeGame(root / GAME_ID);
    const auto found = Common::FS::FindGameByID(root, GAME_ID, 0);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(*found, root / GAME_ID / "eboot.bin");
}

TEST_F(FindGameTest, SearchFolderIsTheGame) {
    MakeGame(root / GAME_ID);
    const auto found = Common::FS::FindGameByID(root / GAME_ID, GAME_ID, 0);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(*found, root / GAME_ID / "eboot.bin");
}

TEST_F(FindGameTest, NestedGameNeedsEnoughDepth) {
    // root/a/b/PPSA01234: the game folder is checked from b, two levels down.
    MakeGame(root / "a" / "b" / GAME_ID);
    EXPECT_FALSE(Common::FS::FindGameByID(root, GAME_ID, 0).has_value());
    EXPECT_FALSE(Common::FS::FindGameByID(root, GAME_ID, 1).has_value());
    const auto found = Common::FS::FindGameByID(root, GAME_ID, 2);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(*found, root / "a" / "b" / GAME_ID / "eboot.bin");
}

TEST_F(FindGameTest, NeedsParamJson) {
    MakeGame(root / GAME_ID, /*with_param=*/false);
    EXPECT_FALSE(Common::FS::FindGameByID(root, GAME_ID, 2).has_value());
}

TEST_F(FindGameTest, Ps4ParamSfoIsNotEnough) {
    MakeGame(root / GAME_ID, /*with_param=*/false);
    Touch(root / GAME_ID / "sce_sys" / "param.sfo");
    EXPECT_FALSE(Common::FS::FindGameByID(root, GAME_ID, 2).has_value());
}

TEST_F(FindGameTest, NeedsEboot) {
    MakeGame(root / GAME_ID, /*with_param=*/true, /*with_eboot=*/false);
    EXPECT_FALSE(Common::FS::FindGameByID(root, GAME_ID, 2).has_value());
}

TEST_F(FindGameTest, OtherGameIsNotReturned) {
    MakeGame(root / "PPSA09999");
    EXPECT_FALSE(Common::FS::FindGameByID(root, GAME_ID, 2).has_value());
}

TEST_F(FindGameTest, NameMustMatchExactly) {
    // A longer ID that starts with the one searched for is a different game.
    MakeGame(root / (std::string(GAME_ID) + "5"));
    EXPECT_FALSE(Common::FS::FindGameByID(root, GAME_ID, 2).has_value());
}

TEST_F(FindGameTest, DoesNotLookInsideOtherGames) {
    // A game folder stops the search, even if something inside looks like a match.
    MakeGame(root / "PPSA09999");
    MakeGame(root / "PPSA09999" / GAME_ID);
    EXPECT_FALSE(Common::FS::FindGameByID(root / "PPSA09999", GAME_ID, 2).has_value());
    EXPECT_FALSE(Common::FS::FindGameByID(root, GAME_ID, 2).has_value());
}

TEST_F(FindGameTest, FindsGameNextToOthers) {
    MakeGame(root / "PPSA09999");
    MakeGame(root / "more" / GAME_ID);
    const auto found = Common::FS::FindGameByID(root, GAME_ID, 1);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(*found, root / "more" / GAME_ID / "eboot.bin");
}

TEST_F(FindGameTest, FilesAreSkipped) {
    Touch(root / "readme.txt");
    Touch(root / GAME_ID); // a file, not a game folder
    EXPECT_FALSE(Common::FS::FindGameByID(root, GAME_ID, 2).has_value());
}

TEST_F(FindGameTest, MissingSearchFolder) {
    EXPECT_FALSE(Common::FS::FindGameByID(root / "missing", GAME_ID, 2).has_value());
}

TEST_F(FindGameTest, EmptySearchFolder) {
    EXPECT_FALSE(Common::FS::FindGameByID(root, GAME_ID, 2).has_value());
}
