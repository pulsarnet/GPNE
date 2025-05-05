/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use std::collections::HashMap;
use crate::net::vertex::VertexIndex;
use crate::net::{DirectedEdge, InhibitorEdge, PetriNet};
use indexmap::IndexMap;

#[repr(C)]
pub struct UpdateMarking {
    place: VertexIndex,
    // How much take or put
    marking: usize,
}

pub struct Simulation {
    cache: SimulationCache,
    fired: Vec<VertexIndex>,
    // Cycles
    cycles: usize,
}

impl Simulation {
    pub fn new(net: &PetriNet) -> Self {
        Self {
            cache: generate_cache(net),
            fired: vec![],
            cycles: 0,
        }
    }

    /// Returns the number of triggered transitions
    pub fn simulate(&mut self) -> i32 {
        let mut fired = 0;
        let mut fired_transitions = Vec::new();
        let mut take_marking = self.cache.marking.clone();
        take_marking.iter_mut().for_each(|(_, v)| *v = 0);

        let mut put_marking = self.cache.marking.clone();
        put_marking.iter_mut().for_each(|(_, v)| *v = 0);

        for transition in self.cache.transitions.iter() {
            let input = self.cache.input.get(transition).expect("input");
            let output = self.cache.output.get(transition).expect("output");
            let inhibitor = self.cache.inhibitor.get(transition).expect("inhibitor");

            let mut can_fire = true;
            for connection in input.iter() {
                let common_marking_at = *self.cache.marking.get(&connection.begin()).unwrap();
                let took_marking_at = *take_marking.get(&connection.begin()).unwrap();
                if (common_marking_at - took_marking_at) < connection.weight() as usize {
                    can_fire = false;
                    break;
                }
            }

            // still can fire, check inhibitor
            if can_fire {
                for connection in inhibitor.iter() {
                    let marking_at = *self.cache.marking.get(&connection.place()).unwrap();
                    if marking_at > 0 {
                        can_fire = false;
                        break;
                    }
                }
            }

            if can_fire {
                for connection in input.iter() {
                    *take_marking.get_mut(&connection.begin()).unwrap() += connection.weight() as usize;
                }

                for connection in output.iter() {
                    *put_marking.get_mut(&connection.end()).unwrap() += connection.weight() as usize;
                }

                fired_transitions.push(*transition);
                fired += 1;
            }
        }

        // take and put in self.marking
        for (index, take) in take_marking.iter() {
            let put = *put_marking.get(index).unwrap();
            let mark = *self.cache.marking.get(index).unwrap();
            *self.cache.marking.get_mut(index).unwrap() = mark - take + put;
        }
        self.fired = fired_transitions;

        if fired > 0 {
            self.cycles += 1;
        }

        fired
    }

    pub fn markers_at(&self, idx: VertexIndex) -> usize {
        // todo unwrap
        self.cache.marking.get(&idx).copied().unwrap_or(0)
    }

    pub fn cycles(&self) -> usize {
        self.cycles
    }

    pub fn fired(&self) -> &[VertexIndex] {
        &self.fired
    }
}

#[derive(Default)]
struct SimulationCache {
    transitions: Vec<VertexIndex>,
    input: HashMap<VertexIndex, Vec<DirectedEdge>>, // vec<p> to t
    output: HashMap<VertexIndex, Vec<DirectedEdge>>, // t to vec<p>
    inhibitor: HashMap<VertexIndex, Vec<InhibitorEdge>>, // t to vec<p>
    marking: HashMap<VertexIndex, usize>,
}

fn generate_cache(net: &PetriNet) -> SimulationCache {
    let mut cache = SimulationCache::default();
    for (&index, _) in net.transitions().iter() {
        let transition = VertexIndex::transition(index);
        let input = net
            .directed()
            .iter()
            .filter(|c| c.end() == transition)
            .cloned()
            .collect::<Vec<_>>();

        let output = net
            .directed()
            .iter()
            .filter(|c| c.begin() == transition)
            .cloned()
            .collect::<Vec<_>>();

        let inhibitor = net
            .edges()
            .inhibitor()
            .iter()
            .filter(|c| c.transition() == transition)
            .cloned()
            .collect::<Vec<_>>();

        cache.input.insert(transition, input);
        cache.output.insert(transition, output);
        cache.inhibitor.insert(transition, inhibitor);

        cache.transitions.push(transition);
    }

    for (&index, vertex) in net.places().iter() {
        cache.marking.insert(VertexIndex::place(index), vertex.markers());
    }

    cache
}