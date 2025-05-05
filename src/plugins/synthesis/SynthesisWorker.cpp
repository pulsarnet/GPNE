// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SynthesisWorker.h"

#include "AbstractAnalysisModule.h"
#include <corelib/ApplicationProjectController.h>
#include <editor/model/NetModel.h>
#include <float.h>
#include <QtConcurrent/QtConcurrent>

#include <ptn/decompose.h>
#include <ptn/net.h>
#include <QDebug>
#include <queue>
#include <random>
#include <semaphore>

#include "AbstractSynthesisProgramGenerator.h"

#define THREAD_COUNT 16
#define LIMIT_TOTAL 200'000'000
#define MAX_SOLUTIONS 30
#define TIME_LIMIT 3600


SynthesisWorker::SynthesisWorker(std::shared_ptr<SynthesisContext> context)
    : Task(nullptr)
    , m_context(context)
{
    setTitle("Synthesis");
    setSteps(0);

    for (size_t i = 0; i < m_context->analysisModules.size(); ++i) {
        if (m_context->analysisModules[i]->needValidation()) {
            m_preAnalysisModules.push_back(i);
        } else {
            m_postAnalysisModules.push_back(i);
        }
    }

    m_moduleToMetricStorageRange.resize(m_context->analysisModules.size());
    size_t offset = 0;
    for (size_t i = 0; i < m_context->analysisModules.size(); i++) {
        m_moduleToMetricStorageRange[i] = { offset, offset + m_context->analysisModules[i]->metricCount() };
        offset += m_context->analysisModules[i]->metricCount();
    }

    m_totalMetrics = std::accumulate(
        m_context->analysisModules.begin(),
        m_context->analysisModules.end(),
        0,
        [](size_t sum, const std::shared_ptr<AbstractAnalysisModule>& module) {
            return sum + module->metricCount();
        });
}

SynthesisWorker::~SynthesisWorker() {}

void SynthesisWorker::run()
{
    if (!m_context->decomposition) {
        qWarning() << "Context is NULL";
        return;
    }

    // Counter for the number of valid solutions found
    std::atomic_int64_t found = 0;
    // The target number of solutions to find
    size_t limit = MAX_SOLUTIONS;
    // Counter for the total number of iterations performed
    std::atomic_int64_t iterations = 0;

    setMessage(QString("Found %1/%2 in %3 iterations").arg(found.load()).arg(limit).arg(iterations.load()));
    setProgress((double)found / limit * steps());

    std::mutex evaluated_mutex;
    QList<Evaluated> evaluated;

    auto worker = [this, &found, &evaluated, &evaluated_mutex, &iterations, limit]() {
        AbstractSynthesisProgramGenerator generator(m_context->decomposition->positions(), m_context->decomposition->transitions());

        double betterCost = DBL_MAX;
        bool improved = false;
        int iterations_since_improved = 0;

        std::vector<QVariant> metrics(m_totalMetrics);

        while (found < limit) {
            if (isCancel()) {
                break;
            }

            generator.next();
            auto& places = generator.placesAsRGS();
            auto& transitions = generator.transitionsAsRGS();

            improved = false;

            auto placesRGSSlice = rust::Slice<u16>(const_cast<uint16_t*>(places.data()), places.size());
            auto transitionsRGSSlice = rust::Slice<u16>(const_cast<uint16_t*>(transitions.data()), transitions.size());

            auto program = m_context->decomposition->eval_program(placesRGSSlice, transitionsRGSSlice);
            if (!program) {
                continue;
            }

            bool isMatch = true;
            double metricsCost = 0.;

            for (auto idx : m_preAnalysisModules) {
                auto module = m_context->analysisModules[idx];
                // The range of metrics this particular module writes into
                auto [begin , end] = m_moduleToMetricStorageRange[idx];
                // Execute the module and update or gather metrics
                isMatch = isMatch && module->run(program, std::span<QVariant>(metrics.begin() + begin, end - begin), metricsCost);
            }

            if (metricsCost < betterCost) {
                qDebug() << "Improved: " << betterCost << " new cost: " << metricsCost << " iterations: " << iterations_since_improved << "metrics: " << metrics;
                betterCost = metricsCost;
                improved = true;

                generator.apply();
            }

            if (isMatch) {
                for (auto idxModule : m_postAnalysisModules) {
                    auto module = m_context->analysisModules[idxModule];
                    auto [begin , end] = m_moduleToMetricStorageRange[idxModule];
                    module->run(program,
                                std::span<QVariant>(metrics.begin() + begin, end - begin),
                                metricsCost);
                }

                std::lock_guard<std::mutex> lock(evaluated_mutex);
                Evaluated e;
                e.idx = found.fetch_add(1, std::memory_order_seq_cst);
                e.metrics = metrics;
                e.places = places;
                e.transitions = transitions;

                evaluated.push_back(e);
            }

            iterations++;

            ptn::net::PetriNet::drop(program);

            if (betterCost == 0) {
                betterCost = DBL_MAX;
            }

            // If no improvement occurred during this iteration, increment the counter
            if (!improved) {
                iterations_since_improved++;
                // If too many consecutive iterations occur without improvement, reset local search
                if (iterations_since_improved > 5000) {
                    // reset local search
                    iterations_since_improved = 0;
                    betterCost = DBL_MAX;
                    generator.reset();
                }
            }
        }
    };

    std::vector<std::thread> pool;
    for (int i = 0; i < THREAD_COUNT; i++) {
        pool.emplace_back(worker);
    }

    setMessage(QString("Found %1/%2 in %3 iterations").arg(found.load()).arg(limit).arg(iterations.load()));

    int sent = 0;
    while (sent < limit) {
        if (isCancel()) {
            break;
        }

        QList<Evaluated> swap;
        {
            std::lock_guard guard(evaluated_mutex);
            evaluated.swap(swap);
        }

        setMessage(QString("Found %1/%2 in %3 iterations").arg(found.load()).arg(limit).arg(iterations.load()));

        if (swap.empty()) {
            continue;
        }

        sent += swap.count();
        emit programsEvaluated(swap);
    }

    for (auto& thread : pool) {
        thread.join();
    }
}

// generate random program

// size_t threadCount = std::max(1, QThread::idealThreadCount() - 4); // todo settings
// size_t programs = m_context->programs();
// size_t threads = std::min(programs, threadCount);
// size_t programs_per_thread = programs / threads;
// std::vector<std::thread> pool;
// std::mutex evaluated_mutex;
// QList<Evaluated> evaluated;
// for (size_t thread = 0; thread < threads; thread++) {
//     pool.push_back(
//         std::thread([this,
//                      programs_per_thread,
//                      thread,
//                      threads,
//                      programs,
//                      &evaluated_mutex,
//                      &evaluated]() {
//             size_t begin = programs_per_thread * thread;
//             size_t end = thread == (threads - 1) ? programs : begin + programs_per_thread;
//
//             for (size_t i = begin; i < end; i++) {
//                 if (isCancel()) {
//                     break;
//                 }
//
//                 auto program = m_context->eval_program(i);
//                 Evaluated e;
//                 e.idx = i;
//                 e.places = program->places_count();
//                 e.transitions = program->transitions_count();
//                 auto connections = program->directed_arcs();
//                 for (auto& conn : connections) {
//                     e.weight += conn.weight();
//                 }
//                 program->drop();
//
//                 // if (e.weight > m_maxEdgeWight) {
//                 //     // skip
//                 //     continue;
//                 // }
//
//                 std::lock_guard guard(evaluated_mutex);
//                 evaluated.push_back(e);
//             }
//         })
//     );
// }
//
// size_t total = 0;
// while (total < programs) {
//     if (isCancel()) {
//         break;
//     }
//
//     QList<Evaluated> swap;
//     {
//         std::lock_guard guard(evaluated_mutex);
//         evaluated.swap(swap);
//     }
//
//     if (swap.empty()) {
//         continue;
//     }
//
//     total += swap.count();
//     emit programsEvaluated(swap);
//
//     setMessage(QString("Generating %1/%2").arg(total).arg(programs));
//     setProgress(((double)total / programs) * steps());
//
//     qWarning() << "Total: " << total;
// }
//
// for (auto& thread : pool) {
//     thread.join();
// }
