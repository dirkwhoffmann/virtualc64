// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "SVMFile.h"
#include "Manifest.h"
#include "Images/ImageTypes.h"
#include "Images/ImageError.h"
#include "utl/chrono.h"
#include "utl/io.h"
#include "utl/support.h"
#include <format>
#include <set>

namespace retro::vault {

    namespace fs = std::filesystem;
    using utl::IOError;

    /* An SVM is identified by its suffix alone: a directory named *.svm.
     * This never has to look inside and never has to stat.
     *
     * A directory carrying an extension is valid on every platform we build
     * for: it is the macOS bundle convention (.app, .rtfd), and Win32 forbids
     * only a *trailing* period, not an interior one.
     *
     * As with any other format here, this classifies without validating. A
     * '.svm' holding no manifest is reported as an SVM and then fails to open
     * with VM_NO_MANIFEST, which says more than "unknown file type" would.
     */
    optional<ImageInfo>
    SVMFile::about(const fs::path &path)
    {
        if (utl::uppercased(path.extension().string()) == ".SVM") {
            return {{ ImageType::VM, ImageFormat::SVM }};
        }

        return {};
    }

    std::vector<string>
    SVMFile::describeImage() const noexcept
    {
        return {
            manifest.name.empty() ? "Virtual Machine" : manifest.name,
            PlatformEnum::key(manifest.platform),
            std::format("{} Snapshot{}", manifest.numSnapshots(),
                        manifest.numSnapshots() != 1 ? "s" : "")
        };
    }

    u64
    SVMFile::hash(HashAlgorithm algorithm) const
    {
        const auto id = manifest.uuid.toString() + "@" + std::to_string(manifest.generation);
        return Hashable::hash((const u8 *)id.data(), isize(id.size()), algorithm);
    }

    SVMFile::SVMFile(CreateTag, const fs::path &path) {
        init(Create, path);
    }

    SVMFile::SVMFile(OpenTag, const fs::path &path) {
        init(Open, path);
    }

    SVMFile::SVMFile(CloneTag, const fs::path &path, const fs::path &clone) {
        init(Clone, path, clone);
    }

    void
    SVMFile::init(SVMFile::CreateTag, const fs::path &path) {

        // The suffix is what every later reader classifies this machine by,
        // so it is not the caller's to get wrong
        this->path = utl::ensureExtension(path, suffix);

        std::error_code ec;

        // If the item exists, delete it before proceeding
        utl::remove(this->path);

        // Create empty directory
        fs::create_directories(this->path, ec);
        if (ec) throw utl::IOError(utl::IOError::DIR_CANT_CREATE, this->path);

        /* Ask Finder to show the tree as a single file. The exported UTI in
         * Silicium's Info.plist says the same thing, but only to Macs where
         * Silicium is installed; the flag is carried by the folder itself and
         * survives the copy to a machine that has never seen it.
         */
        utl::setPackageBit(this->path);

        // Prepare the manifest
        manifest.uuid = utl::UUID::v4();
        manifest.created = std::time(nullptr);
        manifest.modified = manifest.created;

        // Save changes
        persist();
    }

    void
    SVMFile::init(SVMFile::OpenTag, const fs::path &path) {
        this->path = path;

        std::error_code ec;

        if (!fs::exists(path)) {
            throw ImageError(ImageError::VM_NOT_FOUND);
        }

        auto status = fs::status(path, ec);
        if (ec) throw ImageError(ImageError::VM_CANT_OPEN);

        const auto perms = status.permissions();

        const bool readable = (perms & fs::perms::owner_read) != fs::perms::none ||
                              (perms & fs::perms::group_read) != fs::perms::none ||
                              (perms & fs::perms::others_read) != fs::perms::none;

        if (!readable) {
            throw ImageError(ImageError::VM_CANT_OPEN);
        }

        /* An SVM is a directory, and only a directory.
         *
         * about() classifies by the suffix alone, so anything at all can
         * arrive here wearing a '.svm'. The one case worth naming is a ZIP
         * archive: that was a supported backing until the format was reduced
         * to folders, and a machine left over from then is a file, not a
         * tree. Every path below assumes it can walk the root, so this is
         * caught once, here, rather than as a puzzling failure later.
         */
        if (!fs::is_directory(path, ec) || ec) {
            throw ImageError(ImageError::VM_CORRUPTED, "not a folder");
        }

        readManifest();
    }

    void
    SVMFile::init(CloneTag, const fs::path &path, const fs::path &clonePath) {
        // Open the source SVM
        SVMFile src(path);

        // Create this SVM. init(Create) settles the clone's suffix, so
        // getSourcePath() -- not clonePath -- is the name it ends up under.
        init(Create, clonePath);

        // Copy the source machine's tree into ours
        fs::copy(src.root(), root(),
                 fs::copy_options::recursive | fs::copy_options::overwrite_existing);

        // Update the manifest
        manifest.load(root() / "manifest.json");

        // Assign a new UUID to distinguish the clone from the source
        manifest.uuid = utl::UUID::v4();

        // Drop any metadata and make the clone writable
        manifest.meta.reset();
        manifest.readOnly = false;

        // Write changes back to the SVM
        persist();
    }

    void
    SVMFile::persist()
    {
        // Only proceed if we have write permission
        if (isReadOnly()) throw ImageError(ImageError::VM_READ_ONLY);

        // Make sure that a consistent machine is written
        tidyUp();

        // Update the modification date and the generation counter
        manifest.modified = time(nullptr);
        manifest.generation++;

        /* Save the manifest. The rest of the tree is already where it
         * belongs -- callers write their assets straight into root() -- so
         * this is the whole of it.
         */
        manifest.save(root() / "manifest.json");
    }

    void
    SVMFile::readManifest()
    {
        manifest = Manifest(root() / "manifest.json");
    }

    bool
    SVMFile::isOutdated() const
    {
        try {
            return Manifest(root() / "manifest.json").generation > manifest.generation;
        } catch (const std::exception &) {
            return false;
        }
    }

    void
    SVMFile::tidyUp()
    {
        std::error_code ec;
        const auto workspace = root() / workspaceDir;
        const auto snapshots = root() / snapshotDir;

        auto present = [&](const fs::path &base, const fs::path &file) {

            if (file.empty() || file.is_absolute()) return false;

            std::error_code ignore;
            return fs::is_regular_file(base / file, ignore);
        };

        // Remove snapshots with a missing binary
        std::erase_if(manifest.snapshots.get(), [&](const SnapshotInfo &info) {

            if (present(snapshots, info.binary)) return false;

            logmsg(LOG_WARN, "Dropping snapshot %s: '%s' is missing.\n",
                     info.uuid.toString().c_str(), info.binary.string().c_str());
            return true;
        });

        // Check for snapshots with a missing screenshot
        for (auto &info: manifest.snapshots.get()) {

            if (!info.screenshot.empty() && !present(snapshots, info.screenshot)) {

                logmsg(LOG_WARN, "Snapshot %s: dropping missing screenshot '%s'.\n",
                        info.uuid.toString().c_str(), info.screenshot.string().c_str());
                info.screenshot.clear();
            }
        }

        // The VM's own screenshot depicts the workspace and is filed with it
        if (!manifest.screenshot.empty() && !present(workspace, manifest.screenshot)) {

            logmsg(LOG_WARN, "Dropping missing screenshot '%s'.\n", manifest.screenshot.string().c_str());
            manifest.screenshot.clear();
        }

        // Check for a missing startup script
        if (!manifest.startup.empty() && !present(workspace, manifest.startup)) {

            logmsg(LOG_WARN, "Dropping missing startup script '%s'.\n", manifest.startup.string().c_str());
            manifest.startup.clear();
        }

        // Remove orphaned snapshot binaries
        if (!fs::is_directory(snapshots, ec)) return;

        std::set<fs::path> claimed;
        for (const auto &info: manifest.snapshots.get()) {

            claimed.insert(info.binary);
            if (!info.screenshot.empty()) claimed.insert(info.screenshot);
        }

        std::vector<fs::path> orphans;
        for (const auto &entry: fs::recursive_directory_iterator(snapshots, ec)) {

            if (!entry.is_regular_file()) continue;
            if (claimed.contains(entry.path().lexically_relative(snapshots))) continue;

            orphans.push_back(entry.path());
        }

        for (const auto &orphan: orphans) {

            logmsg(LOG_WARN, "Deleting unreferenced snapshot file '%s'.\n",
                    orphan.filename().string().c_str());
            fs::remove(orphan, ec);
        }
    }
}
