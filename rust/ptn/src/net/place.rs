/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

#[derive(Debug, Clone, Default)]
pub struct Place {
    markers: usize,
}

impl Place {
    pub fn markers(&self) -> usize {
        self.markers
    }

    pub fn add_marker(&mut self) {
        self.markers = self.markers.saturating_add(1);
    }

    pub fn remove_marker(&mut self) {
        self.markers = self.markers.saturating_sub(1);
    }

    pub fn set_markers(&mut self, markers: usize) {
        self.markers = markers;
    }
}
