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

#include "vcconfig.h"
#include "Mps803.h"
#include <algorithm>
#include <cstdio>

namespace vc64 {

namespace {

// True if `value` is an ASCII decimal digit ('0'-'9'). Used only by
// CHR$(16) POS argument parsing.
bool isAsciiDigit(u8 value) { return value >= '0' && value <= '9'; }

}

void
Mps803::clear()
{
    std::fill(charset.begin(), charset.end(), 0);
    loaded = false;
}

void
Mps803::flashCharset(const u8 *buf, isize len)
{
    if (len != charsetSize) {

        // Plain stderr output rather than logmsg(): this class, like Paper
        // and IecListener, is a plain data owner and not a CoreObject/
        // Loggable, so it has no `log()` member to hang the usual macro on.
        std::fprintf(stderr,
                      "MPS-803 charset has wrong size (%ld, expected %ld); ignored\n",
                      long(len), long(charsetSize));
        return;
    }

    std::copy(buf, buf + len, charset.begin());
    loaded = true;
}

u8
Mps803::glyphRow(Table table, u8 code, isize row) const
{
    if (!loaded || row < 0 || row >= glyphRows) return 0;

    isize base = table == Table::Business ? tableSize : 0;
    isize offset = base + isize(code) * bytesPerGlyph + row;

    return charset[offset];
}

bool
Mps803::dotSet(Table table, u8 code, isize row, isize col) const
{
    if (col < 0 || col >= glyphCols) return false;

    // Bit 7 is the leftmost dot column, bit 2 the rightmost -- see the
    // class comment in Mps803.h. col 0 -> bit 7, col 5 -> bit 2.
    isize bit = 7 - col;
    u8 byte = glyphRow(table, code, row);

    return (byte & (1 << bit)) != 0;
}

void
Mps803::reset()
{
    table = Table::Graphics;
    col = 0;
    lineStart = topMargin;
    reverse = false;
    doubleWidth = false;
    quote = false;
    bitImage = false;
    dotCol = 0;
    pendingRepeat = PendingRepeat::None;
    repeatCount = 0;
    pendingCmd = PendingCmd::None;
    posDigit1 = 0;
    escHigh = 0;
}

void
Mps803::setChannel(u8 sa)
{
    table = (sa == 7) ? Table::Business : Table::Graphics;
}

bool
Mps803::isControlCode(u8 value) const
{
    switch (value) {

        case 17:    // CHR$(17):  select Business set (in-stream)
        case 145:   // CHR$(145): select Graphics set (in-stream)
        case 18:    // CHR$(18):  reverse on
        case 146:   // CHR$(146): reverse off
        case 14:    // CHR$(14):  double width on
            // NOTE: CHR$(15) (double width off) is deliberately NOT listed
            // here. It is now a shared trigger point with a second,
            // independent effect (leave bit-image mode) and is handled
            // directly at the top of write(), before the charset gate --
            // see the comment there for why.
            return true;

        default:
            return false;
    }
}

void
Mps803::applyControlCode(u8 value)
{
    switch (value) {

        case 17:    table = Table::Business; break;
        case 145:   table = Table::Graphics; break;
        case 18:    reverse = true; break;
        case 146:   reverse = false; break;
        case 14:    doubleWidth = true; break;

        default:
            break;
    }
}

void
Mps803::write(u8 value)
{
    // 1. Mid-sequence bytes of a pending CHR$(26) REPEAT (count byte, then
    //    one data byte) are consumed unconditionally: they are raw
    //    graphics data, not subject to quote mode or the charset gate.
    if (pendingRepeat == PendingRepeat::Count) {
        repeatCount = value;
        pendingRepeat = PendingRepeat::Data;
        return;
    }
    if (pendingRepeat == PendingRepeat::Data) {
        pendingRepeat = PendingRepeat::None;
        // A count of 0 means 256, per the manual's worked example.
        isize times = repeatCount == 0 ? 256 : isize(repeatCount);
        for (isize i = 0; i < times; i++) printGraphicsByte(value);
        return;
    }

    // 1b. Mid-sequence bytes of CHR$(16) POS: exactly two ASCII digit
    //     characters immediately following, forming a 2-digit column
    //     number. DEFINED BEHAVIOUR for a non-digit byte (this task's
    //     answer to "what happens with invalid arguments"): it is treated
    //     as digit value 0 for that position, rather than aborting the
    //     sequence -- the sequence always consumes exactly two bytes
    //     (matching the manual's fixed-width argument), so the stream
    //     never desynchronizes over a malformed POS.
    if (pendingCmd == PendingCmd::PosDigit1) {
        posDigit1 = isAsciiDigit(value) ? u8(value - '0') : u8(0);
        pendingCmd = PendingCmd::PosDigit2;
        return;
    }
    if (pendingCmd == PendingCmd::PosDigit2) {
        u8 digit2 = isAsciiDigit(value) ? u8(value - '0') : u8(0);
        isize target = isize(posDigit1) * 10 + isize(digit2);
        col = std::min(target, columnsPerLine - 1);
        pendingCmd = PendingCmd::None;
        return;
    }

    // 1c. Mid-sequence bytes of the CHR$(27) ESC dot-address sequence:
    //     CHR$(27), CHR$(16), nH, nL. The CHR$(27) byte itself is handled
    //     further down (it's what starts this state machine); from here
    //     we expect exactly CHR$(16), then two raw binary bytes. If the
    //     confirmation byte isn't 16, the sequence is malformed -- abort
    //     it (this byte is simply dropped) rather than guessing.
    if (pendingCmd == PendingCmd::EscExpect16) {
        pendingCmd = (value == 16) ? PendingCmd::EscHigh : PendingCmd::None;
        return;
    }
    if (pendingCmd == PendingCmd::EscHigh) {
        escHigh = value;
        pendingCmd = PendingCmd::EscLow;
        return;
    }
    if (pendingCmd == PendingCmd::EscLow) {
        // nH = 000000 P10 P9 (top 2 bits), nL = P8..P1 (bottom 8 bits) --
        // a 10-bit dot address per the manual. The manual states a range
        // of 0-639, which contradicts this printer's 480-dot page width
        // (the manual's stated range is inconsistent with its own
        // 480-dot line width); clamp to the last real dot column instead
        // of wrapping, since wrapping would silently alias two different
        // requested addresses onto the same column.
        isize raw = (isize(escHigh & 0x03) << 8) | isize(value);
        dotCol = std::min(raw, Paper::width - 1);
        pendingCmd = PendingCmd::None;
        return;
    }

    // 2. CHR$(15) is a SHARED TRIGGER POINT: it cancels double width
    //    (a text-mode effect) AND leaves bit-image mode -- two
    //    independent effects of the same byte. Checked here, ahead of the
    //    charset gate below, so a stream that entered bit-image mode
    //    before any charset was ever flashed can still leave it. Like
    //    every other control code, quote mode swallows this and prints
    //    0x0F as a glyph instead (handled by falling through to the
    //    bottom of this function).
    if (!quote && value == 15) {
        doubleWidth = false;
        bitImage = false;
        return;
    }

    // 2b. CR/LF/FF and the START of CHR$(16) POS / CHR$(27) ESC are, like
    //     CHR$(15) above, SHARED TRIGGER POINTS checked ahead of both the
    //     charset gate and the bit-image catch-all: they are physical
    //     paper/head movements, not character rendering, so they work
    //     with no charset loaded and/or mid-bit-image-mode -- exactly what
    //     the manual requires for CR, which explicitly "cancels bit-image
    //     mode".
    //
    //     CR and LF/FF are NOT gated by quote mode: CR is the genuine
    //     end-of-line event (mirrors printGlyph()'s auto-wrap, which also
    //     clears quote unconditionally -- see below), and LF/FF are
    //     physical paper feeds, neither of which is a printable glyph a
    //     quoted string could sensibly contain. POS/ESC's start byte IS
    //     gated by quote, like every other in-band command.
    if (value == 13) {                      // CR
        col = 0;
        reverse = false;
        doubleWidth = false;
        quote = false;
        bitImage = false;                   // manual: CR cancels bit-image mode
        advanceLine(lineSpacing);
        return;
    }
    if (value == 10) {                      // LF: column unchanged, unlike CR
        advanceLine(bitImage ? graphicsLineSpacing : lineSpacing);
        return;
    }
    if (value == 12) {                      // Form feed
        paper.formFeed();
        col = 0;
        lineStart = paper.rows();
        snapToPage();                       // rows() is the new page's top
                                            // EDGE; printing starts below
                                            // its margin
        return;
    }
    if (!quote && value == 16) {            // CHR$(16) POS: begin 2-digit arg
        pendingCmd = PendingCmd::PosDigit1;
        return;
    }
    if (!quote && value == 27) {            // CHR$(27) ESC: begin dot-address arg
        pendingCmd = PendingCmd::EscExpect16;
        return;
    }

    // 3. In bit-image mode, a byte with bit 7 set is graphics data: it
    //    paints one vertical dot column and advances the DOT column, not
    //    the character column. Needs no charset.
    if (bitImage && (value & 0x80)) {
        printGraphicsByte(value);
        return;
    }

    // 4. CHR$(26): begin a REPEAT sequence (count byte, then one data
    //    byte to repeat). Per the manual this is only meaningful once
    //    already inside bit-image mode. Outside bit-image mode, 26 was
    //    never a recognized text control code either, so it falls through
    //    to the ordinary "unrecognized low byte -> ignored" rule below,
    //    same as before this task.
    if (!quote && bitImage && value == 26) {
        pendingRepeat = PendingRepeat::Count;
        return;
    }

    // 5. CHR$(8): enter bit-image mode. Resets the dot column to 0 --
    //    cross-mode head positioning (CHR$(16)/CHR$(27)) is out of scope
    //    for this task. Needs no charset: bit-image is a separate
    //    rendering path from printGlyph().
    if (!quote && value == 8) {
        bitImage = true;
        dotCol = 0;
        return;
    }

    // 6. DECISION: a byte that reaches here while still in bit-image mode
    //    is neither graphics data (bit 7 clear) nor one of the mode's own
    //    control codes handled above. It is silently ignored -- consumed,
    //    but with no effect -- and bit-image mode stays active. This
    //    mirrors the existing "unrecognized control code -> ignored, not
    //    fatal" policy for text mode below, rather than either exiting
    //    bit-image mode unexpectedly or misinterpreting the byte as a dot
    //    column with a phantom bit 7.
    if (bitImage) return;

    // Everything from here on is TEXT-mode dispatch and DOES need a
    // charset -- with no charset loaded, nothing is rendered.
    if (!loaded) return;

    // Quote mode toggles on '"' itself, and the '"' is always still
    // rendered as a glyph -- entering/leaving quote mode never swallows
    // the character that triggered it.
    if (value == '"') {
        quote = !quote;
        printGlyph(value);
        return;
    }

    // Outside quote mode, a recognized control code is obeyed and consumes
    // the byte (nothing is printed for it).
    if (!quote && isControlCode(value)) {
        applyControlCode(value);
        return;
    }

    // Bytes below 0x20 that aren't (yet) implemented control codes --
    // positioning, line spacing, CR/LF -- are out of this task's scope
    // and are silently ignored rather than printed, exactly like a real
    // printer shrugs off a code it doesn't recognize. This only applies
    // outside quote mode: inside quote mode every control code,
    // implemented or not, is rendered as a glyph instead.
    if (!quote && value < 0x20) return;

    printGlyph(value);
}

void
Mps803::printGlyph(u8 code)
{
    isize cellWidth = doubleWidth ? glyphCols * 2 : glyphCols;
    isize x0 = col * cellWidth;

    for (isize row = 0; row < glyphRows; row++) {
        for (isize c = 0; c < glyphCols; c++) {

            bool dot = dotSet(table, code, row, c);
            if (reverse) dot = !dot;

            if (doubleWidth) {
                // Each source dot column is rendered twice, side by side,
                // so the glyph occupies twice the horizontal space.
                paper.setDot(x0 + c * 2,     lineStart + row, dot);
                paper.setDot(x0 + c * 2 + 1, lineStart + row, dot);
            } else {
                paper.setDot(x0 + c, lineStart + row, dot);
            }
        }
    }

    col += doubleWidth ? 2 : 1;
    if (col >= columnsPerLine) {
        col = 0;
        lineStart += lineSpacing;
        // Quote mode clears at end of line, same as it does on an
        // explicit CR (see write()) -- an earlier task left this path
        // (the 80-column auto-wrap) unaddressed; this is the other of
        // the two places it must be cleared.
        quote = false;
    }
}

void
Mps803::printGraphicsByte(u8 value)
{
    // VERTICAL bit convention (see the class comment in Mps803.h --
    // deliberately the OPPOSITE of printGlyph()'s horizontal one): bit 7
    // marked this byte as graphics data and is not itself a dot. Bits
    // 0..6 are the seven dots of this column, bit 0 = TOP, bit 6 = BOTTOM.
    for (isize row = 0; row < glyphRows; row++) {
        bool dot = (value & (1 << row)) != 0;
        paper.setDot(dotCol, lineStart + row, dot);
    }

    dotCol++;

    // Right-edge wrap: writing off the edge of the paper would either
    // silently drop the dot (Paper::setDot ignores out-of-range x) or,
    // worse, leave every subsequent column stuck writing to a
    // permanently-invalid x. Instead, wrap like an implicit line feed --
    // new dot column 0, one line down, at the bit-image line spacing.
    if (dotCol >= Paper::width) {
        dotCol = 0;
        lineStart += graphicsLineSpacing;
    }
}

void
Mps803::advanceLine(isize spacing)
{
    lineStart += spacing;
    snapToPage();

    // Registers the paper feed even though nothing is printed here, so
    // paper.rows() reflects a CR/LF exactly like a physical paper advance
    // would -- otherwise a blank line at the end of a job would never show
    // up in rows()/pages() at all, only lines that are later drawn on
    // would. value=false is a no-op on the bit itself; only the resize
    // (Paper::setDot grows its buffer to cover `y`) matters here.
    paper.setDot(0, lineStart - 1, false);
}

void
Mps803::snapToPage()
{
    isize page = lineStart / Paper::height;
    isize row = lineStart % Paper::height;

    if (row < topMargin) {

        // Inside the margin: down to the printable area.
        lineStart = page * Paper::height + topMargin;

    } else if (row + glyphRows > Paper::height) {

        // The line's dot rows would cross the bottom edge. A glyph split
        // across two sheets is physically impossible on real paper -- and
        // the overflow would land inside the next page's margin, which is
        // supposed to be hard. Move the whole line there instead.
        lineStart = (page + 1) * Paper::height + topMargin;
    }
}

}
