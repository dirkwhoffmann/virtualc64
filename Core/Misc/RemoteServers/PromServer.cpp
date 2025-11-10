// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "PromServer.h"
#include "Emulator.h"
#include "httplib.h"
#include <thread>

namespace vc64 {

void
PromServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    HttpServer::_dump(category, os);
}

string
PromServer::respond(const httplib::Request& request)
{
    std::ostringstream output;

    auto translate = [&](const string& metric,
                         const string& help,
                         const string& type,
                         auto value,
                         const std::map<string, string>& labels = {}) {

        if (!help.empty()) {
            output << "# HELP " << metric << " " << help << "\n";
        }
        if (!metric.empty()) {
            output << "# TYPE " << metric << " " << type << "\n";
        }
        output << metric;
        if (!labels.empty()) {
            output << "{";
            bool first = true;
            for (const auto& [key,val] : labels) {
                if (!first) output << ",";
                output << key << "=\"" << val << "\"";
                first = false;
            }
            output << "}";
        }
        output << " " << value << "\n\n";
    };

    output << std::fixed << std::setprecision(4);

    {   auto stats = emulator.getStats();

        translate("vc64_cpu_load", "",
                  "gauge", stats.cpuLoad,
                  {{"component","emulator"}});

        translate("vc64_fps", "",
                  "gauge", stats.fps,
                  {{"component","emulator"}});

        translate("vc64_resyncs", "",
                  "gauge", stats.resyncs,
                  {{"component","emulator"}});
    }

    /*
    {   auto stats = agnus.getStats();

        translate("vamiga_activity_copper", "",
                  "gauge", stats.copperActivity,
                  {{"component","agnus"}});

        translate("vamiga_activity_blitter", "",
                  "gauge", stats.blitterActivity,
                  {{"component","agnus"}});

        translate("vamiga_activity_disk", "",
                  "gauge", stats.diskActivity,
                  {{"component","agnus"}});

        translate("vamiga_activity_disk", "",
                  "gauge", stats.diskActivity,
                  {{"component","agnus"}});

        translate("vamiga_activity_audio", "",
                  "gauge", stats.audioActivity,
                  {{"component","agnus"}});

        translate("vamiga_activity_sprite", "",
                  "gauge", stats.spriteActivity,
                  {{"component","agnus"}});

        translate("vamiga_activity_bitplane", "",
                  "gauge", stats.bitplaneActivity,
                  {{"component","agnus"}});
    }
    */

    {   auto stats_1 = cia1.getStats();
        auto stats_2 = cia2.getStats();

        translate("vc64_ciaa_idle_sec", "",
                  "gauge", stats_1.idleCycles,
                  {{"component","cia1"}});
        translate("vc64_ciab_idle_sec", "",
                  "gauge", stats_2.idleCycles,
                  {{"component","cia2"}});

        translate("vc64_cia_idle_sec_total", "",
                  "gauge", stats_1.totalCycles,
                  {{"component","cia1"}});
        translate("vc64_cia_idle_sec_total", "",
                  "gauge", stats_2.totalCycles,
                  {{"component","cia2"}});

        translate("vc64_cia_idle_percentage", "",
                  "gauge", stats_1.idlePercentage,
                  {{"component","cia1"}});
        translate("vc64_cia_idle_percentage", "",
                  "gauge", stats_2.idlePercentage,
                  {{"component","cia2"}});
    }

    {   auto stats = mem.getStats();

        translate("vc64_mem_accesses", "",
                  "gauge", stats.reads,
                  {{"component","memory"},{"type","read"}});
        translate("vc64_mem_accesses", "",
                  "gauge", stats.writes,
                  {{"component","memory"},{"type","write"}});
    }

    {   auto stats = audioPort.getStats();

        translate("vc64_audio_buffer_exceptions", "",
                  "gauge", stats.bufferOverflows,
                  {{"component","audio"},{"type","overflow"}});
        translate("vc64_audio_buffer_exceptions", "",
                  "gauge", stats.bufferUnderflows,
                  {{"component","audio"},{"type","underflow"}});

        translate("vc64_audio_samples", "",
                  "gauge", stats.consumedSamples,
                  {{"component","audio"},{"type","consumed"}});
        translate("vc64_audio_samples", "",
                  "gauge", stats.producedSamples,
                  {{"component","audio"},{"type","produced"}});
        translate("vc64_audio_samples", "",
                  "gauge", stats.idleSamples,
                  {{"component","audio"},{"type","idle"}});

        translate("vc64_audio_fill_level", "",
                  "gauge", stats.fillLevel,
                  {{"component","audio"}});
    }

    return output.str();
}

void
PromServer::main()
{
    try {

        // Create the HTTP server
        if (!srv) srv = new httplib::Server();

        // Define the "/metrics" endpoint where Prometheus will scrape metrics
        srv->Get("/metrics", [this](const httplib::Request& req, httplib::Response& res) {

            switchState(SrvState::CONNECTED);
            res.set_content(respond(req), "text/plain");
        });

        // Start the server to listen on localhost
        debug(SRV_DEBUG, "Starting Prometheus data provider\n");
        srv->listen("localhost", (int)config.port);

    } catch (std::exception &err) {

        debug(SRV_DEBUG, "Server thread interrupted\n");
        handleError(err.what());
    }
}

}
