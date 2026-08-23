
// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/types/UUID.h"

#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

namespace utl {

UUID::UUID(const string &str)
{
    *this = UUID::fromString(str);
}

UUID
UUID::v4() {

    std::random_device rd;
    std::mt19937_64 gen(rd());

    UUID uuid;
    uuid.high = gen();
    uuid.low  = gen();

    // Set version (4) → bits 12–15 of high
    uuid.high &= 0xFFFFFFFFFFFF0FFFULL;
    uuid.high |= 0x0000000000004000ULL;

    // Set variant (RFC 4122) → bits 64+ (top 2 bits of low)
    uuid.low &= 0x3FFFFFFFFFFFFFFFULL;
    uuid.low |= 0x8000000000000000ULL;

    return uuid;
}

string
UUID::toString() const {

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    oss << std::setw(8) << (u32)(high >> 32);
    oss << "-";
    oss << std::setw(4) << (u16)(high >> 16);
    oss << "-";
    oss << std::setw(4) << (u16)(high);
    oss << "-";
    oss << std::setw(4) << (u16)(low >> 48);
    oss << "-";
    oss << std::setw(12) << (low & 0x0000FFFFFFFFFFFFULL);

    return oss.str();
}

UUID
UUID::fromString(const std::string& str) {

    if (str.empty() || str == "0") return UUID(0);

    // Remove dashes
    std::string hex;
    hex.reserve(32);

    for (char c : str) {
        if (c == '-') continue;
        if (!std::isxdigit(static_cast<unsigned char>(c))) {
            throw std::invalid_argument("Invalid UUID string");
        }
        hex += c;
    }

    if (hex.size() != 32) {
        return UUID(0);
        // throw std::invalid_argument("Invalid UUID length");
    }

    auto hexToUint64 = [](const std::string& s) -> uint64_t {
        uint64_t value = 0;
        for (char c : s) {
            value <<= 4;
            if (c >= '0' && c <= '9') value |= (c - '0');
            else if (c >= 'a' && c <= 'f') value |= (c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') value |= (c - 'A' + 10);
        }
        return value;
    };

    UUID uuid;
    uuid.high = hexToUint64(hex.substr(0, 16));
    uuid.low  = hexToUint64(hex.substr(16, 16));

    return uuid;
}

}
