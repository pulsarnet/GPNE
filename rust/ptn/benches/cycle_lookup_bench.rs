/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use criterion::{criterion_group, criterion_main, Criterion};
use ptn::core::johnson_circuit::johnson_circuit;
use ptn::net::place::Place;
use ptn::net::transition::Transition;
use ptn::net::vertex::VertexIndex;
use ptn::net::{DirectedEdge, PetriNet};
use std::time::Instant;

fn full_connected_petri_net(places: usize, transitions: usize) -> PetriNet {
    let mut net = PetriNet::new();

    for _ in 0..places {
        net.add_place(Place::default());
    }

    for _ in 0..transitions {
        net.add_transition(Transition::default());
    }

    let places = net.places().iter().map(|(p_idx, _)| VertexIndex::place(*p_idx)).collect::<Vec<_>>();
    let transitions = net.transitions().iter().map(|(t_idx, _)| VertexIndex::transition(*t_idx)).collect::<Vec<_>>();
    for p_idx in places {
        for t_idx in transitions.iter().copied() {
            net.add_directed(DirectedEdge::new(p_idx, t_idx));
            net.add_directed(DirectedEdge::new(t_idx, p_idx));
        }
    }

    net
}


pub fn cycle_lookup_benchmark(c: &mut Criterion) {
    let mut group = c.benchmark_group("Full connected Petri Net");
    group.sample_size(10);
    let data = [
        (2, full_connected_petri_net(2, 2)),
        (3, full_connected_petri_net(3, 3)),
        (4, full_connected_petri_net(4, 4)),
        (5, full_connected_petri_net(5, 5)),
        (6, full_connected_petri_net(6, 6)),
        (7, full_connected_petri_net(7, 4)),
        (8, full_connected_petri_net(8, 8)),
    ];
    for (i, net) in data.iter() {
        let time = Instant::now();
        let result = johnson_circuit(net);
        println!("Elapsed time for {i} is {:#?}, circuits {}", time.elapsed(), result.len());
        // group.bench_with_input(
        //     BenchmarkId::new("Johnson circuit", i),
        //     net,
        //     |b, net| {
        //         b.iter(|| black_box(johnson_circuit(net)))
        //     });
    }
    group.finish();


    let time = Instant::now();
    let result = johnson_circuit(&data[0].1);
    println!("Elapsed time {:#?} - {}", time.elapsed(), result.len());
}

criterion_group!(benches, cycle_lookup_benchmark);
criterion_main!(benches);