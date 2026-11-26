// SPDX-FileCopyrightText: Copyright 2026 shadPS5 Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <cstdio>
#include <common/logging/log.h>
#include "common/scm_rev.h"

int main(int argc, char** argv) {
    // sample log messages to test the logging system
    Common::Log::Setup("shadps5.txt");
    LOG_INFO(Loader, "Welcome to shadPS5 v{}", Common::g_version);
    LOG_INFO(Loader, "Revision {}", Common::g_scm_rev);
    LOG_INFO(Loader, "Branch {}", Common::g_scm_branch);
    LOG_INFO(Loader, "Description {}", Common::g_scm_desc);
    LOG_INFO(Loader, "Remote {} ({})", Common::g_scm_remote_name, Common::g_scm_remote_url);
    LOG_INFO(Loader, "Built {}", Common::g_scm_date);

    Common::Log::Shutdown();
    return 1;
}
