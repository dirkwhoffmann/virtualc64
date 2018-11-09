/*!
 * @file        TAPFile.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "TAPFile.h"

const uint8_t TAPFile::magicBytes[] = {
    0x43, 0x36, 0x34, 0x2D, 0x54, 0x41, 0x50, 0x45, 0x2D, 0x52, 0x41, 0x57, 0x00 };

TAPFile::TAPFile()
{
    setDescription("TAPFile");
    data = NULL;
    dealloc();
}

TAPFile *
TAPFile::makeWithBuffer(const uint8_t *buffer, size_t length)
{
    TAPFile *tape = new TAPFile();
    
    if (!tape->readFromBuffer(buffer, length)) {
        delete tape;
        return NULL;
    }
    
    return tape;
}

TAPFile *
TAPFile::makeWithFile(const char *filename)
{
    TAPFile *tape = new TAPFile();
    
    if (!tape->readFromFile(filename)) {
        delete tape;
        return NULL;
    }
    
    return tape;
}

bool
TAPFile::isTAPBuffer(const uint8_t *buffer, size_t length)
{
    if (length < 0x15) return false;
    return checkBufferHeader(buffer, length, magicBytes);
}

bool
TAPFile::isTAPFile(const char *filename)
{
    assert (filename != NULL);
    
    if (!checkFileSuffix(filename, ".TAP") && !checkFileSuffix(filename, ".tap") &&
        !checkFileSuffix(filename, ".T64") && !checkFileSuffix(filename, ".t64"))
        return false;
    
    if (!checkFileSize(filename, 0x15, -1))
        return false;
    
    if (!checkFileHeader(filename, magicBytes))
        return false;
    
    return true;
}

void
TAPFile::dealloc()
{
    fp = -1;
}

const char *
TAPFile::getName()
{
    unsigned i;
    
    for (i = 0; i < 17; i++) {
        name[i] = data[0x08+i];
    }
    name[i] = 0x00;
    return name;
}

bool
TAPFile::readFromBuffer(const uint8_t *buffer, size_t length)
{
    if (!AnyC64File::readFromBuffer(buffer, length))
        return false;
    
    int l = LO_LO_HI_HI(data[0x10], data[0x11], data[0x12], data[0x13]);
    if (l + 0x14 /* Header */ != size) {
        warn("Size mismatch! Archive should have %d data bytes, found %d\n", l, size - 0x14);
    }
        
    return true;
}

