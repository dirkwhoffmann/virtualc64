// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/types/Integers.h"
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

class Error : public GenericException<i64> {

public:

    Error(i64 d = 0, const std::string &s = "") : GenericException<i64>(d, s) { }

    virtual i64 fault() const { return payload; }
    virtual const char *errstr() const noexcept { return "GENERIC"; }
};

}
