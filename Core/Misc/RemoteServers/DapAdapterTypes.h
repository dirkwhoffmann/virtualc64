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

#pragma once

#include "json.h"

using json = nlohmann::json;

namespace vc64 {

struct BreakpointLocation {
    
    std::optional<isize> line;
    std::optional<isize> column;
    std::optional<isize> endLine;
    std::optional<isize> endColumn;

    BreakpointLocation() = default;

    explicit BreakpointLocation(const json &j) {
        
        if (j.contains("line"))      line      = j["line"].get<isize>();
        if (j.contains("column"))    column    = j["column"].get<isize>();
        if (j.contains("endLine"))   endLine   = j["endLine"].get<isize>();
        if (j.contains("endColumn")) endColumn = j["endColumn"].get<isize>();
    }
    
    json asJson() const {
        
        json j;
        if (line)      j["line"] = *line;
        if (column)    j["column"] = *column;
        if (endLine)   j["endLine"] = *endLine;
        if (endColumn) j["endColumn"] = *endColumn;
        return j;
    }
};

struct BreakpointLocations {
    
    std::vector<BreakpointLocation> locations;

    BreakpointLocations() = default;
    
    explicit BreakpointLocations(const json &j) {
        
        if (j.is_array()) {
            locations.reserve(j.size());
            for (const auto &item : j) {
                locations.emplace_back(item);
            }
        }
    }
    
    // Find the first breakpoint with a given line number
    const BreakpointLocation* findByLine(isize line) const {
        
        for (const auto &bp : locations) {
            if (bp.line && *bp.line == line) return &bp;
        }
        return nullptr;
    }
    
    // Convert back to JSON
    json asJson() const {
        
        json arr = json::array();
        for (const auto &bp : locations) arr.push_back(bp.asJson());
        return arr;
    }
};

struct SourceBreakpoint {
    
    std::optional<isize> line;
    std::optional<isize> column;
    std::string condition;
    std::string hitCondition;
    std::string logMessage;
    std::string mode;
    
    explicit SourceBreakpoint(const json &j) {
        
        if (j.contains("line"))      line      = j["line"].get<isize>();
        if (j.contains("column"))    column    = j["column"].get<isize>();
        
        condition    = j.value("condition", "");
        hitCondition = j.value("hitCondition", "");
        logMessage   = j.value("logMessage", "");
        mode         = j.value("mode", "");
    }
    
    json asJson() const {
        
        json j;
        if (line)                   j["line"] = *line;
        if (column)                 j["column"] = *column;
        if (!condition.empty())     j["condition"] = condition;
        if (!hitCondition.empty())  j["hitCondition"] = hitCondition;
        if (!logMessage.empty())    j["logMessage"] = logMessage;
        if (!mode.empty())          j["mode"] = mode;
        return j;
    }
    
    bool matchesLine(isize targetLine) const {
        return line && *line == targetLine;
    }
};

struct SourceBreakpoints {

    std::vector<SourceBreakpoint> breakpoints;

    explicit SourceBreakpoints(const json &j) {

        if (j.is_array()) {
            breakpoints.reserve(j.size());
            for (const auto &item : j) {
                breakpoints.emplace_back(item);
            }
        }
    }

    // Find the first breakpoint with a given line number
    const SourceBreakpoint* findByLine(isize line) const {

        for (const auto &bp : breakpoints) {
            if (bp.line && *bp.line == line) return &bp;
        }
        return nullptr;
    }

    // Convert back to JSON
    json asJson() const {

        json arr = json::array();
        for (const auto &bp : breakpoints) arr.push_back(bp.asJson());
        return arr;
    }
};

}
