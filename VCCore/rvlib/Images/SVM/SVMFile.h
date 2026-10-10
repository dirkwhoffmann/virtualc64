// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Images/AnyImage.h"
#include "Manifest.h"
#include <utl/io.h>

namespace retro::vault {

/* This class represents a Silicium Virtual Machine file. Such a file holds all
 * assets of a virtual machine, including configuration files, ROMs, media
 * images, and snapshots. A central component is the manifest, a JSON document
 * describing the other files as well as the machine itself.
 *
 * Structure of an SVM:
 *
 *  SVM
 *   |- manifest.json
 *   |- workspace (folder)
 *   |- snapshots (folder)
 *
 * An SVM is a directory named *.svm, and nothing else -- it is its own root().
 * Opening one costs nothing, persist() writes the manifest in place, and the
 * tree on disk is at all times the machine: there is no second copy of it to
 * unpack, repack, or lose in a crash.
 *
 * Earlier versions also stored a machine as a ZIP archive, so that an SVM
 * would be a single file rather than a folder. Finder does that part on its
 * own now -- Silicium's Info.plist declares the .svm type as a package, and
 * utl::setPackageBit() marks the folder for Macs where it is not installed --
 * and the archive cost more than it was worth: root() had to unpack the whole
 * machine before a caller could read one file from it, and persist() repacked
 * all of it on every save. With a hard drive attached, each snapshot rewrote
 * the entire disk image.
 */

class SVMFile : public AnyImage {

  public:

    static constexpr auto workspaceDir = "workspace";
    static constexpr auto snapshotDir = "snapshots";

    // The suffix an SVM is named by
    static constexpr auto suffix = ".svm";

  private:

    // Meta information about the virtual machine
    Manifest manifest;

  public:

    struct OpenTag {};
    static constexpr OpenTag Open {};

    struct CreateTag {};
    static constexpr CreateTag Create {};

    struct CloneTag {};
    static constexpr CloneTag Clone {};

    // Analyzes the type of the provided file
    static optional<ImageInfo> about(const fs::path &path);

    // Creates an object from scratch
    SVMFile(CreateTag, const fs::path &path);

    // Creates an object from an existing file
    SVMFile(OpenTag, const fs::path &path);

    // Creates an object from a copy of an existing file
    SVMFile(CloneTag, const fs::path &path, const fs::path &clone);

    // Default constructor (defaults to Open)
    SVMFile(const fs::path &path) : SVMFile(Open, path) { }

    SVMFile(const SVMFile&) = delete;
    SVMFile& operator=(const SVMFile&) = delete;

    /* No destructor. The tree on disk is the machine, not a working copy of
     * it, so there is nothing to write back and nothing to clean up.
     */

  private:

    void init(CreateTag, const fs::path &path);
    void init(OpenTag, const fs::path &path);
    void init(CloneTag, const fs::path &path, const fs::path &clonePath);


    //
    // Methods from AnyImage
    //

  public:

    bool validateURL(const fs::path &path) const noexcept override {
        return about(path).has_value();
    }

    ImageType type() const noexcept override { return ImageType::VM; }
    ImageFormat format() const noexcept override { return ImageFormat::SVM; }
    std::vector<string> describeImage() const noexcept override;
    void save() override { persist(); }


    //
    // Methods from Hashable
    //

  public:

    /* An SVM has no bytes in memory to hash, so its identity stands in: the
     * UUID says which machine, the generation which revision of it.
     */
    u64 hash(HashAlgorithm algorithm) const override;


    //
    // Accessing the manifest
    //

  public:

    Manifest &getManifest() { return manifest; }
    const Manifest &getManifest() const { return manifest; }

    bool isReadOnly() const { return manifest.isReadOnly(); }


    //
    // Accessing assets
    //

  public:

    // Returns the path of the SVM on disk
    const fs::path &getSourcePath() const { return path; }

    // Returns the root of the SVM's file tree, which is the SVM itself
    const fs::path &root() const { return path; }

    // Writes the manifest back to the file tree
    void persist();

    // Loads the manifest from the file tree
    void readManifest();

    // True if another process has updated the manifest in the file tree
    bool isOutdated() const;

private:

    // Reconciles the manifest with the contents of the root folder
    void tidyUp();
};

}
