/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use crate::net::vertex::VertexIndex;
use crate::net::PetriNet;
use nalgebra::DMatrix;
use std::cmp::Ordering;
use std::collections::{HashMap, VecDeque};
use std::fmt::{Display, Formatter};
use std::ops::{Add, AddAssign, Index, IndexMut, SubAssign};
use std::slice::Iter;

#[allow(unused)]
mod decompose;
#[allow(unused)]
pub mod simulation;

#[derive(PartialEq, Clone, Debug, Copy)]
#[repr(u8)]
pub enum CovType {
    DeadEnd = 0,
    Inner = 1,
    Boundary = 2,
    Duplicate = 3,
}

#[derive(Clone, Copy, Debug, Hash)]
pub enum MarkerValue {
    Value(i32),
    Infinity,
}

impl MarkerValue {
    /// -1 - inf
    /// 0..=i32::MAX - number
    pub fn as_number(&self) -> i32 {
        match self {
            MarkerValue::Value(v) => *v,
            MarkerValue::Infinity => -1,
        }
    }
}

impl Ord for MarkerValue {
    fn cmp(&self, other: &Self) -> Ordering {
        match (self, other) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => a.cmp(b),
            (MarkerValue::Infinity, MarkerValue::Infinity) => Ordering::Less,
            (MarkerValue::Value(_), MarkerValue::Infinity) => Ordering::Less,
            (MarkerValue::Infinity, MarkerValue::Value(_)) => Ordering::Greater,
        }
    }
}

impl Eq for MarkerValue {}

impl PartialEq for MarkerValue {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => *a == *b,
            (MarkerValue::Infinity, MarkerValue::Infinity) => true,
            (MarkerValue::Value(_), MarkerValue::Infinity)
            | (MarkerValue::Infinity, MarkerValue::Value(_)) => false,
        }
    }
}

impl PartialEq<i32> for MarkerValue {
    fn eq(&self, other: &i32) -> bool {
        match self {
            MarkerValue::Value(a) => a == other,
            MarkerValue::Infinity => false,
        }
    }
}

impl PartialOrd for MarkerValue {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        match (self, other) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => a.partial_cmp(b),
            (MarkerValue::Infinity, MarkerValue::Infinity) => Some(Ordering::Less),
            (MarkerValue::Value(_), MarkerValue::Infinity) => Some(Ordering::Less),
            (MarkerValue::Infinity, MarkerValue::Value(_)) => Some(Ordering::Greater),
        }
    }
}

impl PartialOrd<i32> for MarkerValue {
    fn partial_cmp(&self, other: &i32) -> Option<Ordering> {
        match self {
            MarkerValue::Value(a) => a.partial_cmp(other),
            MarkerValue::Infinity => Some(Ordering::Greater),
        }
    }
}

impl Add for MarkerValue {
    type Output = Self;

    fn add(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (MarkerValue::Value(a), MarkerValue::Value(b)) => MarkerValue::Value(a + b),
            (MarkerValue::Infinity, MarkerValue::Infinity)
            | (MarkerValue::Value(_), MarkerValue::Infinity)
            | (MarkerValue::Infinity, MarkerValue::Value(_)) => MarkerValue::Infinity,
        }
    }
}

impl Display for MarkerValue {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            MarkerValue::Value(a) => {
                write!(f, "{}", a)
            }
            MarkerValue::Infinity => {
                write!(f, "w")
            }
        }
    }
}

impl SubAssign<i32> for MarkerValue {
    fn sub_assign(&mut self, rhs: i32) {
        if let MarkerValue::Value(a) = self {
            *a -= rhs;
        }
    }
}

impl AddAssign<i32> for MarkerValue {
    fn add_assign(&mut self, rhs: i32) {
        if let MarkerValue::Value(a) = self {
            *a += rhs;
        }
    }
}

impl num_traits::Zero for MarkerValue {
    fn zero() -> Self {
        MarkerValue::Value(0)
    }

    fn is_zero(&self) -> bool {
        matches!(self, MarkerValue::Value(0))
    }
}

#[derive(Clone, Debug)]
pub struct Marking {
    type_: CovType,
    data: DMatrix<MarkerValue>,
    prev: Vec<(VertexIndex, usize)>,
    next: Vec<(VertexIndex, usize)>,
}

impl Marking {
    pub fn data(&self) -> &DMatrix<MarkerValue> {
        &self.data
    }

    pub fn r#type(&self) -> CovType {
        self.type_
    }

    pub fn next(&self) -> &[(VertexIndex, usize)] {
        self.next.as_ref()
    }
    
    pub fn prev(&self) -> &[(VertexIndex, usize)] {
        self.prev.as_ref()
    }
}

impl From<DMatrix<i32>> for Marking {
    fn from(data: DMatrix<i32>) -> Self {
        let mut marking = DMatrix::<MarkerValue>::zeros(data.nrows(), data.ncols());
        for row in 0..data.nrows() {
            for col in 0..data.ncols() {
                marking.row_mut(row)[col] = MarkerValue::Value(data.row(row)[col]);
            }
        }
        Marking::from(marking)
    }
}

impl From<DMatrix<MarkerValue>> for Marking {
    fn from(data: DMatrix<MarkerValue>) -> Self {
        Marking {
            type_: CovType::Boundary,
            data,
            prev: vec![],
            next: vec![],
        }
    }
}

pub struct Reachability {
    transitions: Vec<VertexIndex>,

    input: DMatrix<i32>,
    output: DMatrix<i32>,
    inhibitor: DMatrix<i32>,

    marking: Marking,

    graph: ReachabilityGraph,
    boundary: VecDeque<usize>,
    hash: HashMap<DMatrix<MarkerValue>, Vec<usize>>,
    hard_limit: usize,
}

impl Reachability {
    pub fn new(net: &PetriNet, hard_limit: usize) -> Self {
        let (input, output) = net.adjacency_matrices::<i32>();
        let inhibitor = net.inhibitor_matrix();
        let marking = Marking::from(net.marking());
        let places = net.places().keys().map(|key| VertexIndex::place(*key)).collect();

        Reachability {
            transitions: net
                .transitions()
                .keys()
                .map(|key| VertexIndex::transition(*key))
                .collect(),
            input,
            output,
            inhibitor,
            marking: marking.clone(),
            graph: ReachabilityGraph::new(marking, places),
            boundary: VecDeque::from([0]),
            hash: HashMap::new(),
            hard_limit,
        }
    }

    pub fn graph(&self) -> &ReachabilityGraph {
        &self.graph
    }

    pub fn has_boundary(&self) -> bool {
        !self.boundary.is_empty()
    }

    /// Get possible markings from the current one
    fn selector(&self, marking: &Marking) -> Vec<Selected> {
        let mut markings = vec![];

        'main: for transition in 0..self.input.ncols() {
            let input_col = self.input.column(transition);
            let output_col = self.output.column(transition);
            let inhibitor_col = self.inhibitor.column(transition);

            for position in 0..self.input.nrows() {
                if inhibitor_col[position] != 0 {
                    // if it's inhibitor arc
                    if marking.data.row(0)[position] != 0 {
                        // check that the place has no markers
                        continue 'main;
                    }
                } else if input_col[position] != 0 {
                    // if it's directed arc
                    if marking.data.row(0)[position] < input_col[position] {
                        // check that the place has more markers than arc weight
                        continue 'main;
                    }
                }
            }

            let mut new_marking = Marking::from(marking.data.clone());
            for position in 0..self.input.nrows() {
                new_marking.data.row_mut(0)[position] -= input_col[position];
                new_marking.data.row_mut(0)[position] += output_col[position];
            }

            let transition = self.transitions[transition];
            markings.push({
                Selected {
                    transition,
                    marking: new_marking,
                    has_w: false,
                }
            });
        }

        markings
    }

    /// Returns a tree of achievable layouts
    pub fn compute(&mut self, limit: usize) -> usize {
        let generated = self.graph.count();
        let mut limit = generated + limit;
        if self.hard_limit > 0 {
            limit = std::cmp::min(limit, self.hard_limit)
        }

        'main: loop {
            if self.graph.count() >= limit {
                break
            }

            let Some(marking) = self.boundary.pop_front() else { break };
            // If there is another vertex Y in the tree that is not a boundary, 
            // and the same label is associated with it, M(X) = M(Y), 
            // then vertex X is a duplicate
            // if let Some(_) = tree
            //     .iter()
            //     .filter(|mark| mark.type_ != CovType::Boundary)
            //     .position(|mark| mark.data == tree[marking].data)
            // {
            //     tree[marking].type_ = CovType::Duplicate;
            //     continue;
            // }

            // If more than one of the transitions is not allowed for labeling X, then the vertex X is terminal
            let mut selector = self.selector(&self.graph[marking]);
            if selector.is_empty() {
                self.graph[marking].type_ = CovType::DeadEnd;
                continue;
            }

            // For every transition t allowed in M(X), create a new vertex Z of the reachability tree
            for selected in selector.iter_mut() {
                // On the path from the root to M if there exists a marking M"
                // such that M'(p)≥M"(p) for each place p and M'M", i.e., M" is coverable,
                // then replace M'(p) by o for each p such that M'(p)>M"(p).
                let mut prev = Some(marking);
                while let Some(i) = prev {
                    if i == 0 {
                        // If this is the root of the tree, then we do not take it into account.
                        break;
                    }

                    prev = self.graph[i].prev.get(0).map(|v| v.1);

                    if selected.marking.data.row(0) >= self.graph[i].data.row(0)
                        && selected.marking.data.row(0) != self.graph[i].data.row(0)
                    {
                        for position in 0..self.input.nrows() {
                            if selected.marking.data[(0, position)] > self.graph[i].data[(0, position)] {
                                selected.marking.data[(0, position)] = MarkerValue::Infinity;
                                selected.has_w = true;
                                // todo
                            }
                        }
                    }
                }
            }

            // The arc labeled tj is directed from vertex x to vertex z. 
            // Vertex x is redefined as an internal, vertex z becomes boundary.
            for mut selected in selector.into_iter() {
                let contains_w = selected
                    .marking
                    .data
                    .iter()
                    .find(|w| **w == MarkerValue::Infinity)
                    .is_some();
                if contains_w {
                    selected.marking.type_ = CovType::DeadEnd;
                } else {
                    selected.marking.type_ = CovType::Boundary;
                }
                // select.type_ = CovType::Boundary;
                selected.marking.prev.push((selected.transition, marking));

                let entry = self.hash.entry(selected.marking.data.clone()).or_insert_with(Vec::new);
                entry.push(self.graph.count());
                if entry.len() == 1 && selected.marking.type_ == CovType::Boundary {
                    self.boundary.push_back(self.graph.count())
                }
                self.graph.append(selected.marking);

                let index = self.graph.count() - 1;
                self.graph[marking].next.push((selected.transition, index));
                self.graph[marking].type_ = CovType::Inner;

                if self.graph.count() >= limit {
                    break 'main
                }
            }
        }

        self.graph.count() - generated
    }
}

struct Selected {
    transition: VertexIndex,
    marking: Marking,
    has_w: bool,
}

pub struct ReachabilityGraph {
    positions: Vec<VertexIndex>,
    markings: Vec<Marking>,
}

impl ReachabilityGraph {
    fn new(init: Marking, positions: Vec<VertexIndex>) -> Self {
        ReachabilityGraph {
            positions,
            markings: vec![init],
        }
    }

    pub fn positions(&self) -> &[VertexIndex] {
        &self.positions
    }

    pub fn markings(&self) -> &[Marking] {
        &self.markings
    }

    fn get_boundary(&self) -> Option<&Marking> {
        self.markings
            .iter()
            .find(|mark| mark.type_ == CovType::Boundary)
    }

    fn has_boundary(&self) -> bool {
        self.get_boundary().is_some()
    }

    pub fn count(&self) -> usize {
        self.markings.len()
    }

    fn append(&mut self, value: Marking) {
        self.markings.push(value)
    }

    fn iter(&self) -> Iter<'_, Marking> {
        self.markings.iter()
    }
}

impl Index<usize> for ReachabilityGraph {
    type Output = Marking;

    fn index(&self, index: usize) -> &Self::Output {
        &self.markings[index]
    }
}

impl IndexMut<usize> for ReachabilityGraph {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        &mut self.markings[index]
    }
}

#[cfg(test)]
mod tests {
    use crate::net::place::Place;
    use crate::net::transition::Transition;
    use crate::net::vertex::VertexIndex;
    use crate::net::{DirectedEdge, PetriNet};

    #[test]
    pub fn test_cov() {
        let mut net = PetriNet::new();
        let p1 = VertexIndex::place(net.add_place(Place::default()));
        let p2 = VertexIndex::place(net.add_place(Place::default()));
        let p3 = VertexIndex::place(net.add_place(Place::default()));
        let p4 = VertexIndex::place(net.add_place(Place::default()));

        let t1 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t2 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t3 = VertexIndex::transition(net.add_transition(Transition::default()));

        // p1
        net.add_directed(DirectedEdge::new(p1, t1));
        net.add_directed(DirectedEdge::new(p1, t2));
        net.add_directed(DirectedEdge::new(t1, p1));

        // p2
        net.add_directed(DirectedEdge::new(p2, t3));
        net.add_directed(DirectedEdge::new(t1, p2));

        // p3
        net.add_directed(DirectedEdge::new(p3, t3));
        net.add_directed(DirectedEdge::new(t3, p3));
        net.add_directed(DirectedEdge::new(t2, p3));

        // p4
        net.add_directed(DirectedEdge::new(t3, p4));

        // marking
        net.place_mut(p1.id).expect("place").add_marker();

        // let cov = Reachability::new(&net);
        // let tree = cov.compute().expect("non empty tree");
        //
        // let mut current = vec![0];
        // let mut level = 0;
        // loop {
        //     if current.is_empty() {
        //         break;
        //     }
        //
        //     current = current
        //         .clone()
        //         .iter()
        //         .map(|i| &tree[*i])
        //         .map(|m| &m.next)
        //         .flatten()
        //         .map(|(_, i)| *i)
        //         .collect::<Vec<_>>();
        //
        //     level += 1;
        // }
    }
}
