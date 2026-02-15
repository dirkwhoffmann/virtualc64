// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include <exception>
#include <string>
#include <any>

namespace utl {

struct Exception : public std::exception {

};

template<typename T>
struct GenericException : public Exception {

    T payload;
    std::string msg;

    explicit GenericException(T payload = {}, std::string msg = "")
    : payload(std::move(payload)), msg(std::move(msg)) { }

    const char *what() const noexcept override {
        return msg.c_str();
    }

    // Setters
    void set_msg(std::string value) { msg = std::move(value); }
    void set_payload(T value) { payload = std::move(value); }
};

class Error : public GenericException<long> {

public:

    Error(long d = 0, const std::string &s = "") : GenericException<long>(d, s) { }

    virtual long fault() const { return payload; }
    virtual const char *errstr() const noexcept { return "GENERIC"; }
};

}
