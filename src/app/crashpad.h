// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef CRASHPAD_H
#define CRASHPAD_H

#include <string>

bool startCrashHandler(std::string const& url, std::wstring const& handler_path, std::wstring const& db_path);

#endif //CRASHPAD_H
