// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
    const char A_NEW_PROJECT[] = "GPNE.Action.NewProject";
    const char A_OPEN_PROJECT[] = "GPNE.Action.OpenProject";
    const char A_CLOSE_PROJECT[] = "GPNE.Action.CloseProject";
    const char A_SAVE_PROJECT[] = "GPNE.Action.SaveProject";
    const char A_SAVE_UNDO[] = "GPNE.Action.Undo";
    const char A_SAVE_REDO[] = "GPNE.Action.Redo";
    const char A_EXIT[] = "GPNE.Action.Exit";
    const char A_ABOUT[] = "GPNE.Action.About";
    const char A_ABOUT_QT[] = "GPNE.Action.AboutQt";
    const char A_RECENT_PROJECTS[] = "GPNE.Action.RecentProjects";
    const char A_DOCK_VIEW[] = "GPNE.Action.DockView";

    const char FILE_GROUP[] = "GPNE.ActionGroup.File";
    const char EDIT_GROUP[] = "GPNE.ActionGroup.Edit";
    const char VIEW_GROUP[] = "GPNE.ActionGroup.View";
    const char TOOLS_GROUP[] = "GPNE.ActionGroup.Tools";
    const char WINDOW_GROUP[] = "GPNE.ActionGroup.Window";
    const char HELP_GROUP[] = "GPNE.ActionGroup.Help";

    const char RECENT_PROJECTS_GROUP[] = "GPNE.ActionGroup.RecentProjects";

    const char ICON_CREATE[] = ":/images/icons/create_project.svg";
    const char ICON_FOLDER[] = ":/images/icons/folder.svg";
    const char ICON_SAVE_PROJECT[] = ":/images/icons/save.svg";
    const char ICON_CLOSE_PROJECT[] = ":/images/icons/close.svg";
    const char ICON_EXIT[] = ":/images/icons/exit.svg";
    const char ICON_ABOUT[] = ":/images/icons/about.svg";
    const char ICON_RELOAD[] = ":/images/reload.svg";
}

#endif //CONSTANTS_H
