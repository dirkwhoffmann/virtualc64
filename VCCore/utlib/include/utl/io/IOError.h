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

/** @brief  Exception thrown if a file or folder cannot be accessed
 *
 *  The error code (see fault()) is one of the constants below. The message
 *  returned by what() names the affected file or folder.
 */
struct IOError : public Error {

    static constexpr long DIR_NOT_FOUND         = 0; ///< The folder does not exist
    static constexpr long DIR_ACCESS_DENIED     = 1; ///< Access to the folder was denied
    static constexpr long DIR_CANT_CREATE       = 2; ///< The folder could not be created
    static constexpr long DIR_CANT_DELETE       = 3; ///< The folder could not be deleted
    static constexpr long DIR_NOT_EMPTY         = 4; ///< The folder is not empty

    static constexpr long FILE_NOT_FOUND        = 10; ///< The file does not exist
    static constexpr long FILE_EXISTS           = 11; ///< The file exists already
    static constexpr long FILE_IS_DIRECTORY     = 12; ///< The path refers to a folder, not a file
    static constexpr long FILE_ACCESS_DENIED    = 13; ///< Access to the file was denied
    static constexpr long FILE_TYPE_MISMATCH    = 14; ///< The file has the wrong type
    static constexpr long FILE_TYPE_UNSUPPORTED = 15; ///< The file type is not supported
    static constexpr long FILE_CANT_READ        = 16; ///< The file could not be read
    static constexpr long FILE_CANT_WRITE       = 17; ///< The file could not be written
    static constexpr long FILE_CANT_CREATE      = 18; ///< The file could not be created
    static constexpr long FILE_CANT_DELETE      = 19; ///< The file could not be deleted
    static constexpr long FILE_INVALID_DATA     = 20; ///< The file contains invalid data
    static constexpr long FILE_IN_USE           = 21; ///< The file is in use

    static constexpr long JSON_ERROR            = 30; ///< A JSON document could not be processed
    static constexpr long ZLIB_ERROR            = 31; ///< Compression or decompression failed

    /// Returns the name of the error code
    const char *errstr() const noexcept override {

        switch (payload) {
                
            case DIR_NOT_FOUND:         return "DIR_NOT_FOUND";
            case DIR_ACCESS_DENIED:     return "DIR_ACCESS_DENIED";
            case DIR_CANT_CREATE:       return "DIR_CANT_CREATE";
            case DIR_CANT_DELETE:       return "DIR_CANT_DELETE";
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
            case FILE_CANT_DELETE:      return "FILE_CANT_DELETE";
            case FILE_INVALID_DATA:     return "FILE_INVALID_DATA";
            case FILE_IN_USE:           return "FILE_IN_USE";

            case JSON_ERROR:            return "JSON_ERROR";
            case ZLIB_ERROR:            return "ZLIB_ERROR";

            default:
                return "UNKNOWN";
        }
    }

    /// Creates an error with an error code and the name of the affected file or folder
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

            case DIR_CANT_DELETE:
              set_msg("Failed to delete folder \"" + msg + "\".");
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

            case FILE_CANT_DELETE:
              set_msg("Failed to delete file \"" + msg + "\".");
              break;

            case FILE_INVALID_DATA:
                set_msg("File \"" + msg + "\" contains invalid data.");
                break;

            case FILE_IN_USE:
                set_msg("File \"" + msg + "\" is in use.");
                break;

            case JSON_ERROR:
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
