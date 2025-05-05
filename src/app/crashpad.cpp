// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "crashpad.h"

#define NOMINMAX
#include <client/crash_report_database.h>
#include <client/settings.h>
#include <client/crashpad_client.h>
#include <client/crashpad_info.h>
#include <iostream>

std::unique_ptr<crashpad::CrashReportDatabase> database;

bool startCrashHandler(std::string const& url, std::wstring const& handler_path, std::wstring const& db_path)
{
    using namespace crashpad;

    std::map<std::string, std::string> annotations;
    std::vector<std::string> arguments;
    bool rc;

    /*
     * THE FOLLOWING ANNOTATIONS MUST BE SET.
     *
     * Backtrace supports many file formats. Set format to minidump
     * so it knows how to process the incoming dump.
     */
    annotations["format"] = "minidump";

    /*
     * REMOVE THIS FOR ACTUAL BUILD.
     *
     * We disable crashpad rate limiting for this example.
     */
    arguments.push_back("--no-rate-limit");

    base::FilePath db(db_path);
    base::FilePath handler(handler_path);

    database = CrashReportDatabase::Initialize(db);

    if (database == nullptr || database->GetSettings() == NULL)
        return false;

    /* Enable automated uploads. */
    database->GetSettings()->SetUploadsEnabled(false);

    CrashpadInfo::GetCrashpadInfo()->set_crashpad_handler_behavior(crashpad::TriState::kEnabled);
    CrashpadInfo::GetCrashpadInfo()
            ->set_gather_indirectly_referenced_memory(crashpad::TriState::kEnabled, 0);

    CrashpadClient* client = new CrashpadClient();
    return client->StartHandler(
        handler, db, db, url, annotations, arguments, true, true, {}
    );
}