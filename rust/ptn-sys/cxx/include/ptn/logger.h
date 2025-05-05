// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef LOGGER_H
#define LOGGER_H

namespace ptn::logger {
    void init();
    void debug(const char* msg, const char* function, const char* filename, int line);
    void info(const char* msg, const char* function, const char* filename, int line);
    void warn(const char* msg, const char* function, const char* filename, int line);
    void error(const char* msg, const char* function, const char* filename, int line);
}

#endif //LOGGER_H
