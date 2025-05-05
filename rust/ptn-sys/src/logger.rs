/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use libc::c_char;
use std::ffi::CStr;
use std::fs::OpenOptions;
use tracing::{debug, error, info, warn};
use tracing_subscriber::layer::SubscriberExt;
use tracing_subscriber::util::SubscriberInitExt;
use tracing_subscriber::{filter, fmt, Layer};

#[export_name = "ptn$logger$init"]
extern "C" fn init() {
    let debug_file = OpenOptions::new()
        .truncate(true)
        .create(true)
        .write(true)
        .open("log-debug.log")
        .unwrap();
    
    let stdout_write = tracing_subscriber::fmt::layer()
        .with_ansi(false)
        .with_thread_ids(true)
        .with_target(true)
        //.with_writer(debug_file).compact()
        .pretty();

    tracing_subscriber::registry()
        .with(stdout_write)
        //.with(stdout_write.with_filter(tracing_subscriber::filter::EnvFilter::from_default_env()))
        .init();
}

fn try_char_to_str(msg: *const c_char) -> Option<&'static str> {
    if msg.is_null() {
        return None;
    }

    unsafe { CStr::from_ptr(msg) }.to_str().ok()
}

#[export_name = "ptn$logger$debug"]
extern "C" fn external_debug_log(
    msg: *const c_char,
    function: *const c_char,
    filename: *const c_char,
    line: i32,
) {
    let msg = try_char_to_str(msg).unwrap_or_default();
    let function = try_char_to_str(function).unwrap_or("-");
    let filename = try_char_to_str(filename).unwrap_or("-");

    debug!(target: "external",
        function = function,
        filename = filename,
        line = line,
        "{msg}"
    );
}

#[export_name = "ptn$logger$info"]
extern "C" fn external_info_log(
    msg: *const c_char,
    function: *const c_char,
    filename: *const c_char,
    line: i32,
) {
    let msg = try_char_to_str(msg).unwrap_or_default();
    let function = try_char_to_str(function).unwrap_or("-");
    let filename = try_char_to_str(filename).unwrap_or("-");

    info!(target: "external",
        function = function,
        filename = filename,
        line = line,
        "{msg}"
    );
}

#[export_name = "ptn$logger$warn"]
extern "C" fn external_warn_log(
    msg: *const c_char,
    function: *const c_char,
    filename: *const c_char,
    line: i32,
) {
    let msg = try_char_to_str(msg).unwrap_or_default();
    let function = try_char_to_str(function).unwrap_or("-");
    let filename = try_char_to_str(filename).unwrap_or("-");

    warn!(target: "external",
        function = function,
        filename = filename,
        line = line,
        "{msg}"
    );
}

#[export_name = "ptn$logger$error"]
extern "C" fn external_error_log(
    msg: *const c_char,
    function: *const c_char,
    filename: *const c_char,
    line: i32,
) {
    let msg = try_char_to_str(msg).unwrap_or_default();
    let function = try_char_to_str(function).unwrap_or("-");
    let filename = try_char_to_str(filename).unwrap_or("-");

    error!(target: "external",
        function = function,
        filename = filename,
        line = line,
        "{msg}"
    );
}
