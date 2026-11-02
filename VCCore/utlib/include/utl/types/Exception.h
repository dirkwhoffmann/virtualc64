// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------
/// @file

#pragma once

#include "utl/types/Integers.h"
#include <exception>
#include <string>
#include <any>

namespace utl {

/// Base class of all exceptions thrown by the libraries
struct Exception : public std::exception {

};

/// An exception that carries a payload and a message
template<typename T>
struct GenericException : public Exception {

    T payload;                                  ///< Exception-specific payload
    std::string msg;                            ///< Message returned by what()

    /// Creates an exception with a payload and a message.
    explicit GenericException(T payload = {}, std::string msg = "")
    : payload(std::move(payload)), msg(std::move(msg)) { }

    /// Returns the message
    const char *what() const noexcept override {
        return msg.c_str();
    }

    /// Setters
    void set_msg(std::string value) { msg = std::move(value); }
    /// Replaces the payload
    void set_payload(T value) { payload = std::move(value); }
};

/** @brief  Base class of the errors thrown by the API
 *
 *  The payload is an error code. Derived classes (CoreError, IOError,
 *  MediaError) define the codes and a human-readable message.
 */
class Error : public GenericException<i64> {

public:

    /// Creates an error with an error code and a message
    Error(i64 d = 0, const std::string &s = "") : GenericException<i64>(d, s) { }

    /// Returns the error code
    virtual i64 fault() const { return payload; }
    /// Returns the name of the error code
    virtual const char *errstr() const noexcept { return "GENERIC"; }
};

}
