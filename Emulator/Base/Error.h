// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "ErrorTypes.h"
#include "Exception.h"

//
// VCError
//

struct VC64Error : public util::Exception
{
    VC64Error(ErrorCode code) : Exception((i64)code) { }
    
    const char *what() const throw() override;    
};


//
// ConfigError
//

struct ConfigError : public std::exception
{
    string description;
    
    ConfigError(const string &s) : description(s) { }
    
    const char *what() const throw() override;    
};

struct ConfigArgError : ConfigError {
    ConfigArgError(const string &s) : ConfigError(s) { };
};

struct ConfigFileNotFoundError : ConfigError {
    ConfigFileNotFoundError(const string &s) : ConfigError(s) { };
};

struct ConfigFileReadError : ConfigError {
    ConfigFileReadError(const string &s) : ConfigError(s) { };
};

struct ConfigLockedError : ConfigError {
    ConfigLockedError() : ConfigError("") { };
};

struct ConfigUnsupportedError : ConfigError {
    ConfigUnsupportedError() : ConfigError("") { };
};
