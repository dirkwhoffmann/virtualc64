// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Reflectable.h"

namespace retro::vault {

using namespace utl;

using FSFault = long;

struct FSError : public Error {

    static constexpr long FS_OK                     = 0;
    static constexpr long FS_CUSTOM                 = 1;

    // General
    static constexpr long FS_INVALID_ARGUMENT       = 10;
    static constexpr long FS_INVALID_PATH           = 11;
    static constexpr long FS_EXISTS                 = 12;
    static constexpr long FS_NOT_A_DIRECTORY        = 13;
    static constexpr long FS_NOT_A_FILE             = 14;
    static constexpr long FS_NOT_FOUND              = 15;
    static constexpr long FS_NOT_EMPTY              = 16;
    static constexpr long FS_UNFORMATTED            = 17;
    static constexpr long FS_UNSUPPORTED            = 18;
    
    // IO
    static constexpr long FS_BUSY                   = 20;
    static constexpr long FS_READ_ONLY              = 21;
    static constexpr long FS_READ_ERROR             = 22;
    static constexpr long FS_WRITE_ERROR            = 23;
    static constexpr long FS_OUT_OF_SPACE           = 24;

    // Integrity
    static constexpr long FS_OUT_OF_RANGE           = 30;
    static constexpr long FS_CORRUPTED              = 31;
    static constexpr long FS_WRONG_BSIZE            = 32;
    static constexpr long FS_WRONG_CAPACITY         = 33;
    static constexpr long FS_WRONG_DOS_TYPE         = 34;
    static constexpr long FS_WRONG_BLOCK_TYPE       = 35;

    // Posix layer
    static constexpr long FS_INVALID_HANDLE         = 40;
    static constexpr long FS_PERMISSION_DENIED      = 41;
    
    // Export
    static constexpr long FS_EXPORT_ERROR           = 50;

    const char *errstr() const noexcept override {
        
        switch (payload) {
                
            case FS_OK:                         return "FS_OK";
            case FS_CUSTOM:                     return "FS_CUSTOM";
                
            case FS_INVALID_ARGUMENT:           return "FS_INVALID_ARG";
            case FS_INVALID_PATH:               return "FS_INVALID_PATH";
            case FS_EXISTS:                     return "FS_EXISTS";
            case FS_NOT_A_DIRECTORY:            return "FS_NOT_A_DIRECTORY";
            case FS_NOT_A_FILE:                 return "FS_NOT_A_FILE";
            case FS_NOT_FOUND:                  return "FS_NOT_FOUND";
            case FS_NOT_EMPTY:                  return "FS_NOT_EMPTY";
            case FS_UNFORMATTED:                return "FS_UNFORMATTED";
            case FS_UNSUPPORTED:                return "FS_UNSUPPORTED";
                
            case FS_BUSY:                       return "FS_BUSY";
            case FS_READ_ONLY:                  return "FS_READ_ONLY";
            case FS_READ_ERROR:                 return "FS_READ_ERROR";
            case FS_WRITE_ERROR:                return "FS_WRITE_ERROR";
            case FS_OUT_OF_SPACE:               return "FS_OUT_OF_SPACE";

            case FS_OUT_OF_RANGE:               return "FS_OUT_OF_RANGE";
            case FS_CORRUPTED:                  return "FS_CORRUPTED";
            case FS_WRONG_BSIZE:                return "FS_WRONG_BSIZE";
            case FS_WRONG_CAPACITY:             return "FS_WRONG_CAPACITY";
            case FS_WRONG_DOS_TYPE:             return "FS_WRONG_DOS_TYPE";
            case FS_WRONG_BLOCK_TYPE:           return "FS_WRONG_BLOCK_TYPE";
                
            case FS_INVALID_HANDLE:             return "FS_INVALID_HANDLE";
            case FS_PERMISSION_DENIED:          return "FS_PERMISSION_DENIED";
                
            case FS_EXPORT_ERROR:               return "FS_EXPORT_ERROR";
                
            default:
                return "UNKNOWN_FAULT";
        }
    }
    
    int posixErrno() const noexcept {
        
        switch (payload) {
                
            case FS_OK:                         return 0;
            case FS_CUSTOM:                     return EIO;
                
            case FS_INVALID_ARGUMENT:           return EINVAL;
            case FS_INVALID_PATH:               return EINVAL;
            case FS_EXISTS:                     return EEXIST;
            case FS_NOT_A_DIRECTORY:            return ENOTDIR;
            case FS_NOT_A_FILE:                 return EISDIR;
            case FS_NOT_FOUND:                  return ENOENT;
            case FS_NOT_EMPTY:                  return ENOTEMPTY;
            case FS_UNFORMATTED:                return EIO;
            case FS_UNSUPPORTED:                return EINVAL;

            case FS_BUSY:                       return EBUSY;
            case FS_READ_ONLY:                  return EROFS;
            case FS_READ_ERROR:                 return EIO;
            case FS_WRITE_ERROR:                return EIO;
            case FS_OUT_OF_SPACE:               return ENOSPC;

            case FS_OUT_OF_RANGE:               return EINVAL;
            case FS_CORRUPTED:                  return EIO;
            case FS_WRONG_BSIZE:                return EINVAL;
            case FS_WRONG_CAPACITY:             return EINVAL;
            case FS_WRONG_DOS_TYPE:             return EINVAL;
            case FS_WRONG_BLOCK_TYPE:           return EINVAL;
                
            case FS_INVALID_HANDLE:             return EINVAL;
            case FS_PERMISSION_DENIED:          return EACCES;

            case FS_EXPORT_ERROR:               return EIO;
                
            default:
                return EIO;
        }
    }

    explicit FSError(long fault, const std::string &msg = "");
    explicit FSError(long fault, const char *str) : FSError(fault, string(str)) { }
    explicit FSError(long fault, const fs::path &path) : FSError(fault, path.string()) { }
};

}
