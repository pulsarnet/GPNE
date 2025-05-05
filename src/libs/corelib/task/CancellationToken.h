// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef CANCELLATIONTOKEN_H
#define CANCELLATIONTOKEN_H
#include <atomic>
#include <memory>

class CancellationToken {

public:
    CancellationToken() : m_cancelled(std::make_shared<std::atomic_bool>(false)) {}

    explicit operator bool() const noexcept { return m_cancelled->load(); }

    void cancel() noexcept { m_cancelled->store(true); }

private:
    std::shared_ptr<std::atomic_bool> m_cancelled;
};

#endif // CANCELLATIONTOKEN_H
