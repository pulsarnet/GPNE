// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef MACROS_H
#define MACROS_H

#define GPNE_GADGET                                                                                \
public:                                                                                            \
    virtual const QMetaObject* metaObject() const { return &staticMetaObject; }                    \
                                                                                                   \
private:

#define GPNE_GADGET_OVERRIDE                                                                       \
public:                                                                                            \
virtual const QMetaObject* metaObject() const override { return &staticMetaObject; }               \
                                                                                                   \
private:

#define GPNE_GADGET_OVERRIDE                                                                       \
public:                                                                                            \
virtual const QMetaObject* metaObject() const override { return &staticMetaObject; }               \
                                                                                                   \
private:

#endif // MACROS_H
