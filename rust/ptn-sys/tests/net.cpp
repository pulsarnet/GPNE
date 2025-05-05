// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifdef Q_OS_WIN
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "userenv.lib")
#endif

#include <ptn/net.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

namespace net = ptn::net;
namespace vertex = net::vertex;

TEST_CASE("Create Petri Net") {
    auto net = net::PetriNet::create();
    REQUIRE(net);
}

TEST_CASE("Work with vertecies of Petri Net") {
    auto net = net::PetriNet::create();
    //REQUIRE(net != nullptr);
    SECTION("get non-existing vertex from net") {
        REQUIRE(!net->place(1));
        REQUIRE(!net->place(1));
    }
    SECTION("add position to net") {
        auto place_id = net->add_place();
        REQUIRE(net->place(place_id));
        REQUIRE(net->places().size() == 1);
    }
    SECTION("add transition to net") {
        auto transition_id = net->add_transition();
        REQUIRE(net->transition(transition_id));
        REQUIRE(net->transitions().size() == 1);
    }
    SECTION("benchmark") {
        // BENCHMARK
        auto p_bench_net = net::PetriNet::create();
        BENCHMARK("add position to net") {
            return p_bench_net->add_place();
        };

        BENCHMARK("get position from net") {
            return p_bench_net->place(1);
        };

        auto t_bench_net = net::PetriNet::create();
        BENCHMARK("add transition to net") {
            return t_bench_net->add_transition();
        };

        BENCHMARK("get transition from net") {
            return t_bench_net->transition(1);
        };
    }
}