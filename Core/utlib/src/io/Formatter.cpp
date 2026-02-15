// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/io/Formatter.h"

namespace utl {

void
Formatter::printTable(std::ostream& os, const vector<string> &cells, const char *fmt)
{
    printTable(os, cells, string(fmt));
}

void
Formatter::printTable(std::ostream& os, const vector<string> &cells, const string &fmt)
{
    TableFormat tf;
    for (auto c : fmt) {
        if (c) tf.columns.push_back(ColumnFormat { .align = c });
    }
    printTable(os, cells, tf);
}

void
Formatter::printTable(std::ostream& os, const vector<string> &cells, const TableFormat &fmt)
{
    if (fmt.columns.empty()) return;

    auto items = isize(cells.size());
    auto cols  = isize(fmt.columns.size());
    auto rows  = (items + cols - 1) / cols;

    // Maps a row / column index to the item number
    auto index = [&](isize r, isize c) -> isize {

        if (fmt.layout == Layout::RowMajor) {
            return r * cols + c;
        } else {
            return c * (items / cols) + std::min(c, items % cols) + r;
        }
    };

    // Derives missing column width information
    auto deriveWidths = [&](const vector<ColumnFormat> &fmts) -> vector<ColumnFormat> {

        vector<ColumnFormat> result = fmts;

        for (isize c = 0; c < cols; c++) {

            if (result[c].width != 0) continue;

            for (isize r = 0; r < rows; r++) {
                if (auto i = index(r,c); i < items) {
                    result[c].width = std::max(result[c].width, isize(cells[i].size()));
                }
            }
        }
        return result;
    };

    // Derive missing column information
    auto fmts = deriveWidths(fmt.columns);

    // Print the table
    for (isize r = 0; r < rows; r++) {

        os << fmt.inset;
        
        for (isize c = 0; c < cols; c++) {
            if (auto i = index(r,c); i < items) {

                if (c != 0) { os << fmt.separator; }
                os << padString(cells[i], fmts[c].width, fmts[c].align);
            }
        }
        os << '\n';
    }
}

}
