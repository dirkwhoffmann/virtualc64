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
 * The class presents an SVM as a plain directory tree, rooted at root(), and
 * treats the storage format underneath as an implementation detail. Everything
 * inside is addressed relative to that root, so callers never need to know
 * which of the two formats they are working with:
 *
 *   ZipFile   The standard format: the machine is a single .svm archive with a
 *             small footprint. root() is a temporary directory, unpacked from
 *             the archive when it is first asked for and packed back up by
 *             persist(). Nothing is unpacked for a caller that only reads the
 *             manifest, which is what keeps listing a library of machines cheap.
 *
 *   Folder    An ordinary directory, useful for manual editing. It is already a
 *             tree, so it *is* its own root: there is no copy, opening it costs
 *             nothing, and persist() writes the manifest in place without
 *             packing anything.
 *
 * Both carry the .svm suffix.
 */

// An SVM is a .svm ZIP archive or a .svm directory -- the suffix is the same
enum class SVMType { ZipFile, Folder };

class SVMFile : public AnyImage {

  public:

    static constexpr auto workspaceDir = "workspace";
    static constexpr auto snapshotDir = "snapshots";

  private:

    // Storage format
    SVMType svmType = SVMType::ZipFile;

    // Meta information about the virtual machine
    Manifest manifest;

    // The SVM's file tree, as an ordinary directory
    fs::path rootFolder;

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
    SVMFile(CreateTag, const fs::path &path, SVMType type = SVMType::ZipFile);

    // Creates an object from an existing file
    SVMFile(OpenTag, const fs::path &path);

    // Creates an object from a copy of an existing file
    SVMFile(CloneTag, const fs::path &path, const fs::path &clone, SVMType type = SVMType::ZipFile);

    // Default constructor (defaults to Open)
    SVMFile(const fs::path &path) : SVMFile(Open, path) { }

    SVMFile(const SVMFile&) = delete;
    SVMFile& operator=(const SVMFile&) = delete;

    ~SVMFile();

  private:

    void init(CreateTag, const fs::path &path, SVMType type = SVMType::ZipFile);
    void init(OpenTag, const fs::path &path);
    void init(CloneTag, const fs::path &path, const fs::path &clonePath, SVMType type = SVMType::ZipFile);


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

    // Returns the path of the SVM archive on disk
    const fs::path &getSourcePath() const { return path; }

    // Returns how this machine is stored (ZIP archive or plain folder)
    SVMType getSVMType() const { return svmType; }

    // Returns the root of the SVM's file tree, materialising it on demand
    const fs::path &root();

    // Writes the working folder back to the SVM file
    void persist();

    // Loads the manifest, either from the file tree or the backing storage
    void readManifest();

    // True if another process has updated the manifest in the file tree
    bool isOutdated() const;

private:

    // Materialises the root folder
    void createRoot();

    // Reconciles the manifest with the contents of the root folder
    void tidyUp();

    // Unpacks the source archive into the root folder (ZIP-backed only)
    void unpackArchive();

    // Packs the root folder back into the source archive (ZIP-backed only)
    void packArchive();
};

}
