// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <ptn/reachability.h>
#include <ptn/vec.h>
#include <ptn/place.h>
#include <ptn/types.h>

namespace net = ptn::net;
namespace reachability = ptn::modules::reachability;
namespace vec = ptn::alloc::vec;
namespace vertex = net::vertex;

extern "C" {
    reachability::Reachability* ptn$modules$reachability$init(const net::PetriNet*, size_t);
    std::size_t ptn$modules$reachability$compute(reachability::Reachability*, size_t);
    bool ptn$modules$reachability$hasboundary(const reachability::Reachability*);
    std::size_t ptn$modules$reachability$count(const reachability::Reachability*);
    void ptn$modules$reachability$positions(const reachability::Reachability*, vec::RustVec<vertex::VertexIndex>&);
    rust::repr::Fat ptn$modules$reachability$marking(const reachability::Reachability*);
    void ptn$modules$reachability$drop(reachability::Reachability*);
    std::size_t ptn$modules$reachability$marking$sizeof();
    std::size_t ptn$modules$reachability$marking$alignof();

    void ptn$modules$reachability$marking$values(const reachability::Marking*, vec::RustVec<i32>&);
    reachability::CovType ptn$modules$reachability$marking$type(const reachability::Marking*);
    void ptn$modules$reachability$marking$prev(const reachability::Marking*, vec::RustVec<reachability::MarkingIndex>&);
    void
    ptn$modules$reachability$marking$next(const reachability::Marking*, vec::RustVec<reachability::MarkingIndex>&);
}

reachability::Reachability* reachability::Reachability::init(net::PetriNet* net, size_t hardLimit)
{
    return ptn$modules$reachability$init(net, hardLimit);
}

size_t reachability::Reachability::compute(size_t limit)
{
    return ptn$modules$reachability$compute(this, limit);
}

bool reachability::Reachability::hasBoundary() const
{
    return ptn$modules$reachability$hasboundary(this);
}

size_t reachability::Reachability::count() const
{
    return ptn$modules$reachability$count(this);
}

vec::RustVec<vertex::VertexIndex> reachability::Reachability::positions() const {
    vec::RustVec<vertex::VertexIndex> result{};
    ptn$modules$reachability$positions(this, result);
    return std::move(result);
}

rust::Slice<reachability::Marking> reachability::Reachability::marking() const
{
    return rust::impl<rust::Slice<Marking>>::slice(ptn$modules$reachability$marking(this));
}

void reachability::Reachability::drop() { ptn$modules$reachability$drop(this); }

vec::RustVec<int> reachability::Marking::values() const {
    vec::RustVec<int> result;
    ptn$modules$reachability$marking$values(this, result);
    return result;
}

reachability::CovType reachability::Marking::type() const {
    return ptn$modules$reachability$marking$type(this);
}

vec::RustVec<reachability::MarkingIndex> reachability::Marking::prev() const {
    vec::RustVec<MarkingIndex> result;
    ptn$modules$reachability$marking$prev(this, result);
    return result;
}

vec::RustVec<reachability::MarkingIndex> reachability::Marking::next() const
{
    vec::RustVec<MarkingIndex> result;
    ptn$modules$reachability$marking$next(this, result);
    return result;
}

size_t reachability::Marking::layout::size() noexcept
{
    return ptn$modules$reachability$marking$sizeof();
}

size_t reachability::Marking::layout::align() noexcept
{
    return ptn$modules$reachability$marking$alignof();
}