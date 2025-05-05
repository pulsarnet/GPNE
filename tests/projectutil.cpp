// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <corelib/ProjectUtil.h>
#include <QTest>
#include <iostream>

class projectutil_test : public QObject {
    Q_OBJECT

private slots:

    void canonicalPath() {
        QString path = "hello/world";
        QString canonical = ProjectUtil::canonicalPath(path);
        QVERIFY(canonical.isEmpty());
    }

};

QTEST_MAIN(projectutil_test)

#include "projectutil.moc"