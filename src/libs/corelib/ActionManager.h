// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QSet>

class QToolBar;
class QMenu;
class QAction;
class QMenuBar;

class ActionGroup : public QObject {

    Q_OBJECT

public:
    ActionGroup(QString id) : m_id(id) {}

    const QString& id() const { return m_id; }

    void addSeparator();
    void addAction(const QString& actionId, const QString& relativeActionId = QString(), bool before = true);
    void removeAction(QAction* action);

    QMenu* createMenu(const QString& name);
    QToolBar* createToolBar();

public slots:

    void menuDestroyed(QObject*);
    void toolBarDestroyed(QObject*);

private:

    QString m_id;
    QList<QAction*> m_actions;
    QSet<QMenu*> m_menus;
    QSet<QToolBar*> m_toolbars;
};

class ActionManager : public QObject {
    Q_OBJECT

public:

    static ActionManager* instance();

    static QAction* registerAction(const QString& id, QAction* action);

    static QAction* action(const QString& id);

    static QString actionId(QAction* action);

    static void attachAction(const QString& actionId, QWidget* widget);
    static void attachAction(QAction* action, QWidget* widget);

    static ActionGroup* createActionGroup(const QString& id);

    static ActionGroup* group(const QString& id);

public slots:

    static void actionDestroyed(QObject*);
    static void widgetDestroyed(QObject*);
    static void focusChanged(QWidget* old, QWidget* now);

private:

    ActionManager();

    QHash<QString, QAction*> m_idToAction;
    QHash<QAction*, QString> m_actionToId;

    QHash<QString, ActionGroup*> m_idToActionGroup;
    QHash<ActionGroup*, QString> m_actionGroupToId;

    QHash<QWidget*, QSet<QAction*>> m_widgetToActions;
};


#endif //ACTIONMANAGER_H
