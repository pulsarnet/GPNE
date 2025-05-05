// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ActionManager.h"
#include <QAction>
#include <QApplication>
#include <qcoreapplication.h>
#include <QMenu>
#include <QToolBar>

QList<QWidget*> widgetChain(QWidget* widget);

void ActionGroup::addSeparator() {
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    m_actions.append(separator);
}

void ActionGroup::addAction(const QString &actionId, const QString &relativeActionId, bool before) {
    auto action = ActionManager::action(actionId);
    auto relativeAction = ActionManager::action(relativeActionId);

    if (action) {
        if (!relativeAction) {
            m_actions.append(action);
        } else {
            int index = m_actions.indexOf(relativeAction);
            if (index != -1) {
                if (before) {
                    m_actions.insert(index, action);
                } else {
                    m_actions.insert(index + 1, action);
                }
            } else {
                m_actions.append(action);
            }
        }

        for (auto& menu : m_menus) {
            if (!relativeAction) {
                menu->addAction(action);
            } else if (before) {
                menu->insertAction(relativeAction, action);
            } else {
                // Insert after the relative action
                auto actions = menu->actions();
                auto it = std::find(actions.begin(), actions.end(), relativeAction);
                menu->insertAction(*(++it), action);
            }
        }
    }
}

void ActionGroup::removeAction(QAction *action) {
    m_actions.removeAll(action);
    std::ranges::for_each(m_menus, [action](QMenu* menu) {
        menu->removeAction(action);
    });
}

QMenu* ActionGroup::createMenu(const QString &name) {
    QMenu* menu = new QMenu(name);
    connect(menu, &QObject::destroyed, this, &ActionGroup::menuDestroyed);
    m_menus.insert(menu);

    for (auto& groupAction : m_actions) {
        menu->addAction(groupAction);
    }
    return menu;
}

QToolBar * ActionGroup::createToolBar() {
    QToolBar* toolBar = new QToolBar();
    toolBar->setOrientation(Qt::Horizontal);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->setIconSize(QSize(16, 16));

    connect(toolBar, &QToolBar::destroyed, this, &ActionGroup::toolBarDestroyed);
    m_toolbars.insert(toolBar);

    for (auto& groupAction : m_actions) {
        toolBar->addAction(groupAction);
    }
    return toolBar;
}

void ActionGroup::menuDestroyed(QObject* obj) {
    auto menu = qobject_cast<QMenu*>(obj);
    if (menu) {
        m_menus.remove(menu);
    }
}

void ActionGroup::toolBarDestroyed(QObject *) {
    auto toolBar = qobject_cast<QToolBar*>(sender());
    if (toolBar) {
        m_toolbars.remove(toolBar);
    }
}

ActionManager* ActionManager::instance() {
    static ActionManager instance;
    return &instance;
}

QAction* ActionManager::registerAction(const QString& id, QAction *action) {
    if (!action || id.isEmpty()) {
        return nullptr;
    }

    auto instance = ActionManager::instance();
    if (instance->m_actionToId.contains(action)) {
        return action;
    }
    if (!instance->m_idToAction.contains(id)) {
        instance->m_idToAction.insert(id, action);
        instance->m_actionToId.insert(action, id);
        connect(action, &QObject::destroyed, &ActionManager::actionDestroyed);
        return action;
    }
    return nullptr;
}

QAction* ActionManager::action(const QString& id) {
    return instance()->m_idToAction.value(id, nullptr);
}

QString ActionManager::actionId(QAction *action) {
    return instance()->m_actionToId.value(action, QString());
}

void ActionManager::attachAction(const QString &actionId, QWidget *widget) {
    attachAction(instance()->action(actionId), widget);
}

void ActionManager::attachAction(QAction *action, QWidget *widget) {
    if (!action || !widget) {
        return;
    }

    auto instance = ActionManager::instance();
    instance->m_widgetToActions[widget].insert(action);
    connect(
        widget,
        &QWidget::destroyed,
        &ActionManager::widgetDestroyed
    );
}

ActionGroup* ActionManager::createActionGroup(const QString &id) {
    if (id.isEmpty()) {
        return nullptr;
    }

    auto instance = ActionManager::instance();
    ActionGroup* group = instance->m_idToActionGroup.value(id, nullptr);
    if (!group) {
        group = new ActionGroup(id);
        instance->m_idToActionGroup.insert(id, group);
        instance->m_actionGroupToId.insert(group, id);
    }
    return group;
}

ActionGroup* ActionManager::group(const QString &id) {
    return instance()->m_idToActionGroup.value(id, nullptr);
}

void ActionManager::actionDestroyed(QObject *obj) {
    auto action = qobject_cast<QAction*>(obj);
    if (!action) {
        return;
    }

    auto instance = ActionManager::instance();
    auto id = instance->m_actionToId.value(action, QString());
    if (!id.isEmpty()) {
        instance->m_actionToId.remove(action);
        instance->m_idToAction.remove(id);
    }

    // remove action from groups
    for (auto& group : instance->m_idToActionGroup.values()) {
        group->removeAction(action);
    }

    // remove from m_widgetToActions
    for (auto& values : instance->m_widgetToActions) {
        values.remove(action);
    }
}

void ActionManager::widgetDestroyed(QObject *widget) {
    instance()->m_widgetToActions.remove(qobject_cast<QWidget*>(widget));
}

void ActionManager::focusChanged(QWidget *old, QWidget *now) {
    qDebug().nospace() << "ActionManager::focusChanged(" << old << ", " << now << ")";
    auto oldChain = widgetChain(old);
    auto nowChain = widgetChain(now);

    auto [removed, added] = std::mismatch(
        oldChain.begin(), oldChain.end(),
        nowChain.begin(), nowChain.end()
    );

    // disable actions for the old widget
    for (auto it = removed; it != oldChain.end(); ++it) {
        auto widget = *it;
        auto actions = instance()->m_widgetToActions.value(widget);
        for (auto& action : actions) {
            action->setEnabled(false);
        }
    }

    // enable actions for the new widget
    for (auto it = added; it != nowChain.end(); ++it) {
        auto widget = *it;
        auto actions = instance()->m_widgetToActions.value(widget);
        for (auto& action : actions) {
            action->setEnabled(true);
        }
    }
}

ActionManager::ActionManager() {
    connect(
        qApp,
        &QApplication::focusChanged,
        this,
        &ActionManager::focusChanged
    );
}

QList<QWidget*> widgetChain(QWidget* widget) {
    QList<QWidget*> widgets;
    while (widget) {
        widgets.append(widget);
        widget = widget->parentWidget();
    }
    return widgets;
}
