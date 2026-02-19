// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/types/Exception.h"

namespace utl {

struct IOError : public Error {

    static constexpr long DIR_NOT_FOUND         = 0;
    static constexpr long DIR_ACCESS_DENIED     = 1;
    static constexpr long DIR_CANT_CREATE       = 2;
    static constexpr long DIR_NOT_EMPTY         = 3;

    static constexpr long FILE_NOT_FOUND        = 4;
    static constexpr long FILE_EXISTS           = 5;
    static constexpr long FILE_IS_DIRECTORY     = 6;
    static constexpr long FILE_ACCESS_DENIED    = 7;
    static constexpr long FILE_TYPE_MISMATCH    = 8;
    static constexpr long FILE_TYPE_UNSUPPORTED = 9;
    static constexpr long FILE_CANT_READ        = 10;
    static constexpr long FILE_CANT_WRITE       = 11;
    static constexpr long FILE_CANT_CREATE      = 12;

    static constexpr long ZLIB_ERROR            = 20;

    const char *errstr() const noexcept override {

        switch (payload) {
                
            case DIR_NOT_FOUND:         return "DIR_NOT_FOUND";
            case DIR_ACCESS_DENIED:     return "DIR_ACCESS_DENIED";
            case DIR_CANT_CREATE:       return "DIR_CANT_CREATE";
            case DIR_NOT_EMPTY:         return "DIR_NOT_EMPTY";
                
            case FILE_NOT_FOUND:        return "FILE_NOT_FOUND";
            case FILE_EXISTS:           return "FILE_EXISTS";
            case FILE_IS_DIRECTORY:     return "FILE_IS_DIRECTORY";
            case FILE_ACCESS_DENIED:    return "FILE_ACCESS_DENIED";
            case FILE_TYPE_MISMATCH:    return "FILE_TYPE_MISMATCH";
            case FILE_TYPE_UNSUPPORTED: return "FILE_TYPE_UNSUPPORTED";
            case FILE_CANT_READ:        return "FILE_CANT_READ";
            case FILE_CANT_WRITE:       return "FILE_CANT_WRITE";
            case FILE_CANT_CREATE:      return "FILE_CANT_CREATE";
                
            case ZLIB_ERROR:            return "ZLIB_ERROR";
                
            default:
                return "UNKNOWN";
        }
    }

    explicit IOError(long fault, const std::string &msg = "") : Error(fault) {

        switch (fault) {

            case DIR_NOT_FOUND:
                set_msg("Folder \"" + msg + "\" not found.");
                break;

            case DIR_ACCESS_DENIED:
                set_msg("Unable to access folder \"" + msg + "\". Permission denied.");
                break;

            case DIR_CANT_CREATE:
                set_msg("Failed to create folder \"" + msg + "\".");
                break;

            case DIR_NOT_EMPTY:
                set_msg("Folder \"" + msg + "\" is not empty.");
                break;

            case FILE_NOT_FOUND:
                set_msg("File \"" + msg + "\" not found.");
                break;

            case FILE_EXISTS:
                set_msg("File \"" + msg + "\" exists.");
                break;

            case FILE_IS_DIRECTORY:
                set_msg("File \"" + msg + "\" is a directory.");
                break;

            case FILE_ACCESS_DENIED:
                set_msg("Unable to access file \"" + msg + "\". Permission denied.");
                break;

            case FILE_TYPE_MISMATCH:
                set_msg("File \"" + msg + "\" does not match its type.");
                break;

            case FILE_TYPE_UNSUPPORTED:
                set_msg(msg.empty() ?
                        "Unsupported file type." :
                        "Unsupported file type: \"" + msg + "\".");
                break;

            case FILE_CANT_READ:
                set_msg("Failed to read from file \"" + msg + "\".");
                break;

            case FILE_CANT_WRITE:
                set_msg("Failed to write to file \"" + msg + "\".");
                break;

            case FILE_CANT_CREATE:
                set_msg("Failed to create file \"" + msg + "\".");
                break;

            case ZLIB_ERROR:
                set_msg(msg);
                break;

            default:
                set_msg("IOError " + std::to_string(fault) + " (" + errstr() + ")");
        }
    }

    explicit IOError(long fault, const char *str) : IOError(fault, string(str)) { }
    explicit IOError(long fault, const fs::path &path) : IOError(fault, path.string()) { }
};

}
