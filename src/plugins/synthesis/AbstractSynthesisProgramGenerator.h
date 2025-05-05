// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef ABSTRACTSYNTHESISPROGRAMGENERATOR_H
#define ABSTRACTSYNTHESISPROGRAMGENERATOR_H

#include <random>
#include <algorithm>

class AbstractSynthesisProgramGenerator {

    struct State {
        std::vector<uint16_t> places;
        std::vector<uint16_t> transitions;
    };

    enum Stage {
        FreeChoice,
        MovePlace,
        MoveTransition,
    };

    enum Modification {
        None,
        Place,
        Transition,
    };

public:
    explicit AbstractSynthesisProgramGenerator(int places, int transitions) {
        m_state.places.resize(places, 0);
        m_state.transitions.resize(transitions, 0);
        reset();
    }
    virtual ~AbstractSynthesisProgramGenerator() = default;

    virtual void next(int seed = 0) {
        std::random_device rd;
        std::mt19937 gen(rd());

        // rollback
        if (m_modification == Place) {
            m_state.places[m_lastIndex] = m_lastGroup;
            m_modification = None;
        } else if (m_modification == Transition) {
            m_state.transitions[m_lastIndex] = m_lastGroup;
            m_modification = None;
        }

        if (m_stage == FreeChoice) {
            std::uniform_int_distribution<> dist(0, 1);
            bool changePlace = dist(gen);
            if (changePlace) {
                m_modification = Place;
                modifyGroup(m_state.places, gen);
            } else {
                m_modification = Transition;
                modifyGroup(m_state.transitions, gen);
            }
        } else if (m_stage == MovePlace) {
            m_modification = Place;
            modifyGroup(m_state.places, gen);
        } else if (m_stage == MoveTransition) {
            m_modification = Transition;
            modifyGroup(m_state.transitions, gen);
        }
    }

    const std::vector<uint16_t>& placesAsRGS() const {
        return m_state.places;
    }

    const std::vector<uint16_t>& transitionsAsRGS() const {
        return m_state.transitions;
    }

    void reset() {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<> distPlace(0, m_state.places.size() - 1);
        for (int i = 0; i < m_state.places.size(); ++i) {
            m_state.places[i] = distPlace(gen);
        }

        std::uniform_int_distribution<> distTransition(0, m_state.transitions.size() - 1);
        for (int i = 0; i < m_state.transitions.size(); ++i) {
            m_state.transitions[i] = distTransition(gen);
        }

        m_stage = FreeChoice;
        m_modification = None;
        m_lastGroup = -1;
        m_lastIndex = -1;
    }

    virtual void apply() {
        if (m_modification == Place) {
            m_stage = MoveTransition;
        } else if (m_modification == Transition) {
            m_stage = MovePlace;
        }
        m_modification = None;
    }

private:

    void modifyGroup(std::vector<uint16_t>& arr, std::mt19937& gen) {
        auto it = std::max_element(arr.begin(), arr.end());
        int max = *it == 0 ? arr.size() - 1 : *it - 1;
        std::uniform_int_distribution<> dist(0, max);
        m_lastIndex = it - arr.begin();
        m_lastGroup = dist(gen);
        arr[m_lastIndex] = m_lastGroup;

        // TODO: maybe check if value changed
    }

    Stage m_stage;
    Modification m_modification;

    int m_lastIndex = -1;
    int m_lastGroup = -1;

    State m_state;
};

#endif //ABSTRACTSYNTHESISPROGRAMGENERATOR_H
