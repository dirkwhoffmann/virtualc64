// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "TestSupport.h"
#include "Paper.h"

using namespace vc64;

TEST(paper_starts_empty)
{
    Paper paper;

    CHECK_EQ(paper.rows(), (isize)0);
    CHECK_EQ(paper.pages(), (isize)0);
    CHECK(!paper.getDot(0, 0));
}

TEST(paper_records_dots_and_grows)
{
    Paper paper;

    paper.setDot(3, 5, true);

    CHECK(paper.getDot(3, 5));
    CHECK(!paper.getDot(4, 5));
    CHECK_EQ(paper.rows(), (isize)6);
}

TEST(paper_ignores_out_of_range_dots)
{
    Paper paper;

    paper.setDot(-1, 0, true);
    paper.setDot(Paper::width, 0, true);
    paper.setDot(0, -1, true);

    CHECK_EQ(paper.rows(), (isize)0);
}

TEST(paper_form_feed_starts_a_new_page)
{
    Paper paper;

    paper.setDot(0, 0, true);
    paper.formFeed();
    paper.setDot(1, paper.pageStart(1), true);

    CHECK_EQ(paper.pages(), (isize)1);
    CHECK(paper.getDot(0, 0));
    CHECK(paper.getDot(1, paper.pageStart(1)));
}

TEST(paper_auto_breaks_page_without_form_feed)
{
    Paper paper;

    paper.setDot(0, Paper::height, true);

    CHECK_EQ(paper.pages(), (isize)1);
    CHECK(paper.getDot(0, Paper::height));
}

TEST(paper_clear_resets_everything)
{
    Paper paper;

    paper.setDot(0, 0, true);
    paper.formFeed();
    paper.clear();

    CHECK_EQ(paper.rows(), (isize)0);
    CHECK_EQ(paper.pages(), (isize)0);
    CHECK(!paper.getDot(0, 0));
}

TEST(paper_copy_page_produces_known_greyscale_pattern)
{
    Paper paper;

    // A tiny synthetic pattern: two dots set on row 0, one on row 1, nothing
    // else. copyPage() must turn every dot on the page into a single
    // greyscale byte, row-major -- ink black (0x00), bare paper white
    // (0xFF), matching the DeviceGray colour space it is rendered in.
    paper.setDot(0, 0, true);
    paper.setDot(2, 0, true);
    paper.setDot(1, 1, true);

    std::vector<u8> buf(usize(Paper::width) * usize(Paper::height), 0x55);
    auto written = paper.copyPage(0, buf.data(), isize(buf.size()));

    CHECK_EQ(written, isize(buf.size()));

    for (isize y = 0; y < Paper::height; y++) {
        for (isize x = 0; x < Paper::width; x++) {

            auto expected = paper.getDot(x, y) ? u8(0x00) : u8(0xFF);
            CHECK_EQ((int)buf[usize(y) * usize(Paper::width) + usize(x)], (int)expected);
        }
    }

    CHECK_EQ((int)buf[0], (int)0x00);                       // dot -> black
    CHECK_EQ((int)buf[1], (int)0xFF);                       // gap -> white
    CHECK_EQ((int)buf[2], (int)0x00);                       // dot -> black
    CHECK_EQ((int)buf[usize(Paper::width) + 1], (int)0x00);  // dot on row 1
}

TEST(paper_copy_page_returns_zero_for_nonexistent_page)
{
    Paper paper;

    std::vector<u8> buf(usize(Paper::width) * usize(Paper::height), 0);

    // Nothing has been printed at all -- page 0 does not exist yet.
    CHECK_EQ(paper.copyPage(0, buf.data(), isize(buf.size())), (isize)0);

    // A page far beyond anything printed also does not exist.
    paper.setDot(0, 0, true);
    CHECK_EQ(paper.copyPage(5, buf.data(), isize(buf.size())), (isize)0);

    // Negative page indices are rejected too.
    CHECK_EQ(paper.copyPage(-1, buf.data(), isize(buf.size())), (isize)0);
}

TEST(paper_copy_page_never_writes_past_a_short_buffer)
{
    Paper paper;

    paper.setDot(0, 0, true);
    paper.setDot(Paper::width - 1, Paper::height - 1, true);

    // A guard byte right after a too-small buffer must survive untouched --
    // copyPage() must never write beyond `capacity`, however small.
    isize capacity = 10;
    std::vector<u8> buf(usize(capacity) + 1, 0xAA);

    auto written = paper.copyPage(0, buf.data(), capacity);

    CHECK_EQ(written, capacity);
    CHECK_EQ((int)buf[usize(capacity)], (int)0xAA);

    // Zero capacity must be handled the same way: nothing written, no crash.
    CHECK_EQ(paper.copyPage(0, buf.data(), 0), (isize)0);
}
