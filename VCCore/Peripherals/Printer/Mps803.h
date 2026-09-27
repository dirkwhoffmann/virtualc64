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

#pragma once

#include "BasicTypes.h"
#include "Paper.h"
#include <vector>

namespace vc64 {

// Owns the MPS-803 character ROM (a user-supplied file, exactly like the
// C64's own BASIC/KERNAL/CHAR ROMs) and gives access to its glyph data.
//
// The MPS-803 charset lives entirely outside the C64's address space -- it
// is read by the printer's own logic board, not by the 6510 -- so unlike
// the other ROM types it is never flashed into C64::mem.rom. This class is
// a plain data owner, not a SubComponent: it has no ticking, no events, and
// (like Paper and IecListener) is driven directly by its owner (Printer).
//
// ROM LAYOUT (measured from a real dump):
//
//   - The file is 3584 bytes = 512 glyphs x 7 bytes, one byte per dot row.
//   - Glyph index == character code (0-255).
//   - It is exactly two 256-glyph tables back to back: table 0 (Graphics,
//     the default set selected by secondary address 0 / CHR$(145)) at
//     offset 0, and table 1 (Business, selected by secondary address 7 /
//     CHR$(17)) at offset 0x0700 (1792).
//   - BIT ORDER within a row byte: bits 7..2 are the six dot columns, with
//     BIT 7 THE LEFTMOST DOT and bit 2 the rightmost. Bits 0 and 1 are
//     never set in the real ROM. Getting this backwards mirrors every
//     character -- see glyphRow()/dotSet() below, which are the only two
//     places this ordering is decoded.
//
// TWO DIFFERENT BIT CONVENTIONS COEXIST IN THIS CLASS -- do not conflate
// them:
//
//   - TEXT (glyphRow()/dotSet()/printGlyph(), above/below): a charset row
//     byte is HORIZONTAL. Bit 7 is the LEFTMOST dot of a 6-dot-wide row,
//     bit 2 the rightmost; bits 1..0 are unused. One byte = one dot ROW of
//     a character cell.
//   - BIT-IMAGE GRAPHICS (printGraphicsByte(), see Mps803.cpp): a graphics
//     byte on the wire is VERTICAL. Bit 7 marks the byte as graphics data
//     (always set); bits 0..6 are the seven dots of one dot COLUMN, and
//     BIT 0 IS THE TOP DOT, bit 6 the BOTTOM. One byte = one dot COLUMN of
//     the page, not a row of a character cell. Verified against the
//     worked example in the User's Guide's bit-image printing chapter.
//
// These two encodings are unrelated and are never mixed within a single
// function: printGlyph() only ever decodes the horizontal (text) form,
// printGraphicsByte() only ever decodes the vertical (bit-image) form.
class Mps803 {

public:

    // Which of the two 256-glyph character sets to read from
    enum class Table : u8 { Graphics = 0, Business = 1 };

    // Geometry, as measured from a real MPS-803 charset dump
    static constexpr isize glyphsPerTable = 256;
    static constexpr isize glyphRows = 7;                              // dot rows per glyph
    static constexpr isize glyphCols = 6;                              // dot columns per glyph
    static constexpr isize bytesPerGlyph = glyphRows;                  // 1 byte per row
    static constexpr isize tableSize = glyphsPerTable * bytesPerGlyph; // 1792
    static constexpr isize charsetSize = 2 * tableSize;                // 3584

    // Line geometry (per the User's Guide spec sheet): 80 character
    // columns per printed line, and although a
    // glyph is only glyphRows (7) dots tall, a printed line advances 12 dot
    // rows -- 6 lines per inch at the printer's 72 dpi vertical dot pitch,
    // not 7. The gap between the 5 unused rows is intentional whitespace
    // (line leading), not a bug.
    static constexpr isize columnsPerLine = 80;
    static constexpr isize lineSpacing = 12;

    // Bit-image (graphics) line spacing (User's Guide spec sheet):
    // 8 dot rows per line (9 LPI) while in bit-image mode, versus 12 dot
    // rows (6 LPI) in text/double-width mode above. Applied only while
    // isBitImage() is true -- see printGraphicsByte().
    static constexpr isize graphicsLineSpacing = 8;

    // Hard top margin, in dot rows, imposed on every page (half an inch
    // at the 72 dpi vertical pitch -- three text lines). The real MPS-803
    // is a friction-feed printer with no enforced margin: where printing
    // starts is wherever the user loaded the paper. The emulator
    // paginates the continuous roll for display, and this keeps the first
    // line of every page off the sheet's top edge, the way any sensibly
    // loaded sheet would look. Enforced by snapToPage(): a line may never
    // begin inside the margin, and a line whose 7 dot rows would straddle
    // the page boundary moves wholly onto the next page's printable area
    // instead of printing into its margin.
    static constexpr isize topMargin = 36;

private:

    // The paper this printer's glyphs will eventually be rendered onto.
    // Not used by this task; kept as a reference so the rendering task can
    // draw directly through this object without a second lookup.
    Paper &paper;

    // Raw ROM bytes, laid out exactly as the manufacturer's file: table 0
    // (Graphics) at [0, tableSize), table 1 (Business) at
    // [tableSize, charsetSize). Always exactly charsetSize bytes long, all
    // zero until a valid charset is flashed.
    std::vector<u8> charset = std::vector<u8>(charsetSize, 0);

    // True once a correctly-sized charset has been flashed via
    // flashCharset(). Distinct from "charset is all zero" so that hasRom()
    // can answer correctly even for a (hypothetical) all-zero real ROM.
    bool loaded = false;

    // Print-head state (this task): which character set is currently
    // selected (set at OPEN time via setChannel(), see the class comment
    // in Mps803.h about SA 0 vs SA 7), the current character column
    // (0 .. columnsPerLine - 1), and the absolute paper row (see Paper's
    // own comment: y is absolute across the whole roll) at which the top
    // of the current print line begins.
    Table table = Table::Graphics;
    isize col = 0;
    isize lineStart = topMargin;

    // Text-mode flags (this task). All four are in-stream, transient
    // modifiers of how the NEXT printable byte gets rendered -- distinct
    // from `table`, which can also be set persistently at OPEN time via
    // setChannel(). None of them survive a carriage return/line feed in
    // the real printer; that reset is out of scope here (it belongs to the
    // positioning/line-spacing task, which owns CR/LF handling) and is not
    // implemented yet.
    bool reverse = false;
    bool doubleWidth = false;

    // Quote mode toggles on '"' (0x22). While active, control codes that
    // would otherwise be obeyed are instead rendered as glyphs -- this
    // mirrors the C64's own PRINT-string quoting behavior, carried through
    // to the printer. It clears at end of line (also out of scope here,
    // owned by the CR/LF task).
    bool quote = false;

    // Bit-image (graphics) mode state (this task). Entered by CHR$(8),
    // left by CHR$(15) -- see write()'s dispatch comment for why CHR$(15)
    // now carries two independent effects. `dotCol` is the print head's
    // horizontal position measured in DOTS (0 .. Paper::width - 1), a
    // wholly separate axis from the text mode's `col` (measured in
    // character cells): bit-image bytes advance one dot column at a time,
    // not one character column. Reset to 0 every time CHR$(8) is received,
    // since cross-mode head positioning (CHR$(16)/CHR$(27)) is explicitly
    // out of scope for this task.
    bool bitImage = false;
    isize dotCol = 0;

    // State machine for CHR$(26) REPEAT: consumes a count byte then a data
    // byte and emits that dot column COUNT times (0 means 256). Only two
    // bytes are ever pending at once, so a tiny enum + one held count byte
    // is enough -- no queue needed.
    enum class PendingRepeat : u8 { None, Count, Data };
    PendingRepeat pendingRepeat = PendingRepeat::None;
    u8 repeatCount = 0;

    // State machine for the two multi-byte positioning commands (this
    // task): CHR$(16) POS (exactly two ASCII digit characters) and the
    // CHR$(27) ESC dot-address sequence (CHR$(27), CHR$(16), nH, nL --
    // the CHR$(27) byte itself is consumed by write() before this state
    // is entered). Both are single in-flight sequences, never nested or
    // overlapping, so one shared enum is enough; note EscExpect16 reuses
    // byte value 16 in a completely different role from the plain POS
    // command above -- see write()'s dispatch comment.
    enum class PendingCmd : u8 { None, PosDigit1, PosDigit2, EscExpect16, EscHigh, EscLow };
    PendingCmd pendingCmd = PendingCmd::None;
    u8 posDigit1 = 0;   // first POS digit's value (0-9), held while awaiting the second
    u8 escHigh = 0;     // dot-address sequence's nH byte, held while awaiting nL

public:

    explicit Mps803(Paper &paper) : paper(paper) { }

    Mps803 &operator=(const Mps803 &other)
    {
        charset = other.charset;
        loaded = other.loaded;
        table = other.table;
        col = other.col;
        lineStart = other.lineStart;
        reverse = other.reverse;
        doubleWidth = other.doubleWidth;
        quote = other.quote;
        bitImage = other.bitImage;
        dotCol = other.dotCol;
        pendingRepeat = other.pendingRepeat;
        repeatCount = other.repeatCount;
        pendingCmd = other.pendingCmd;
        posDigit1 = other.posDigit1;
        escHigh = other.escHigh;
        return *this;
    }

    // True once a charset has been successfully loaded
    bool hasCharset() const { return loaded; }

    // Wipes the charset back to its power-on/no-ROM state
    void clear();

    // Replaces the charset buffer. `len` MUST be exactly charsetSize; any
    // other size is logged and ignored (a short buffer would otherwise be
    // read out of bounds by glyphRow()/dotSet()).
    void flashCharset(const u8 *buf, isize len);

    // Raw access to the charset buffer, e.g. for hashing (romCRC32/romFNV64)
    // or for writing it back out to a file (saveRom).
    const u8 *data() const { return charset.data(); }
    isize size() const { return isize(charset.size()); }

    // Returns the raw ROM byte for dot row `row` (0 = top .. 6 = bottom) of
    // character code `code` in `table`. Bit order matches the ROM exactly
    // (see class comment): bit 7 is the leftmost of the six dot columns,
    // bit 2 is the rightmost. Returns 0 if no charset is loaded or `row` is
    // out of range.
    u8 glyphRow(Table table, u8 code, isize row) const;

    // Convenience wrapper around glyphRow(): true if the dot at column
    // `col` (0 = leftmost .. 5 = rightmost) of that row is set. This is the
    // one place the bit-7-is-leftmost ordering gets turned into a plain
    // left-to-right column index, so callers never have to think about ROM
    // bit order again.
    bool dotSet(Table table, u8 code, isize row, isize col) const;

    // Resets the print-head state (current column, current line, selected
    // character set) back to power-on. Does NOT touch the loaded charset --
    // the ROM stays in its socket across a reset, exactly like the C64's
    // own ROMs.
    void reset();

    // Selects the character set for subsequent write() calls, from the
    // secondary address given at OPEN time: SA 7 selects the Business
    // (upper/lowercase) set, any other SA selects the default Graphics
    // (uppercase/graphics) set. See the class comment for the SA 0 vs SA 7
    // rule. Does not otherwise affect print position.
    void setChannel(u8 sa);

    // Current character column (0 .. columnsPerLine - 1) the next write()
    // will render into.
    isize column() const { return col; }

    // Text-mode flag readers, for tests and for the next tasks to build on.
    bool isBusiness() const { return table == Table::Business; }
    bool isReverse() const { return reverse; }
    bool isDoubleWidth() const { return doubleWidth; }
    bool isQuote() const { return quote; }

    // True while in bit-image (graphics) mode, i.e. since the last CHR$(8)
    // that hasn't yet been cancelled by a CHR$(15).
    bool isBitImage() const { return bitImage; }

    // Current dot column (0 .. Paper::width - 1) the next graphics byte
    // will be written to. Distinct from column() (character cells) -- see
    // the class comment on `dotCol` above.
    isize dotColumn() const { return dotCol; }

    // Consumes one byte from the bus and renders it. With no charset
    // loaded, TEXT rendering prints nothing -- but BIT-IMAGE mode (this
    // task) needs no charset at all, so entering/using it works regardless
    // of whether flashCharset() was ever called.
    //
    // Dispatch is layered, checked in this order (see Mps803.cpp for the
    // full comment):
    //
    //   1. Mid-sequence bytes of a pending CHR$(26) REPEAT (count, then one
    //      data byte), CHR$(16) POS (two ASCII digits), or CHR$(27) ESC
    //      dot-address (CHR$(16), nH, nL) are consumed unconditionally.
    //   2. CHR$(15) is a SHARED TRIGGER POINT with two independent
    //      effects: cancel double width (text mode) AND leave bit-image
    //      mode. CR/LF/FF and the START of POS/ESC are also shared trigger
    //      points (this task) -- physical paper/head movements, not
    //      character rendering, so (like CHR$(15)) they are checked here,
    //      before the charset gate and the bit-image catch-all, so they
    //      work with no charset loaded and/or mid-bit-image-mode. Per the
    //      manual, CR additionally cancels bit-image mode. CR/LF/FF are
    //      NOT gated by quote mode (they are genuine end-of-line/paper-feed
    //      events); POS/ESC's start byte IS gated by quote, like every
    //      other in-band command.
    //   3. In bit-image mode, a byte with bit 7 set is graphics data --
    //      see printGraphicsByte(). Needs no charset.
    //   4. CHR$(26) begins a REPEAT sequence; only meaningful in bit-image
    //      mode.
    //   5. CHR$(8) enters bit-image mode; needs no charset.
    //   6. Any other byte reaching here while still in bit-image mode is
    //      neither graphics data nor a recognized bit-image control code --
    //      it is silently ignored and bit-image mode stays active (an
    //      explicit decision: see the class comment in Mps803.cpp).
    //   7. Everything below this point is TEXT-mode dispatch and DOES need
    //      a charset: quote-mode toggling, the remaining control codes
    //      (CHR$(17)/(145)/(18)/(146)/(14)), unrecognized low bytes
    //      (ignored), and finally printGlyph() for everything else.
    //
    // Throughout, quote mode's rule is unchanged: while active, control
    // codes are rendered as glyphs instead of being obeyed, except the '"'
    // byte itself, which always toggles quote mode AND is printed. Quote
    // mode also clears whenever a line ends -- both at CR (here) and at
    // the 80-column auto-wrap inside printGlyph().
    void write(u8 value);

    template <class T>
    void serialize(T &worker)
    {
        worker

        << charset
        << loaded
        << table
        << col
        << lineStart
        << reverse
        << doubleWidth
        << quote
        << bitImage
        << dotCol
        << pendingRepeat
        << repeatCount
        << pendingCmd
        << posDigit1
        << escHigh;
    }

private:

    // True if `value` is a control code this task (or a future one) knows
    // how to obey outside quote mode. Kept separate from write()'s switch
    // so quote mode can ask "would this normally be obeyed?" without
    // duplicating the dispatch table.
    bool isControlCode(u8 value) const;

    // Obeys one recognized control code (only ever called when
    // !quote, or for the '"' byte itself). Assumes isControlCode(value).
    void applyControlCode(u8 value);

    // Renders the glyph for `code`, in the currently selected table, into
    // the current character cell on paper, applying the current reverse/
    // double-width modifiers, then advances to the next column (by one
    // cell normally, two when double width is active), wrapping to a new
    // line at columnsPerLine.
    void printGlyph(u8 code);

    // Renders one bit-image (graphics) byte as a single vertical dot
    // column at the current dotCol, then advances dotCol by one. `value`
    // is decoded with the VERTICAL bit convention (see the class comment
    // at the top of this file): bit 7 is assumed set (it is what routed
    // the byte here) and is not itself rendered; bits 0..6 are the seven
    // dots of the column, bit 0 = TOP, bit 6 = BOTTOM.
    //
    // Right-edge wrap: when dotCol reaches Paper::width (480), this wraps
    // to dotCol = 0 and advances lineStart by graphicsLineSpacing (8 dot
    // rows) -- i.e. behaves like an implicit line feed at the paper edge,
    // rather than writing off-paper (Paper::setDot silently clamps/ignores
    // out-of-range x, which would silently drop data) or leaving dotCol
    // stuck past the edge (which would drop every subsequent column too).
    void printGraphicsByte(u8 value);

    // Advances the print head by `spacing` dot rows without printing
    // anything -- the shared implementation behind CR and LF. Also nudges
    // Paper itself (a value-false dot write at the new line's last row) so
    // Paper::rows() reflects the feed exactly like a physical paper
    // advance would, rather than only growing when a later glyph or
    // graphics byte happens to draw on that row.
    void advanceLine(isize spacing);

    // Clamps lineStart onto a page's printable area: pushed down past the
    // top margin, and pushed onto the NEXT page's printable area if the
    // line's glyphRows dot rows would cross the current page's bottom
    // edge. See topMargin above.
    void snapToPage();
};

}
