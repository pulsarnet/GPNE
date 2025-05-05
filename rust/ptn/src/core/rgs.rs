/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use std::ops::{Deref, DerefMut};

#[derive(Debug)]
pub struct RestrictedGrowthString {
    a: Vec<u16>,
}

impl Deref for RestrictedGrowthString {
    type Target = [u16];

    fn deref(&self) -> &Self::Target {
        &self.a
    }
}

impl DerefMut for RestrictedGrowthString {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.a
    }
}

#[derive(Debug, thiserror::Error)]
pub enum RestrictedGrowthStringError {
    #[error("empty data")]
    EmptyData,

    #[error("invalid data")]
    InvalidData,
}

impl TryFrom<Vec<u16>> for RestrictedGrowthString {
    type Error = RestrictedGrowthStringError;

    fn try_from(value: Vec<u16>) -> Result<Self, Self::Error> {
        if value.is_empty() {
            Err(RestrictedGrowthStringError::EmptyData)
        } else if Self::validate(&value) {
            Ok(Self { a: value })
        } else {
            Err(RestrictedGrowthStringError::InvalidData)
        }
    }
}

impl RestrictedGrowthString {
    pub fn validate(data: &[u16]) -> bool {
        let n = data.len();
        if n == 0 || data[0] != 0 {
            return false;
        }

        // a0 = 0 
        // ai <= max(a0, a1, ..., ai-1) + 1
        let mut max = data[0] + 1;
        for i in 1..n {
            if data[i] > max {
                return false;
            }
            max = std::cmp::max(max, data[i] + 1);
        }
        true
    }

    pub fn new(n: usize) -> Self {
        Self {
            a: vec![0; n],
        }
    }

    pub fn data(&self) -> &[u16] {
        &self.a
    }
}

pub struct RestrictedGrowthStringGenerator {
    rgs: RestrictedGrowthString,
    b: Vec<u16>,
    m: u16,
}

impl RestrictedGrowthStringGenerator {
    pub fn new(rgs: RestrictedGrowthString) -> Self {
        let data = rgs.data();
        let n = data.len();
        let m = data[0..n - 1].iter().max().unwrap() + 1;
        let mut b = vec![1; n - 1];
        for i in 1..b.len() {
            b[i] = data[..i].iter().max().unwrap() + 1;
        }
        Self { rgs, b, m }
    }

    pub fn get(&self) -> &RestrictedGrowthString {
        &self.rgs
    }

    pub fn inc(&mut self) -> bool {
        let n = self.rgs.len();
        if n == 0 {
            return false;
        }

        if self.rgs[n - 1] == self.m {
            let mut j = n - 2;
            while self.rgs[j] == self.b[j] {
                j -= 1;
            }

            if j == 0 {
                return false;
            }

            self.rgs[j] += 1;

            self.m = self.b[j] + (self.rgs[j] == self.b[j]) as u16;
            j += 1;

            for k in j..(n - 1) {
                self.rgs[k] = 0;
                self.b[k] = self.m;
            }

            self.rgs[n - 1] = 0;
        } else {
            self.rgs[n - 1] += 1;
        }

        true
    }
}