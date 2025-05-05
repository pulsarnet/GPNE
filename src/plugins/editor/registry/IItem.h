// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef IITEM_H
#define IITEM_H

class NetModel;

class IItem {
public:
    virtual ~IItem() = default;

    virtual void submitData() = 0;
    virtual void restoreData() = 0;
    virtual void removeData() = 0;

protected:

    NetModel* model() const noexcept;

private:

    NetModel* m_model = nullptr;

    friend class NetModel;
};

#endif //IITEM_H
