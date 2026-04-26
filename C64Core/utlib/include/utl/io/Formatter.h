// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"
#include "utl/support/Strings.h"
#include <iostream>

namespace utl {

class Formatter {

public:

    enum class Layout { RowMajor, ColMajor };

    struct ColumnFormat {

        char  align = 'l';   // 'l'|'c'|'r'
        isize width = 0;     // 0 = auto
    };

    struct TableFormat {

        vector<ColumnFormat> columns = { };
        Layout layout                = Layout::ColMajor;
        string inset                 = "";
        string separator             = " ";
    };

    static void printTable(std::ostream& os, const vector<string> &cells, const char *fmt);
    static void printTable(std::ostream& os, const vector<string> &cells, const string &fmt);
    static void printTable(std::ostream& os, const vector<string> &cells, const TableFormat &fmt);
};

}
