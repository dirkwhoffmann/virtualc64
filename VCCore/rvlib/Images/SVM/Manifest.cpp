// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "Manifest.h"
#include "utl/io.h"
#include "json.h"
#include <ctime>
#include <fstream>

namespace retro::vault {

using utl::IOError;

using json = nlohmann::json;

template <typename T>
static inline T
parse(const json &data, const std::string &key)
{
    if (data.contains(key) && !data[key].is_null()) {
        try {
            return data[key].get<T>();
        } catch (const json::exception &) {
            printf("Invalid type for field: %s\n", key.c_str());
            throw utl::IOError(utl::IOError::JSON_ERROR, "Invalid type for field '" + key + "'.");
        }
    }
    throw utl::IOError(utl::IOError::JSON_ERROR, "Missing '" + key + "' field.");
}

template <typename T>
static inline T
parse(const json &data, const std::string &key, const T &fallback)
{
    try {
        return parse<T>(data, key);
    } catch (...) {
        return fallback;
    }
}

template <typename T, typename E>
static inline T
parse(const json &data, const std::string &key)
{
    if (data.contains(key) && !data[key].is_null()) {
        try {
            auto value = data[key].get<std::string>();
            if (auto parsedValue = E::parseEnum(value)) {
                return *parsedValue;
            } else {
                throw utl::IOError(utl::IOError::JSON_ERROR, "Invalid value for '" + key + "' field: " + value);
            }
        } catch (const json::exception &) {
            throw utl::IOError(utl::IOError::JSON_ERROR, "Invalid type for field '" + key + "'.");
        }
    }
    throw utl::IOError(utl::IOError::JSON_ERROR, "Missing '" + key + "' field.");
}

template <typename T, typename E>
static inline T
parse(const json &data, const std::string &key, const T &fallback)
{
    try {
        return parse<T, E>(data, key);
    } catch (...) {
        return fallback;
    }
}

Manifest::Manifest()
{

}

void
Manifest::dump(std::ostream &os) const
{
    os << "Manifest {\n";

    os << "  version          : " << version << "\n";
    os << "  uuid             : " << uuid.toString() << "\n";
    os << "  name             : " << name << "\n";
    os << "  platform         : " << PlatformEnum::key(platform) << "\n";
    os << "  created          : " << created << "\n";
    os << "  modified         : " << modified << "\n";
    os << "  screenshot       : " << (screenshot.empty() ? "<none>" : screenshot.string()) << "\n";
    os << "  startup          : " << (startup.empty() ? "<none>" : startup.string()) << "\n";
    os << "  readOnly         : " << (readOnly ? "true" : "false") << "\n";
    os << "  meta             : ";

    if (!meta) {
        os << "<none>\n";
    } else {
        os << "\n";
        os << "    author         : " << (meta->author.empty() ? "<none>" : meta->author) << "\n";
        os << "    description    : " << (meta->description.empty() ? "<none>" : meta->description) << "\n";
    }
    os << "}\n";

    for (const auto &info : snapshots.get())
        dump(os, info);
}

void
Manifest::dump(std::ostream &os, const SnapshotInfo &info) const
{
    os << "Snapshot {\n";

    os << "    version          : " << info.version << "\n";
    os << "    uuid             : " << info.uuid.toString() << "\n";
    os << "    name             : " << info.name << "\n";
    os << "    platform         : " << PlatformEnum::key(info.platform) << "\n";
    os << "    created          : " << info.created << "\n";
    os << "    modified         : " << info.modified << "\n";
    os << "    screenshot       : " << (info.screenshot.empty() ? "<none>" : info.screenshot.string()) << "\n";
    os << "    binary             : " << (info.binary.empty() ? "<none>" : info.binary.string()) << "\n";

    os << "}\n";
}

void
Manifest::load(fs::path path)
{
    if (fs::is_directory(path)) path /= "manifest.json";

    if (!fs::exists(path)) {
        throw utl::IOError(utl::IOError::FILE_NOT_FOUND, path);
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        throw utl::IOError(utl::IOError::FILE_CANT_READ, path);
    }

    load(file);
}

void
Manifest::load(const char *buf, isize len)
{
    std::stringstream ss;
    ss.write(buf, (size_t)len);
    load(ss);
}

void
Manifest::load(const std::vector<u8> &vec)
{
    load((const char *)vec.data(), (isize)vec.size());
}

void
Manifest::load(std::istream &stream)
{
    auto parseSnapshot = [&](const json &item) -> SnapshotInfo {

        SnapshotInfo snapshot {};
        snapshot.version    = parse<string>(item, "version");
        snapshot.uuid       = parse<string>(item, "uuid");
        snapshot.name       = parse<string>(item, "name", "");
        snapshot.platform   = parse<Platform, PlatformEnum>(item, "platform");
        snapshot.created    = parse<time_t>(item, "created");
        snapshot.modified   = parse<time_t>(item, "modified", snapshot.created);
        snapshot.screenshot = parse<string>(item, "screenshot", "");
        snapshot.binary     = parse<string>(item, "binary", "");

        return snapshot;
    };

    json data;

    // Create JSON object
    try {
        stream >> data;
    } catch (const json::parse_error &e) {
        throw utl::IOError(utl::IOError::JSON_ERROR, e.what());
    }

    // Start from scratch
    *this = Manifest();

    try {

        uuid        = parse<string>(data, "uuid");
        version     = parse<string>(data, "version");
        platform    = parse<Platform, PlatformEnum>(data, "platform", Platform::C64);
        name        = parse<string>(data, "name", "Virtual Machine");
        created     = parse<time_t>(data, "created");
        modified    = parse<time_t>(data, "modified", created);
        generation  = parse<isize>(data, "generation", 0);
        created     = data["created"].get<time_t>();
        screenshot  = parse<string>(data, "screenshot", "");
        startup     = parse<string>(data, "startup", "");
        readOnly    = parse<bool>(data, "readOnly", false);

        /* The presence of the object is what marks this as a showcase, so it
         * is read even when both members are missing. A non-object is ignored
         * rather than treated as an empty showcase -- that way a malformed
         * entry does not silently change what kind of machine this is.
         */
        if (data.contains("meta") && data["meta"].is_object()) {

            const auto &item = data["meta"];

            meta = Meta {
                .author      = parse<string>(item, "author", ""),
                .description = parse<string>(item, "description", "")
            };
        }

        if (data.contains("snapshots") && !data["snapshots"].is_null()) {
            for (const auto &item : data["snapshots"]) {

                SnapshotInfo snapshot {};

                try {

                    snapshot = parseSnapshot(item);

                } catch (const std::exception &e) {

                    printf("Skipping malformed snapshot entry: %s\n", e.what());
                    continue;
                }

                if (snapshot.binary.empty()) {
                    printf("Skipping snapshot %s: no binary.\n", snapshot.uuid.toString().c_str());
                    continue;
                }
                if (snapshot.uuid.isZero()) {
                    printf("Skipping snapshot: nil uuid.\n");
                    continue;
                }
                if (lookupSnapshot(snapshot.uuid)) {
                    printf("Skipping snapshot %s: duplicate uuid.\n", snapshot.uuid.toString().c_str());
                    continue;
                }

                snapshots.insert(snapshot);
            }
        }
    } catch (const std::exception &e) {
        throw utl::IOError(utl::IOError::JSON_ERROR, e.what());
    }
}

void
Manifest::save(fs::path path) const
{
    auto snapshotToJson = [](const SnapshotInfo &snapshot) {
        json data;

        data["uuid"]       = snapshot.uuid.toString();
        data["version"]    = snapshot.version;
        data["platform"]   = PlatformEnum::key(snapshot.platform);
        data["name"]       = snapshot.name;
        data["created"]    = snapshot.created;
        data["modified"]   = snapshot.modified;
        data["binary"]     = snapshot.binary.string();
        data["screenshot"] = snapshot.screenshot.string();

        return data;
    };

    // Add a default file name if a path is provided
    if (fs::is_directory(path)) path /= "manifest.json";

    json data;

    data["uuid"]       = uuid.toString();
    data["version"]    = version;
    data["platform"]   = PlatformEnum::key(platform);
    data["name"]       = name;
    data["created"]    = created;
    data["modified"]   = modified;
    data["generation"] = generation;

    if (!screenshot.empty()) data["screenshot"] = screenshot.string();
    if (!startup.empty()) data["startup"] = startup.string();
    if (readOnly) data["readOnly"] = true;

    // Written whenever it exists, even with both members blank: it is the
    // object's presence that marks a showcase, so dropping an empty one would
    // quietly demote the machine on the next save.
    if (meta) {

        json item = json::object();

        if (!meta->author.empty()) item["author"] = meta->author;
        if (!meta->description.empty()) item["description"] = meta->description;

        data["meta"] = item;
    }

    json array = json::array();
    for (const auto &snapshot : snapshots.get()) {
        array.push_back(snapshotToJson(snapshot));
    }
    if (!array.empty()) {
        data["snapshots"] = array;
    }

    // Write to disk
    try {

        std::ofstream file(path);

        if (!file.is_open()) {
            throw utl::IOError(utl::IOError::FILE_CANT_WRITE, path);
        }

        file << data.dump(4);

    } catch (const std::exception &e) {

        throw utl::IOError(utl::IOError::JSON_ERROR, e.what());
    }
}

}
