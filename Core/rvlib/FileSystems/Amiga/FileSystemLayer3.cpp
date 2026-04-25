// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/Amiga/FileSystem.h"
#include "utl/support.h"

namespace retro::vault::amiga {

void
FileSystem::cd(BlockNr nr)
{
    current = nr;
}

optional<BlockNr>
FileSystem::trySeek(const FSPath &path) const
{
    try {

        BlockNr current = path.absolute() ? root() : pwd();

        for (const auto &p : path) {

            // Check for special tokens
            if (p == "." ) { continue; }
            if (p == "..") { current = fetch(current).getParentDirRef(); continue; }

            auto next = searchdir(current, p);
            if (!next) return { };

            current = *next;
        }
        return current;

    } catch (...) { return { }; }
}

BlockNr
FileSystem::seek(const FSPath &path) const
{
    if (auto it = trySeek(path)) return *it;
    throw FSError(FSError::FS_NOT_FOUND, path.cpp_str());
}

vector<BlockNr>
FileSystem::match(BlockNr top, const vector<FSPattern> &patterns)
{
    vector<BlockNr> currentSet { top };

    for (const auto &pattern : patterns) {

        vector<BlockNr> nextSet;

        // No-ops
        if (pattern.glob == "" || pattern.glob == ".") {
            continue;
        }

        // Root traversal
        if (pattern.glob == ":" || pattern.glob == "/") {
            currentSet = { root() };
            continue;
        }

        // Parent traversal
        if (pattern.glob == "..") {
            for (auto blk : currentSet) {
                nextSet.push_back(fetch(blk).getParentDirRef());
            }
            currentSet = std::move(nextSet);
            continue;
        }

        // Pattern-based lookup
        for (auto blk : currentSet) {

            printf("  Seeking '%s' in '%s'\n",
                   pattern.glob.c_str(),
                   fetch(blk).absName().c_str());

            auto matches = searchdir(blk, pattern);
            for (auto m : matches) {
                printf("    Found %ld (%s)\n", m, fetch(m).absName().c_str());
                nextSet.push_back(m);
            }
        }

        if (nextSet.empty()) {
            printf("No matches for '%s'\n", pattern.glob.c_str());
            return {};
        }

        currentSet = std::move(nextSet);
    }

    return currentSet;
}

vector<BlockNr>
FileSystem::match(const string &path)
{
    return match(pwd(), FSPattern(path).splitted());
}

FSTree
FileSystem::build(BlockNr root, const FSTreeBuildOptions &opt) const
{
    return FSTreeBuilder::build(fetch(root), opt); 
}

}
