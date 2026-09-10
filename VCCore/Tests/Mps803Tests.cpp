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
#include "Mps803.h"
#include "Paper.h"
#include <vector>

using namespace vc64;

namespace {

// A synthetic charset where every glyph is all-bits-set (0xFF). Used by the
// printing tests below where the exact dot pattern doesn't matter, only
// that *something* gets rendered. No real ROM dump is read or embedded.
std::vector<u8> testCharset(Mps803::charsetSize, 0xFF);

// Every page's printable area starts this many dot rows below its top
// edge; all row expectations below are relative to it.
constexpr isize top = Mps803::topMargin;

// Builds a synthetic charset buffer of exactly Mps803::charsetSize bytes,
// all zero except for one 7-byte glyph slot in each of the two tables,
// which is set to `rowsGraphics` / `rowsBusiness` respectively. This is
// entirely made up data -- no real ROM dump is read or embedded here, so
// this test runs the same on CI and on any developer's machine.
std::vector<u8> makeSyntheticCharset(u8 code,
                                      const std::array<u8, 7> &rowsGraphics,
                                      const std::array<u8, 7> &rowsBusiness)
{
    std::vector<u8> buf(Mps803::charsetSize, 0);

    isize graphicsOffset = isize(code) * Mps803::bytesPerGlyph;
    isize businessOffset = Mps803::tableSize + isize(code) * Mps803::bytesPerGlyph;

    for (isize i = 0; i < Mps803::glyphRows; i++) {
        buf[graphicsOffset + i] = rowsGraphics[i];
        buf[businessOffset + i] = rowsBusiness[i];
    }

    return buf;
}

}

TEST(mps803_starts_without_a_charset)
{
    Paper paper;
    Mps803 mps803(paper);

    CHECK(!mps803.hasCharset());
    CHECK_EQ(mps803.glyphRow(Mps803::Table::Graphics, 65, 0), (u8)0);
}

TEST(mps803_flash_rejects_wrong_size_buffer)
{
    Paper paper;
    Mps803 mps803(paper);

    std::vector<u8> tooShort(Mps803::charsetSize - 1, 0xFF);
    mps803.flashCharset(tooShort.data(), isize(tooShort.size()));

    CHECK(!mps803.hasCharset());

    std::vector<u8> tooLong(Mps803::charsetSize + 1, 0xFF);
    mps803.flashCharset(tooLong.data(), isize(tooLong.size()));

    CHECK(!mps803.hasCharset());
}

TEST(mps803_flash_accepts_correctly_sized_buffer)
{
    Paper paper;
    Mps803 mps803(paper);

    // 'F' from the real ROM (code 70), used here only as a plausible-looking
    // row pattern -- these bytes are hand-written, not read from any file.
    std::array<u8, 7> f = {{ 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80 }};
    std::array<u8, 7> zeros = {{ 0, 0, 0, 0, 0, 0, 0 }};
    auto buf = makeSyntheticCharset(70, f, zeros);

    mps803.flashCharset(buf.data(), isize(buf.size()));

    CHECK(mps803.hasCharset());
    CHECK_EQ(mps803.size(), Mps803::charsetSize);
}

TEST(mps803_clear_removes_a_loaded_charset)
{
    Paper paper;
    Mps803 mps803(paper);

    std::vector<u8> buf(Mps803::charsetSize, 0x42);
    mps803.flashCharset(buf.data(), isize(buf.size()));
    CHECK(mps803.hasCharset());

    mps803.clear();

    CHECK(!mps803.hasCharset());
    CHECK_EQ(mps803.glyphRow(Mps803::Table::Graphics, 0, 0), (u8)0);
}

// Locks down the table-offset and bit-order decisions: a synthetic glyph
// planted at the same character code in BOTH tables (table 0 at offset 0,
// table 1 at offset tableSize/0x0700) must be read back independently and
// with bit 7 as the leftmost dot / bit 2 as the rightmost dot.
TEST(mps803_glyph_row_reads_table_offset_and_bit_order_correctly)
{
    Paper paper;
    Mps803 mps803(paper);

    // Graphics-table glyph: a single leftmost dot on row 0 (bit 7 set),
    // nothing else set anywhere in that row.
    std::array<u8, 7> graphicsGlyph = {{ 0x80, 0, 0, 0, 0, 0, 0 }};

    // Business-table glyph at the SAME code: a single rightmost dot on row
    // 0 (bit 2 set) -- deliberately different from the graphics glyph so a
    // table mix-up would be caught.
    std::array<u8, 7> businessGlyph = {{ 0x04, 0, 0, 0, 0, 0, 0 }};

    auto buf = makeSyntheticCharset(65, graphicsGlyph, businessGlyph);
    mps803.flashCharset(buf.data(), isize(buf.size()));

    // Raw row bytes come back unchanged, from the correct table.
    CHECK_EQ(mps803.glyphRow(Mps803::Table::Graphics, 65, 0), (u8)0x80);
    CHECK_EQ(mps803.glyphRow(Mps803::Table::Business, 65, 0), (u8)0x04);

    // Bit-order decode: graphics glyph has its dot at the leftmost column
    // (col 0), business glyph has its dot at the rightmost column (col 5).
    CHECK(mps803.dotSet(Mps803::Table::Graphics, 65, 0, 0));
    for (isize col = 1; col < Mps803::glyphCols; col++) {
        CHECK(!mps803.dotSet(Mps803::Table::Graphics, 65, 0, col));
    }

    CHECK(mps803.dotSet(Mps803::Table::Business, 65, 0, 5));
    for (isize col = 0; col < Mps803::glyphCols - 1; col++) {
        CHECK(!mps803.dotSet(Mps803::Table::Business, 65, 0, col));
    }

    // A different code in the same tables reads back as all-zero (blank).
    CHECK_EQ(mps803.glyphRow(Mps803::Table::Graphics, 66, 0), (u8)0);
    CHECK_EQ(mps803.glyphRow(Mps803::Table::Business, 66, 0), (u8)0);
}

TEST(mps803_glyph_row_out_of_range_row_is_safe)
{
    Paper paper;
    Mps803 mps803(paper);

    std::vector<u8> buf(Mps803::charsetSize, 0xFF);
    mps803.flashCharset(buf.data(), isize(buf.size()));

    CHECK_EQ(mps803.glyphRow(Mps803::Table::Graphics, 0, -1), (u8)0);
    CHECK_EQ(mps803.glyphRow(Mps803::Table::Graphics, 0, Mps803::glyphRows), (u8)0);
}

TEST(mps803_prints_a_character)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write('A');

    // 'A' must put at least one dot in the first character cell (which
    // sits just below the page's top margin)
    bool any = false;
    for (isize x = 0; x < 6; x++) {
        for (isize y = 0; y < 7; y++) any |= paper.getDot(x, top + y);
    }
    CHECK(any);
}

TEST(mps803_advances_the_column)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write('A');
    driver.write('A');

    CHECK_EQ(driver.column(), (isize)2);
}

TEST(mps803_wraps_at_the_end_of_a_line)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    for (isize i = 0; i < 81; i++) driver.write('A');

    CHECK_EQ(driver.column(), (isize)1);
    CHECK(paper.rows() > top + 7);
}

TEST(mps803_prints_nothing_without_a_charset)
{
    Paper paper;
    Mps803 driver(paper);

    driver.write('A');

    CHECK_EQ(paper.rows(), (isize)0);
}

TEST(mps803_line_advances_by_twelve_dot_rows_not_seven)
{
    // Verified against the MPS-803 User's Guide (spec sheet, linespacing
    // section): a printed line advances 12 dot rows (6 LPI @ 72 dpi), even
    // though the glyph itself is only 7 rows tall. After wrapping past the
    // end of one line, the second line's glyph must land starting at row
    // 12, not row 7 -- and rows 7-11 (the leading gap) must stay blank.
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    for (isize i = 0; i < Mps803::columnsPerLine; i++) driver.write('A');
    CHECK_EQ(driver.column(), (isize)0);

    // Rows top+7..top+11 (the gap between the first glyph's bottom row
    // and the second line's top row) must be untouched.
    for (isize y = top + 7; y < top + 12; y++) {
        for (isize x = 0; x < Paper::width; x++) {
            CHECK(!paper.getDot(x, y));
        }
    }

    // The next character prints starting at row top+12 (top of the
    // second line), not top+7.
    driver.write('A');
    bool anyAtRow12 = false;
    for (isize x = 0; x < 6; x++) anyAtRow12 |= paper.getDot(x, top + 12);
    CHECK(anyAtRow12);
}

TEST(mps803_unimplemented_control_codes_are_ignored_not_fatal)
{
    // Bytes below 0x20 that still have no defined behaviour must be
    // silently ignored -- never fatal. An assert here would kill the
    // emulation thread mid-print-job on real-world data. This also checks
    // that ignoring a control byte does not advance the column or print
    // anything.
    //
    // Excluded from the loop below: 8 (bit image), 10 (LF), 12 (FF),
    // 13 (CR), 14/15 (double width on/off), 16 (POS), 17/18
    // (business/reverse on), 26 (repeat), 27 (ESC dot address) -- every
    // one of these now has real, tested behaviour (this task or an
    // earlier one) and is covered by its own dedicated test elsewhere in
    // this file. Looping over them here would no longer exercise "ignored
    // and inert" at all -- e.g. LF/CR now advance the paper on purpose.
    static const u8 stillUnimplemented[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 9, 11,
        19, 20, 21, 22, 23, 24, 25, 28, 29, 30, 31
    };

    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    for (u8 code : stillUnimplemented) driver.write(code);

    CHECK_EQ(driver.column(), (isize)0);
    CHECK_EQ(paper.rows(), (isize)0);
}

// The single most dangerous constant in this whole task: bit order. A
// synthetic glyph with a KNOWN asymmetric dot pattern (an arrow pointing
// left, unambiguous under mirroring) is planted at code 'A' in the
// Graphics table, printed at column 0, and every one of the 42 dots in the
// resulting character cell is checked individually against Paper -- not
// just "something got set somewhere". A renderer that swapped left/right
// (e.g. read bit 2 as leftmost instead of bit 7) would still pass the
// "any dot set" tests above but would fail every assertion here.
TEST(mps803_renders_glyph_with_correct_left_to_right_orientation)
{
    Paper paper;
    Mps803 driver(paper);

    // Row-by-row dot pattern (1 = on, 0 = off), 6 columns, left to right:
    //   row 0: . . . . . #      (single dot at the far right)
    //   row 1: . . . . # .
    //   row 2: . . . # . .
    //   row 3: # # # # # #      (full row -- a crossbar to anchor position)
    //   row 4: . . . # . .
    //   row 5: . . . . # .
    //   row 6: . . . . . #
    // This is an arrowhead pointing left, with the crossbar showing exactly
    // where the tip lands. Mirrored, the tip would point right instead --
    // unmistakable either way.
    static const bool pattern[Mps803::glyphRows][Mps803::glyphCols] = {
        { false, false, false, false, false, true  },
        { false, false, false, false, true,  false },
        { false, false, false, true,  false, false },
        { true,  true,  true,  true,  true,  true  },
        { false, false, false, true,  false, false },
        { false, false, false, false, true,  false },
        { false, false, false, false, false, true  },
    };

    std::array<u8, 7> rows = {{ 0, 0, 0, 0, 0, 0, 0 }};
    for (isize row = 0; row < Mps803::glyphRows; row++) {
        u8 byte = 0;
        for (isize col = 0; col < Mps803::glyphCols; col++) {
            // Bit 7 is the leftmost dot, bit 2 the rightmost (see
            // Mps803.h). This is the ROM's own encoding, used here only to
            // author the synthetic byte -- the code under test is
            // dotSet()/printGlyph(), not this loop.
            if (pattern[row][col]) byte |= u8(1 << (7 - col));
        }
        rows[usize(row)] = byte;
    }

    auto buf = makeSyntheticCharset('A', rows, rows);
    driver.flashCharset(buf.data(), isize(buf.size()));
    driver.setChannel(0);

    driver.write('A');

    for (isize row = 0; row < Mps803::glyphRows; row++) {
        for (isize col = 0; col < Mps803::glyphCols; col++) {
            bool expected = pattern[row][col];
            bool actual = paper.getDot(col, top + row);
            CHECK_EQ((int)actual, (int)expected);
        }
    }
}

TEST(mps803_selects_the_business_charset_from_the_secondary_address)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));

    driver.setChannel(7);

    CHECK(driver.isBusiness());
}

TEST(mps803_switches_charset_in_the_byte_stream)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write(17);                       // CHR$(17): business
    CHECK(driver.isBusiness());

    driver.write(145);                      // CHR$(145): graphics
    CHECK(!driver.isBusiness());
}

TEST(mps803_toggles_reverse)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write(18);                       // CHR$(18): reverse on
    CHECK(driver.isReverse());

    driver.write(146);                      // CHR$(146): reverse off
    CHECK(!driver.isReverse());
}

TEST(mps803_reverse_inverts_the_printed_dots)
{
    Paper paper;
    Mps803 driver(paper);

    // A charset whose glyphs are all blank, so reverse must fill the cell
    std::vector<u8> blank(testCharset.size(), 0x00);
    driver.flashCharset(blank.data(), isize(blank.size()));
    driver.setChannel(0);

    driver.write(18);
    driver.write('A');

    CHECK(paper.getDot(0, top));
}

TEST(mps803_double_width_doubles_the_column_advance)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write(14);                       // CHR$(14): double width on
    driver.write('A');

    CHECK_EQ(driver.column(), (isize)2);

    driver.write(15);                       // CHR$(15): back to normal
    driver.write('A');

    CHECK_EQ(driver.column(), (isize)3);
}

// Proves double width doesn't just advance two columns, but actually
// duplicates each source dot column side by side. Uses an asymmetric
// synthetic glyph (dot pattern differs per column) so a bug that merely
// stretched the column counter -- without doubling the dots themselves --
// would fail this test even though it would pass the coarser test above.
TEST(mps803_double_width_doubles_the_dots_horizontally)
{
    Paper paper;
    Mps803 driver(paper);

    // Row 0 pattern: dot on at columns 1, 3, 5 only (asymmetric).
    std::array<u8, 7> rows = {{ 0, 0, 0, 0, 0, 0, 0 }};
    // Bit 7 = leftmost (col 0) .. bit 2 = rightmost (col 5).
    // Columns on: 1, 3, 5 -> bits (7-1)=6, (7-3)=4, (7-5)=2
    rows[0] = u8((1 << 6) | (1 << 4) | (1 << 2));

    auto buf = std::vector<u8>(Mps803::charsetSize, 0);
    isize graphicsOffset = isize(u8('A')) * Mps803::bytesPerGlyph;
    for (isize i = 0; i < Mps803::glyphRows; i++) buf[graphicsOffset + i] = rows[usize(i)];

    driver.flashCharset(buf.data(), isize(buf.size()));
    driver.setChannel(0);

    driver.write(14);                       // double width on
    driver.write('A');

    // Source dot columns 1, 3, 5 must each appear TWICE, side by side, at
    // absolute paper x = 2*col and 2*col+1 (each source column doubled in
    // place, 12 dots total for the 6-column glyph).
    bool expected[12] = {
        false, false,  // source col 0 (off) -> paper x 0,1
        true,  true,   // source col 1 (on)  -> paper x 2,3
        false, false,  // source col 2 (off) -> paper x 4,5
        true,  true,   // source col 3 (on)  -> paper x 6,7
        false, false,  // source col 4 (off) -> paper x 8,9
        true,  true,   // source col 5 (on)  -> paper x 10,11
    };

    for (isize x = 0; x < 12; x++) {
        CHECK_EQ((int)paper.getDot(x, top), (int)expected[x]);
    }
}

TEST(mps803_quote_mode_prints_control_codes_as_glyphs)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write('"');
    isize before = driver.column();
    driver.write(18);                       // inside quotes: printed, not obeyed

    CHECK(!driver.isReverse());
    CHECK_EQ(driver.column(), before + 1);
}

// The quoted character itself must still be printed -- entering quote mode
// does not swallow the '"'. Uses an all-blank charset except for the '"'
// glyph itself, so if the '"' were swallowed (never rendered) this would
// still incidentally advance the column via some other path; instead we
// check the column advanced by exactly one for the '"' write itself.
TEST(mps803_quote_character_itself_is_printed)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    CHECK_EQ(driver.column(), (isize)0);
    driver.write('"');
    CHECK_EQ(driver.column(), (isize)1);
}

// --- Bit-image graphics ------------------------------------------------

TEST(mps803_enters_and_leaves_bit_image_mode)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write(8);                        // CHR$(8): bit image on
    CHECK(driver.isBitImage());

    driver.write(15);                       // CHR$(15): back to text
    CHECK(!driver.isBitImage());
}

// Bit-image mode must work with NO charset loaded at all -- graphics is a
// wholly separate rendering path from printGlyph()/the charset ROM.
TEST(mps803_prints_a_graphics_byte_as_a_dot_column)
{
    Paper paper;
    Mps803 driver(paper);
    driver.setChannel(0);

    driver.write(8);
    driver.write(0xFF);                     // bit 7 set, all seven dots on

    for (isize y = 0; y < 7; y++) CHECK(paper.getDot(0, top + y));
}

// The load-bearing bit-order test: bit-image bytes are VERTICAL (bit 0 =
// top dot, bit 6 = bottom), the OPPOSITE convention from a text charset
// row byte (bit 7 = leftmost dot). Getting this backwards would print
// upside-down graphics while leaving text completely unaffected -- exactly
// the kind of bug a casual look would miss.
TEST(mps803_graphics_byte_maps_bits_to_dots)
{
    Paper paper;
    Mps803 driver(paper);
    driver.setChannel(0);

    driver.write(8);
    driver.write(0x81);                     // bit 7 marks graphics, bit 0 set

    CHECK(paper.getDot(0, top));
    CHECK(!paper.getDot(0, top + 1));
}

TEST(mps803_repeats_a_graphics_byte)
{
    Paper paper;
    Mps803 driver(paper);
    driver.setChannel(0);

    driver.write(8);
    driver.write(26);                       // CHR$(26): repeat
    driver.write(3);                        // three times
    driver.write(0xFF);

    CHECK(paper.getDot(0, top));
    CHECK(paper.getDot(1, top));
    CHECK(paper.getDot(2, top));
    CHECK(!paper.getDot(3, top));
}

// CHR$(26) with a count byte of 0 means 256 repeats, not zero -- per the
// manual's own worked example. Checked by advancing exactly 256 dot
// columns and confirming the 256th (index 255) is set and the 257th
// (index 256) is not.
TEST(mps803_repeat_count_of_zero_means_256)
{
    Paper paper;
    Mps803 driver(paper);
    driver.setChannel(0);

    driver.write(8);
    driver.write(26);                       // CHR$(26): repeat
    driver.write(0);                        // count of 0 -> 256 repeats
    driver.write(0xFF);

    CHECK_EQ(driver.dotColumn(), (isize)256);
    CHECK(paper.getDot(255, top));
    CHECK(!paper.getDot(256, top));
}

// DECISION (documented, not specified by the brief): a byte received while
// in bit-image mode that does NOT have bit 7 set (and isn't one of the
// mode's own control codes, CHR$(15)/CHR$(26)) is silently ignored -- it
// does not advance the dot column, does not print anything, and does NOT
// exit bit-image mode. This mirrors the existing "unrecognized control
// code is ignored, not fatal" policy already used for text mode, rather
// than crashing, misinterpreting the byte as graphics data with a phantom
// high bit, or unexpectedly falling back to text rendering.
TEST(mps803_non_graphics_byte_in_bit_image_mode_is_ignored)
{
    Paper paper;
    Mps803 driver(paper);
    driver.setChannel(0);

    driver.write(8);
    CHECK(driver.isBitImage());

    driver.write(0x05);                     // bit 7 clear, not a known code

    CHECK(driver.isBitImage());             // still in bit-image mode
    CHECK_EQ(driver.dotColumn(), (isize)0); // dot column did not advance
    CHECK_EQ(paper.rows(), (isize)0);       // nothing was printed

    // Bit-image mode must still be fully usable afterwards.
    driver.write(0xFF);
    CHECK(paper.getDot(0, top));
}

// CHR$(15)'s dual meaning, locked down: it must cancel double width AND
// leave bit-image mode, both from a single byte.
TEST(mps803_chr15_cancels_double_width_and_leaves_bit_image_mode)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write(14);                       // CHR$(14): double width on
    driver.write(8);                        // CHR$(8): bit image on
    CHECK(driver.isDoubleWidth());
    CHECK(driver.isBitImage());

    driver.write(15);                       // CHR$(15): cancel both

    CHECK(!driver.isDoubleWidth());
    CHECK(!driver.isBitImage());
}

// Graphics mode uses its own, tighter line spacing (8 dot rows / 9 LPI)
// rather than the text-mode spacing (12 dot rows / 6 LPI). Wraps a single
// dot column past the right edge (480 dots) to force a line advance, and
// checks the second line's dots land exactly graphicsLineSpacing rows
// below the first, not lineSpacing rows below.
TEST(mps803_graphics_line_spacing_is_eight_dots_not_twelve)
{
    Paper paper;
    Mps803 driver(paper);
    driver.setChannel(0);

    driver.write(8);                        // bit image on
    for (isize i = 0; i < Paper::width; i++) driver.write(0xFF); // fill line 0

    CHECK_EQ(driver.dotColumn(), (isize)0); // wrapped back to column 0
    CHECK(paper.getDot(479, top + 6));      // last column of line 0, bottom dot
    CHECK(!paper.getDot(0, top + 7));       // text spacing NOT used

    // Write a column with only its TOP dot on (bit 0 = top, per the
    // vertical bit-image convention), so exactly one absolute row is set
    // by this write -- unlike 0xFF, which would set all 7 rows of
    // whichever line it lands on and make the two spacings
    // indistinguishable by a single-row check.
    driver.write(0x81);                     // bit 7 (graphics) | bit 0 (top dot)

    CHECK(paper.getDot(0, top + Mps803::graphicsLineSpacing));   // 8 below the margin: correct
    CHECK(!paper.getDot(0, top + Mps803::lineSpacing));          // 12 below: text spacing, wrong here
}

// --- Positioning, line spacing and form feed ----------------------------

TEST(mps803_imposes_a_hard_top_margin)
{
    // The real MPS-803 is a friction-feed printer with no enforced
    // margin -- where printing starts is wherever the user loaded the
    // paper. The emulator paginates the continuous roll for display, and
    // a hard top margin keeps the first line off the sheet's edge, the
    // way any sensibly loaded sheet would look.
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write('A');

    // Nothing above the margin ...
    for (isize y = 0; y < top; y++) {
        for (isize x = 0; x < Paper::width; x++) {
            CHECK(!paper.getDot(x, y));
        }
    }

    // ... and the first glyph row lands exactly on it.
    bool any = false;
    for (isize x = 0; x < Mps803::glyphCols; x++) any |= paper.getDot(x, top);
    CHECK(any);
}

TEST(mps803_top_margin_applies_after_an_automatic_page_break)
{
    // Lines flow off the bottom of one page onto the next; the next
    // page's margin must be just as hard as the first one's. With the
    // margin at 36 and 12-dot line spacing, line k starts at 36 + 12k;
    // line 63 would start exactly on the next page's top edge (row 792)
    // and must be pushed down to its printable area instead.
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    for (isize i = 0; i < 63; i++) driver.write(13);
    driver.write('A');

    // The second page's margin stays blank ...
    for (isize y = paper.pageStart(1); y < paper.pageStart(1) + top; y++) {
        for (isize x = 0; x < Paper::width; x++) {
            CHECK(!paper.getDot(x, y));
        }
    }

    // ... and the glyph starts right below it.
    bool any = false;
    for (isize x = 0; x < Mps803::glyphCols; x++) {
        any |= paper.getDot(x, paper.pageStart(1) + top);
    }
    CHECK(any);
}

TEST(mps803_line_that_would_straddle_a_page_moves_wholly_to_the_next)
{
    // A glyph split across two sheets is physically impossible on real
    // paper and would also print into the next page's margin, defeating
    // it. Bit-image lines advance 8 dots at a time, which (unlike the
    // 12-dot text spacing) can land a 7-dot line across the boundary:
    // from row 36, line 94 starts at row 788 and its dots would reach row
    // 794. The whole line must move below the next page's margin instead.
    Paper paper;
    Mps803 driver(paper);
    driver.setChannel(0);

    driver.write(8);                        // bit image on
    for (isize i = 0; i < 94; i++) driver.write(10);   // LF at 8-dot spacing
    driver.write(0x81);                     // graphics byte, top dot only

    CHECK(!paper.getDot(0, 788));                       // not straddling
    CHECK(!paper.getDot(0, paper.pageStart(1)));        // not in the margin
    CHECK(paper.getDot(0, paper.pageStart(1) + top));   // below it
}

TEST(mps803_carriage_return_starts_a_new_line)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write('A');
    driver.write(13);                       // CR

    CHECK_EQ(driver.column(), (isize)0);
    CHECK(paper.rows() > top + 7);
}

TEST(mps803_carriage_return_clears_transient_modes)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write(18);                       // reverse on
    driver.write(13);                       // CR

    CHECK(!driver.isReverse());
}

// Per the manual, CR also cancels bit-image mode -- documented as a
// separate effect from the reverse/double-width/quote reset above, and
// worth locking down on its own since it's the one CR effect that reaches
// past the ordinary text-mode flags.
TEST(mps803_carriage_return_cancels_bit_image_mode)
{
    Paper paper;
    Mps803 driver(paper);
    driver.setChannel(0);

    driver.write(8);                        // CHR$(8): bit image on
    CHECK(driver.isBitImage());

    driver.write(13);                       // CR

    CHECK(!driver.isBitImage());
}

TEST(mps803_line_feed_advances_without_resetting_column)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write('A');
    isize colBefore = driver.column();
    driver.write(10);                       // CHR$(10): LF

    CHECK_EQ(driver.column(), colBefore);
    // The paper physically advances even though nothing new gets drawn on
    // the new line -- LF must not be a no-op just because it left no dot.
    CHECK(paper.rows() > top + 7);
}

TEST(mps803_form_feed_starts_a_new_page)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write('A');
    driver.write(12);                       // CHR$(12): form feed

    CHECK_EQ(paper.pages(), (isize)1);
    CHECK_EQ(driver.column(), (isize)0);
}

// The print head must actually land on the new page, not merely advance
// paper.pages() -- the next glyph written after FF should appear at the
// new page's first row.
TEST(mps803_form_feed_starts_printing_on_the_new_page)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write('A');
    driver.write(12);                       // form feed
    driver.write('A');

    bool anyAtPageStart = false;
    for (isize x = 0; x < Mps803::glyphCols; x++) {
        anyAtPageStart |= paper.getDot(x, paper.pageStart(1) + top);
    }
    CHECK(anyAtPageStart);
}

TEST(mps803_positions_the_print_head)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write(16);                       // CHR$(16): POS
    driver.write('0');
    driver.write('5');

    CHECK_EQ(driver.column(), (isize)5);
}

// DECISION (documented, not specified by the brief): a non-digit byte in
// either of CHR$(16) POS's two argument positions is treated as digit
// value 0, rather than aborting the sequence. This keeps the sequence's
// length fixed at exactly two bytes no matter what -- the manual commits
// to a 2-digit argument, so consuming anything other than exactly two
// bytes here would desynchronize every byte after a malformed POS.
TEST(mps803_pos_with_non_digit_argument_is_treated_as_zero)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write(16);                       // CHR$(16): POS
    driver.write('X');                      // not a digit -> treated as 0
    driver.write('5');

    CHECK_EQ(driver.column(), (isize)5);    // "05" -> column 5, same as the brief's test
}

TEST(mps803_pos_clamps_a_column_beyond_the_page_width)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write(16);                       // CHR$(16): POS
    driver.write('9');
    driver.write('9');                      // "99" -> clamp to the last real column

    CHECK_EQ(driver.column(), (isize)(Mps803::columnsPerLine - 1));
}

TEST(mps803_esc_sets_the_dot_column)
{
    Paper paper;
    Mps803 driver(paper);
    driver.setChannel(0);

    driver.write(27);                       // CHR$(27): ESC
    driver.write(16);                       // confirms the dot-address sequence
    driver.write(1);                        // nH: bit 8 of the address set
    driver.write(0x2C);                     // nL: 44 -> total 256 + 44 = 300

    CHECK_EQ(driver.dotColumn(), (isize)300);
}

// Required test: the manual's own worked range for the ESC dot-address
// sequence is "0 through 639", which does not fit this printer's 480-dot
// page -- an inconsistency in the manual itself. Encodes exactly 639 (the
// top of the manual's
// stated range) and checks it lands clamped at the last real dot column
// (479), not wrapped or written off-paper.
TEST(mps803_esc_clamps_a_dot_address_beyond_the_page)
{
    Paper paper;
    Mps803 driver(paper);
    driver.setChannel(0);

    driver.write(27);                       // CHR$(27): ESC
    driver.write(16);
    driver.write(0x02);                     // nH
    driver.write(0x7F);                     // nL -> raw 512 + 127 = 639

    CHECK_EQ(driver.dotColumn(), (isize)(Paper::width - 1));
}

// A malformed ESC sequence (not followed by the CHR$(16) confirmation
// byte) must abort cleanly rather than corrupting the rest of the stream:
// the offending byte is dropped, and the very next byte is dispatched
// normally again.
TEST(mps803_esc_sequence_aborts_if_not_followed_by_chr16)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write(27);                       // CHR$(27): ESC
    driver.write('A');                      // not CHR$(16) -> sequence aborts

    CHECK_EQ(driver.column(), (isize)0);    // the aborting byte was not printed

    driver.write('A');                      // the stream resumes normally
    CHECK_EQ(driver.column(), (isize)1);
}

// REQUIRED (carried from an earlier task's open item): quote mode must
// clear at end of line. This is path (a), the explicit carriage return.
TEST(mps803_quote_mode_clears_on_carriage_return)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write('"');
    CHECK(driver.isQuote());

    driver.write(13);                       // CR

    CHECK(!driver.isQuote());
}

// REQUIRED (carried from an earlier task's open item): quote mode must
// also clear at the 80-column auto-wrap, which already existed before
// this task and does not go through CR at all -- a separate code path
// that needs its own explicit reset.
TEST(mps803_quote_mode_clears_on_auto_wrap)
{
    Paper paper;
    Mps803 driver(paper);
    driver.flashCharset(testCharset.data(), isize(testCharset.size()));
    driver.setChannel(0);

    driver.write('"');
    CHECK(driver.isQuote());
    CHECK_EQ(driver.column(), (isize)1);

    // One more column than needed to reach columnsPerLine drives the
    // wrap exactly, without going through CR.
    for (isize i = 0; i < Mps803::columnsPerLine - 1; i++) driver.write('A');

    CHECK_EQ(driver.column(), (isize)0);
    CHECK(!driver.isQuote());
}
