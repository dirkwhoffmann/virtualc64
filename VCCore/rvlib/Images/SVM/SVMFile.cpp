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
#include "utl/storage/ZipArchive.h"
#include <format>
#include <set>

namespace retro::vault {

    namespace fs = std::filesystem;
    using utl::IOError;

    /* Both backings are identified the same way: by the suffix. A folder-backed
     * SVM is a directory named *.svm -- the preinstalled showcases are exactly
     * that -- so this never has to look inside and never has to stat.
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

    SVMFile::SVMFile(CreateTag, const fs::path &path, SVMType type) {
        init(Create, path, type);
    }

    SVMFile::SVMFile(OpenTag, const fs::path &path) {
        init(Open, path);
    }

    SVMFile::SVMFile(CloneTag, const fs::path &path, const fs::path &clone, SVMType type) {
        init(Clone, path, clone, type);
    }

    SVMFile::~SVMFile() {
        // Wipe out anything we've created inside the tmp folder

        std::error_code ec;
        const auto tmp = fs::temp_directory_path(ec);

        if (!ec && fs::equivalent(rootFolder.parent_path(), tmp, ec) && !ec)
            fs::remove_all(rootFolder, ec);
    }

    void
    SVMFile::init(SVMFile::CreateTag, const fs::path &path, SVMType type) {
        this->path = path;
        this->svmType = type;

        std::error_code ec;

        // If the item exists, delete it before proceeding
        utl::remove(path);

        // See if we can create the source item
        if (type == SVMType::Folder) {

            // Create empty directory
            fs::create_directories(path, ec);
            if (ec) throw utl::IOError(utl::IOError::DIR_CANT_CREATE, path);

        } else {

            // Create empty ZIP archive
            printf("Creating empty archive %s\n", path.string().c_str());
            utl::ZipArchive archive(path, 'w');
        }

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

        svmType = fs::is_directory(path, ec) ? SVMType::Folder : SVMType::ZipFile;
        if (ec) throw ImageError(ImageError::VM_CANT_OPEN);

        readManifest();
    }

    void
    SVMFile::init(CloneTag, const fs::path &path, const fs::path &clonePath, SVMType type) {
        // Open the source SVM
        SVMFile src(path);

        // Create this SVM
        init(Create, clonePath, type);

        // Copy the source machine's tree into ours
        fs::copy(src.root(), root(),
                 fs::copy_options::recursive | fs::copy_options::overwrite_existing);

        // Update the manifest
        manifest.load(root() / "manifest.json");

        // Assign a new UUID to distinguish the clone from the source
        manifest.uuid = utl::UUID::v4();

        // Drop any metadata and make the archive writable
        manifest.meta.reset();
        manifest.readOnly = false;

        // Write changes back to the SVM
        persist();
    }

    const fs::path &
    SVMFile::root()
    {
        if (rootFolder.empty()) createRoot();
        return rootFolder;
    }

    void
    SVMFile::persist()
    {
        // Only proceed if we have write permission
        if (isReadOnly()) throw ImageError(ImageError::VM_READ_ONLY);

        // Make sure that a consitent archive is written
        tidyUp();

        // Update the modification date and the generation counter
        manifest.modified = time(nullptr);
        manifest.generation++;

        // Save the manifest
        manifest.save(root() / "manifest.json");

        // Create the compressed archive if the SVM is ZIP-backed
        if (svmType == SVMType::ZipFile) packArchive();
    }

    void
    SVMFile::readManifest()
    {
        // If a live-tree exists, it is the source of truth
        if (!rootFolder.empty()) {

            manifest = Manifest(rootFolder / "manifest.json");
            return;
        }

        // For folder-backed SMVs, read the manifest from the source folder
        if (svmType == SVMType::Folder) {

            manifest = Manifest(path / "manifest.json");
            return;
        }

        // For ZIP-backed SMVs, extract the manifest from the source archive
        utl::ZipArchive archive(path);
        for (auto &item: archive.listFiles()) {

            if (item != "manifest.json") continue;

            manifest = Manifest(archive.uncompress(item));
            return;
        }

        throw ImageError(ImageError::VM_NO_MANIFEST);
    }

    bool
    SVMFile::isOutdated() const
    {
        try {
            return Manifest(rootFolder / "manifest.json").generation > manifest.generation;
        } catch (const std::exception &) {
            return false;
        }
    }

    void
    SVMFile::createRoot()
    {
        // A folder-backed SVM is a tree on disk; it becomes its own root
        if (svmType == SVMType::Folder) {

            rootFolder = path;
        }

        // For a ZIP-backed SVM, create the root in the temp space
        if (svmType == SVMType::ZipFile) {

            rootFolder = utl::makeUniquePath(fs::temp_directory_path() / "svm");
            fs::create_directories(rootFolder);
            unpackArchive();
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

    void
    SVMFile::unpackArchive()
    {
        utl::ZipArchive archive(path);
        archive.uncompressAll(rootFolder);
    }

    void
    SVMFile::packArchive()
    {
        auto files = utl::files(root());

        utl::ZipArchive archive(path, 'w');
        archive.write(files, root());
    }
}
