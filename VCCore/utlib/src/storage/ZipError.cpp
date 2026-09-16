// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/common.h"
#include "utl/storage/ZipError.h"
#include "zip.h"

namespace utl {

void
ZipError::setError(long fault, const std::string &msg)
{
    switch (fault) {

        /* Kept in step with the inline constructor in ZipError.h, which this
         * duplicates. NOT_A_FILE and NOT_A_DIRECTORY were missing here, so the
         * detail was dropped for anyone passing a string literal (which selects
         * the const char* constructor and lands in this function) while a
         * std::string argument kept it.
         */
        case INVALID_ZIP_MODE: set_msg("Invalid ZIP mode: " + msg); break;
        case INVALID_ARCHIVE:  set_msg("Invalid or unsupported ZIP file: " + msg); break;
        case NOT_A_FILE:       set_msg("Not a file: " + msg); break;
        case NOT_A_DIRECTORY:  set_msg("Not a directory: " + msg); break;
        case KUBA_ZIP_ERROR:   set_msg("KubaZip error code: " + msg); break;
        default:               set_msg("ZipError " + std::to_string(fault) + " (" + errstr() + ")");
    }
}

void
ZipError::setError(long fault, long ec)
{
    switch (fault) {
        case KUBA_ZIP_ERROR:

            switch (ec) {

            case ZIP_ENOINIT:     set_msg("ZIP_ENOINIT (not initialized)"); break;
            case ZIP_EINVENTNAME: set_msg("ZIP_EINVENTNAME (invalid entry name)"); break;
            case ZIP_ENOENT:      set_msg("ZIP_ENOENT (entry not found)"); break;
            case ZIP_EINVMODE:    set_msg("ZIP_EINVMODE (invalid zip mode)"); break;
            case ZIP_EINVLVL:     set_msg("ZIP_EINVLVL (invalid compression level)"); break;
            case ZIP_ENOSUP64:    set_msg("ZIP_ENOSUP64 (no zip64 support)"); break;
            case ZIP_EMEMSET:     set_msg("ZIP_EMEMSET (memset error)"); break;
            case ZIP_EWRTENT:     set_msg("ZIP_EWRTENT (cannot write data to entry)"); break;
            case ZIP_ETDEFLINIT:
                    set_msg("ZIP_ETDEFLINIT (cannot initialize tdefl compressor)");
                    break;
            case ZIP_EINVIDX:     set_msg("ZIP_EINVIDX (invalid index)"); break;
            case ZIP_ENOHDR:      set_msg("ZIP_ENOHDR (header not found)"); break;
            case ZIP_ETDEFLBUF:   set_msg("ZIP_ETDEFLBUF (cannot flush tdefl buffer)"); break;
            case ZIP_ECRTHDR:     set_msg("ZIP_ECRTHDR (cannot create entry header)"); break;
            case ZIP_EWRTHDR:     set_msg("ZIP_EWRTHDR (cannot write entry header)"); break;
            case ZIP_EWRTDIR:     set_msg("ZIP_EWRTDIR (cannot write to central dir)"); break;
            case ZIP_EOPNFILE:    set_msg("ZIP_EOPNFILE (cannot open file)"); break;
            case ZIP_EINVENTTYPE: set_msg("ZIP_EINVENTTYPE (invalid entry type)"); break;
            case ZIP_EMEMNOALLOC:
                    set_msg("ZIP_EMEMNOALLOC (extracting data using no memory allocation)");
                    break;
            case ZIP_ENOFILE:     set_msg("ZIP_ENOFILE (file not found)"); break;
            case ZIP_ENOPERM:     set_msg("ZIP_ENOPERM (no permission)"); break;
            case ZIP_EOOMEM:      set_msg("ZIP_EOOMEM (out of memory)"); break;
            case ZIP_EINVZIPNAME: set_msg("ZIP_EINVZIPNAME (invalid zip archive name)"); break;
            case ZIP_EMKDIR:      set_msg("ZIP_EMKDIR (make dir error)"); break;
            case ZIP_ESYMLINK:    set_msg("ZIP_ESYMLINK (symlink error)"); break;
            case ZIP_ECLSZIP:     set_msg("ZIP_ECLSZIP (close archive error)"); break;
            case ZIP_ECAPSIZE:    set_msg("ZIP_ECAPSIZE (capacity size too small)"); break;
            case ZIP_EFSEEK:      set_msg("ZIP_EFSEEK (fseek error)"); break;
            case ZIP_EFREAD:      set_msg("ZIP_EFREAD (fread error)"); break;
            case ZIP_EFWRITE:     set_msg("ZIP_EFWRITE (fwrite error)"); break;
            case ZIP_ERINIT:      set_msg("ZIP_ERINIT (cannot initialize reader)"); break;
            case ZIP_EWINIT:      set_msg("ZIP_EWINIT (cannot initialize writer)"); break;
            case ZIP_EWRINIT:
                    set_msg("ZIP_EWRINIT (cannot initialize writer from reader)");
                    break;
            case ZIP_EINVAL: set_msg("ZIP_EINVAL (invalid argument)"); break;
            case ZIP_ENORITER:
                    set_msg("ZIP_ENORITER (cannot initialize reader iterator)");
                    break;
            case ZIP_ECHKDIR:
                    set_msg("ZIP_ECHKDIR (check dir error: path exists but is not directory)");
                    break;
            case ZIP_EPASSWD:
                    set_msg("ZIP_EPASSWD (wrong password or password required)");
                    break;

            default: set_msg("ZIP_UNKNOWN (" + std::to_string(ec) + ")"); break;
            }
            break;

        default: setError(fault, std::to_string(ec));
    }
}
}