// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"

namespace retro::vault {

struct ImageError : public utl::Error {

    static constexpr long OK                =  0;
    static constexpr long CUSTOM            =  1;

    // General
    static constexpr long SIZE_MISMATCH     = 10;
    static constexpr long FORMAT_MISMATCH   = 11;

    // DMS images
    static constexpr long DMS_CANT_CREATE   = 20;

    // Extended ADF images
    static constexpr long EXT_FACTOR5       = 30;
    static constexpr long EXT_INCOMPATIBLE  = 31;
    static constexpr long EXT_CORRUPTED     = 32;

    // Virtual machines (SVM files)
    static constexpr long VM_NOT_FOUND         = 100;
    static constexpr long VM_CANT_OPEN         = 101;
    static constexpr long VM_CANT_RUN          = 102;
    static constexpr long VM_EXISTS            = 103;
    static constexpr long VM_RUNNING           = 104;
    static constexpr long VM_NO_MANIFEST       = 105;
    static constexpr long VM_CORRUPTED         = 106;
    static constexpr long VM_SAME_UUID         = 107;
    static constexpr long VM_READ_ONLY         = 108;
    static constexpr long NO_CORE              = 109;
    static constexpr long NO_SVM               = 110;
    static constexpr long NO_WORKING_FOLDER    = 111;
    static constexpr long NO_WORKSPACE_FOLDER  = 112;
    static constexpr long NO_SNAPSHOT_FOLDER   = 113;
    static constexpr long EXECUTABLE_NOT_FOUND = 114;

    const char *errstr() const noexcept override {

        switch (payload) {

            case OK:                          return "OK";
            case CUSTOM:                      return "CUSTOM";

            case SIZE_MISMATCH:               return "SIZE_MISMATCH";
            case FORMAT_MISMATCH:             return "FORMAT_MISMATCH";

            case DMS_CANT_CREATE:             return "DMS_CANT_CREATE";
            case EXT_FACTOR5:                 return "EXT_UNSUPPORTED";
            case EXT_INCOMPATIBLE:            return "EXT_INCOMPATIBLE";
            case EXT_CORRUPTED:               return "EXT_CORRUPTED";

            case VM_NOT_FOUND:                return "VM_NOT_FOUND";
            case VM_CANT_OPEN:                return "VM_CANT_OPEN";
            case VM_CANT_RUN:                 return "VM_CANT_RUN";
            case VM_EXISTS:                   return "VM_EXISTS";
            case VM_RUNNING:                  return "VM_RUNNING";
            case VM_NO_MANIFEST:              return "VM_NO_MANIFEST";
            case VM_CORRUPTED:                return "VM_CORRUPTED";
            case VM_SAME_UUID:                return "VM_SAME_UUID";
            case VM_READ_ONLY:                return "VM_READ_ONLY";
            case NO_CORE:                     return "NO_CORE";
            case NO_SVM:                      return "NO_SVM";
            case NO_WORKING_FOLDER:           return "NO_WORKING_FOLDER";
            case NO_WORKSPACE_FOLDER:         return "NO_WORKSPACE_FOLDER";
            case NO_SNAPSHOT_FOLDER:          return "NO_SNAPSHOT_FOLDER";
            case EXECUTABLE_NOT_FOUND:        return "EXECUTABLE_NOT_FOUND";
        }
        return "???";
    }

    explicit ImageError(long fault, const string &s);
    explicit ImageError(long fault, const char *s) : ImageError(fault, string(s)) { };
    explicit ImageError(long fault, const fs::path &p) : ImageError(fault, p.string()) { };
    explicit ImageError(long fault, std::integral auto v) : ImageError(fault, std::to_string(v)) { };
    explicit ImageError(long fault) : ImageError(fault, "") { }
};

}
