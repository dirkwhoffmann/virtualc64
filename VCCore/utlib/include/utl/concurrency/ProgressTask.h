// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/io/IOError.h"
#include "utl/types/Exception.h"
#include "utl/types/Integers.h"
#include <atomic>
#include <filesystem>
#include <fstream>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

namespace utl {

/* What a task reports as it runs. Every hook is optional.
 *
 * IMPORTANT: all of these are called on the task's own thread, not on the one
 * that started it. A caller that has to touch something belonging to another
 * thread -- a window, most likely -- hands the work over itself. Doing that
 * here instead would mean this file knowing about somebody's event loop.
 *
 * 'finished' is always the last one called, whether the task ran to the end,
 * was stopped, or failed. It is the place to take down whatever 'started' put
 * up.
 */
struct ProgressTaskCallbacks {

    std::function<void()> started;
    std::function<void(double)> progressed;

    // The task has moved on to a step worth naming ("Copying...")
    std::function<void(const std::string &)> described;

    std::function<void()> aborted;
    std::function<void(const std::string &)> failed;
    std::function<void()> finished;
};

/* Something slow, run on a thread of its own, that can say how far it has got.
 *
 * The point is to keep work off the thread that draws the window while still
 * being able to show a progress bar. Nothing here knows about any particular
 * user interface: progress is a number between 0 and 1 that can be read at any
 * time from any thread, so a Qt property and a Cocoa timer can both follow it.
 *
 * A body is handed the task it runs inside, which gives it two things: a way
 * to report where it has got to, and a way to find out that somebody has asked
 * it to stop. Long steps are expected to call check() often enough that
 * abort() feels immediate.
 *
 * The thread belongs to this object. That is deliberate: a task whose thread
 * outlives it, or whose body throws with nobody to catch it, is a crash in
 * somebody else's code an hour later.
 */
class ProgressTask {

    std::thread worker;

    std::atomic<double> fraction { 0.0 };
    std::atomic<bool> stopping { false };
    std::atomic<bool> busy { false };

    /* What the task is doing at the moment. A string cannot be an atomic, so
     * this one is behind a lock -- which is affordable because it changes a
     * handful of times per task, unlike the progress beside it.
     */
    mutable std::mutex lock;
    std::string step;

    /* Where the step now running began, so that a weighted step can report
     * within its own slice of the whole. Touched only by the worker.
     */
    double base = 0.0;

    ProgressTaskCallbacks hooks;

public:

    using Body = std::function<void(ProgressTask &)>;

    // Thrown out of check() to unwind a task that has been asked to stop
    struct Aborted : Exception { };

    ProgressTask() = default;
    ~ProgressTask() { abort(); join(); }

    ProgressTask(const ProgressTask &) = delete;
    ProgressTask &operator=(const ProgressTask &) = delete;


    //
    // Running
    //

    /* Hands the body to a new thread and returns at once.
     *
     * Whatever the body throws ends the task through 'failed', with what() as
     * the message; Aborted ends it through 'aborted' instead. Either way
     * 'finished' follows.
     */
    void run(ProgressTaskCallbacks callbacks, Body body) {

        if (busy.load()) throw Exception();

        join();

        hooks = std::move(callbacks);
        fraction.store(0.0);
        stopping.store(false);
        base = 0.0;
        { std::lock_guard<std::mutex> guard(lock); step.clear(); }
        busy.store(true);

        worker = std::thread([this, body = std::move(body)]() {

            auto call = [](const auto &hook, auto &&...args) {
                if (hook) hook(std::forward<decltype(args)>(args)...);
            };

            call(hooks.started);

            try {

                body(*this);
                setProgress(1.0);

            } catch (const Aborted &) {

                call(hooks.aborted);

            } catch (const std::exception &e) {

                call(hooks.failed, std::string(e.what()));

            } catch (...) {

                call(hooks.failed, std::string("Unknown error"));
            }

            /* Cleared before the last hook, so that a 'finished' handler
             * which starts the next task finds this one out of the way.
             */
            busy.store(false);
            call(hooks.finished);
        });
    }

    // Asks the task to stop. Returns at once; the body decides where.
    void abort() { stopping.store(true); }

    // Waits for the thread to end. Safe to call more than once.
    void join() { if (worker.joinable()) worker.join(); }

    bool isRunning() const { return busy.load(); }
    bool isAborting() const { return stopping.load(); }

    // Gives up, from inside a body, if somebody has asked the task to stop
    void check() const { if (isAborting()) throw Aborted(); }


    //
    // Reporting
    //

    // How far along the whole task is, between 0 and 1
    double progress() const { return fraction.load(); }

    // What the task is doing at the moment, empty until it says
    std::string description() const {

        std::lock_guard<std::mutex> guard(lock);
        return step;
    }

    /* Names the step now running, for something that has to tell the user
     * what is going on. Reporting it is all this does: it has no bearing on
     * the progress beside it, so a step may take any share of the whole.
     */
    void setDescription(const std::string &text) {

        {
            std::lock_guard<std::mutex> guard(lock);
            if (step == text) return;
            step = text;
        }
        if (hooks.described) hooks.described(text);
    }

    /* Reports overall progress, and makes that the point a following weighted
     * step carries on from.
     */
    void setProgress(double percentage) {

        auto clamped = percentage < 0.0 ? 0.0 : percentage > 1.0 ? 1.0 : percentage;

        base = clamped;
        report(clamped);
    }


    //
    // Toolbox
    //

    /* Copies a file, reporting as it goes.
     *
     * 'weight' is how much of the whole task this copy accounts for, so a job
     * made of several steps gives each one its share and they add up to one.
     * A copy that is stopped part way removes what it had written: a half
     * file is worse than none, because the next thing along will try to open
     * it.
     */
    void copy(const fs::path &src, const fs::path &dst, double weight = 1.0) {

        constexpr isize chunk = 1024 * 1024;

        std::ifstream in(src, std::ios::binary);
        if (!in) throw IOError(IOError::FILE_CANT_READ, src);

        std::ofstream out(dst, std::ios::binary);
        if (!out) throw IOError(IOError::FILE_CANT_CREATE, dst);

        std::error_code ec;
        const auto total = i64(fs::file_size(src, ec));
        const auto start = base;
        i64 done = 0;

        std::vector<char> buffer(size_t(chunk), '\0');

        try {
            while (in) {

                check();

                in.read(buffer.data(), std::streamsize(chunk));
                const auto read = in.gcount();
                if (read <= 0) break;

                if (!out.write(buffer.data(), read)) {
                    throw IOError(IOError::FILE_CANT_WRITE, dst);
                }

                done += i64(read);
                if (total > 0) report(start + weight * double(done) / double(total));
            }

            out.close();
            if (!out) throw IOError(IOError::FILE_CANT_WRITE, dst);

        } catch (...) {

            out.close();
            fs::remove(dst, ec);
            throw;
        }

        base = start + weight;
        report(base);
    }

private:

    void report(double value) {

        fraction.store(value);
        if (hooks.progressed) hooks.progressed(value);
    }
};

}
