/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

mod edge;
pub mod place;
pub mod transition;
pub mod vertex;

pub use crate::net::edge::{DirectedEdge, Edges, InhibitorEdge};
use crate::net::place::Place;
use crate::net::transition::Transition;
use crate::net::vertex::{VertexIndex, VertexType};
use indexmap::map::IndexMap;
use nalgebra::{DMatrix, Scalar};
use num_traits::{AsPrimitive, One, Zero};
use std::fmt::{Debug, Display, Formatter};
use std::sync::atomic::{AtomicUsize, Ordering};
use tracing::{debug, trace};

#[derive(Debug)]
pub struct PetriNet {
    places: IndexMap<usize, Place>,

    transitions: IndexMap<usize, Transition>,
    
    edges: Edges,
    
    place_index: AtomicUsize,
    
    transition_index: AtomicUsize,
}

impl PetriNet {
    pub fn new() -> Self {
        PetriNet::default()
    }

    pub fn places(&self) -> &IndexMap<usize, Place> {
        &self.places
    }

    pub fn place(&self, key: usize) -> Option<&Place> {
        self.places.get(&key)
    }

    pub fn place_mut(&mut self, key: usize) -> Option<&mut Place> {
        self.places.get_mut(&key)
    }


    pub fn add_place(&mut self, place: Place) -> usize {
        let id = self.next_place_id();
        self.insert_place(id, place, None);
        id
    }

    pub fn insert_place(&mut self, id: usize, place: Place, after: Option<usize>) -> bool {
        trace!("insert place {:?}", place);
        if self.places.contains_key(&id) {
            debug!("place {} already exists. no inserted", id);
            return false;
        }

        // If the current value is less, then set the new one
        self.place_index.fetch_max(id + 1, Ordering::SeqCst);
        let _ = self.places.insert_full(id, place);

        if let Some(after) = after {
            self.move_place(id, after);
        }

        true
    }

    pub fn move_place(&mut self, id: usize, after: usize) {
        if after != id {
            let insert_idx = self.places.get_index_of(&id);
            let after_id = self.places.get_index_of(&after);
            if let (Some(parent_idx), Some(insert_idx)) = (after_id, insert_idx) {
                self.places.move_index(insert_idx, parent_idx + 1);
            }
        }
    }

    pub fn move_place_to_index(&mut self, id: usize, idx: usize) {
        if idx >= self.places.len() {
            return
        }

        let current_idx = self.places.get_index_of(&id);
        if let Some(current_idx) = current_idx {
            self.places.move_index(current_idx, idx);
        }
    }


    pub fn remove_place(&mut self, key: usize) {
        trace!("remove position at {key}");
        if self.places.contains_key(&key) {
            self.edges
                .remove_relative_to_vertex(VertexIndex::place(key));
            self.places.remove(&key);
            debug!("removed place {key}");
        }
    }

    pub fn transitions(&self) -> &IndexMap<usize, Transition> {
        &self.transitions
    }

    pub fn transition(&self, key: usize) -> Option<&Transition> {
        self.transitions.get(&key)
    }

    /// Add transition by index [`VertexIndex`]
    pub fn add_transition(&mut self, transition: Transition) -> usize {
        let id = self.next_transition_id();
        self.insert_transition(id, transition, None);
        id
    }

    pub fn insert_transition(
        &mut self,
        id: usize,
        transition: Transition,
        after: Option<usize>,
    ) -> bool {
        trace!("transition place {:?}", transition);
        if self.transitions.contains_key(&id) {
            debug!("transition {} already exists. no inserted", id);
            return false;
        }

        // If the current value is less, then set the new one
        self.transition_index.fetch_max(id + 1, Ordering::SeqCst);
        let _ = self.transitions.insert_full(id, transition);

        if let Some(after) = after {
            self.move_transition(id, after);
        }

        true
    }

    pub fn move_transition(&mut self, id: usize, after: usize) {
        if after != id {
            let insert_idx = self.transitions.get_index_of(&id);
            let after_id = self.transitions.get_index_of(&after);
            if let (Some(parent_idx), Some(insert_idx)) = (after_id, insert_idx) {
                self.transitions.move_index(insert_idx, parent_idx + 1);
            }
        }
    }

    pub fn move_transition_to_index(&mut self, id: usize, idx: usize) {
        if idx >= self.transitions.len() {
            return
        }

        let current_idx = self.transitions.get_index_of(&id);
        if let Some(current_idx) = current_idx {
            self.transitions.move_index(current_idx, idx);
        }
    }

    /// Remove jump by index [`VertexIndex`]
    pub fn remove_transition(&mut self, key: usize) {
        trace!("remove transition at {key}");
        if self.transitions.contains_key(&key) {
            self.edges
                .remove_relative_to_vertex(VertexIndex::transition(key));
            self.transitions.remove(&key);
            debug!("removed transition {key}");
        }
    }

    pub fn remove(&mut self, index: VertexIndex) {
        match index.type_ {
            VertexType::Place => self.remove_place(index.id),
            VertexType::Transition => self.remove_transition(index.id),
        }
    }

    pub fn edges(&self) -> &Edges {
        &self.edges
    }

    pub fn directed(&self) -> &[DirectedEdge] {
        self.edges.directed()
    }

    /// Find directed arc by `from` and `to` vertex
    pub fn get_directed(&self, from: VertexIndex, to: VertexIndex) -> Option<&DirectedEdge> {
        self.edges.get_directed(from, to)
    }

    pub fn add_directed(&mut self, edge: DirectedEdge) {
        self.edges.add_directed(edge)
    }

    pub fn remove_directed(&mut self, from: VertexIndex, to: VertexIndex) -> Option<DirectedEdge> {
        self.edges.remove_directed(from, to)
    }

    pub fn get_inhibitor(
        &self,
        place: VertexIndex,
        transition: VertexIndex,
    ) -> Option<&InhibitorEdge> {
        self.edges.get_inhibitor(place, transition)
    }

    pub fn add_inhibitor(&mut self, edge: InhibitorEdge) -> bool {
        self.edges.add_inhibitor(edge)
    }

    pub fn remove_inhibitor(&mut self, place: VertexIndex, transition: VertexIndex) {
        self.edges.remove_inhibitor(place, transition)
    }

    #[inline]
    #[must_use]
    pub fn update_position_index(&self) -> usize {
        self.place_index.fetch_add(1, Ordering::SeqCst)
    }

    #[inline]
    pub fn get_position_index(&self) -> usize {
        self.place_index.load(Ordering::SeqCst)
    }

    pub fn next_place_id(&self) -> usize {
        self.update_position_index()
    }

    pub fn adjacency_matrices<I>(&self) -> (DMatrix<I>, DMatrix<I>)
    where
        I: Scalar + Zero + Copy + 'static,
        u32: AsPrimitive<I>,
    {
        let mut input = DMatrix::<I>::zeros(self.places.len(), self.transitions.len());
        let mut output = DMatrix::<I>::zeros(self.places.len(), self.transitions.len());

        for conn in self.edges.directed() {
            let begin = conn.begin();
            let end = conn.end();

            match begin.type_ {
                VertexType::Transition => {
                    let p_idx = self.places.get_index_of(&end.id).unwrap();
                    let t_idx = self.transitions.get_index_of(&begin.id).unwrap();

                    output[(p_idx, t_idx)] = conn.weight().as_();
                }
                VertexType::Place => {
                    let p_idx = self.places.get_index_of(&begin.id).unwrap();
                    let t_idx = self.transitions.get_index_of(&end.id).unwrap();

                    input[(p_idx, t_idx)] = conn.weight().as_();
                }
            }
        }

        (input, output)
    }

    pub fn inhibitor_matrix<I>(&self) -> DMatrix<I>
    where
        I: Scalar + Zero + Copy + 'static,
        u32: AsPrimitive<I>,
    {
        let mut inhibitor = DMatrix::<I>::zeros(self.places.len(), self.transitions.len());

        for arc in self.edges.inhibitor().iter() {
            let place = arc.place();
            let transition = arc.transition();

            let p_idx = self.places.get_index_of(&place.id).unwrap();
            let t_idx = self.transitions.get_index_of(&transition.id).unwrap();

            inhibitor[(p_idx, t_idx)] = 1u32.as_();
        }

        inhibitor
    }

    pub fn one_zero_adjacency_matrices<T>(&self) -> (DMatrix<T>, DMatrix<T>)
    where
        T: Debug + PartialEq + Scalar + Clone + Zero + One,
    {
        let mut input = DMatrix::<T>::zeros(self.places.len(), self.transitions.len());
        let mut output = DMatrix::<T>::zeros(self.places.len(), self.transitions.len());

        for conn in self.edges.directed() {
            let begin = conn.begin();
            let end = conn.end();

            match begin.type_ {
                VertexType::Transition => {
                    let p_idx = self.places.get_index_of(&end.id).unwrap();
                    let t_idx = self.transitions.get_index_of(&begin.id).unwrap();

                    output[(p_idx, t_idx)] = T::one();
                }
                VertexType::Place => {
                    let p_idx = self.places.get_index_of(&begin.id).unwrap();
                    let t_idx = self.transitions.get_index_of(&end.id).unwrap();

                    input[(p_idx, t_idx)] = T::one();
                }
            }
        }

        (input, output)
    }

    #[inline]
    #[must_use]
    pub fn update_transition_index(&self) -> usize {
        self.transition_index.fetch_add(1, Ordering::SeqCst)
    }

    #[inline]
    pub fn get_transition_index(&self) -> usize {
        self.transition_index.load(Ordering::SeqCst)
    }

    pub fn next_transition_id(&self) -> usize {
        self.update_transition_index()
    }

    pub fn marking(&self) -> DMatrix<i32> {
        self.places.values().enumerate().fold(
            DMatrix::zeros(1, self.places.len()),
            |mut acc, (i, place)| {
                acc.row_mut(0)[i] = place.markers() as i32;
                acc
            },
        )
    }
}

impl Default for PetriNet {
    fn default() -> Self {
        PetriNet {
            places: IndexMap::new(),
            transitions: IndexMap::new(),
            edges: Edges::default(),
            place_index: AtomicUsize::new(1),
            transition_index: AtomicUsize::new(1),
        }
    }
}

impl Clone for PetriNet {
    fn clone(&self) -> Self {
        Self {
            places: self.places.clone(),
            transitions: self.transitions.clone(),
            edges: self.edges.clone(),
            place_index: AtomicUsize::new(self.place_index.load(Ordering::SeqCst)),
            transition_index: AtomicUsize::new(self.transition_index.load(Ordering::SeqCst)),
        }
    }
}

impl Display for PetriNet {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        for (id, place) in self.places.iter() {
            if place.markers() > 0 {
                write!(f, "p{}({}) ", id, place.markers())?;
            } else {
                write!(f, "p{} ", id)?;
            }
        }

        writeln!(f, "")?;
        for (id, _) in self.transitions.iter() {
            write!(f, "t{} ", id)?;
        }

        writeln!(f, "")?;
        for edge in self.directed() {
            writeln!(f, "{}--[{}]-->{}", edge.begin(), edge.weight(), edge.end())?;
        }
        Ok(())
    }
}
