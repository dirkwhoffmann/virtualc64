/*!
 * @file        AnyArchive.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
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

#include "T64File.h"
#include "D64File.h"
#include "PRGFile.h"
#include "P00File.h"
#include "G64File.h"

AnyArchive *
AnyArchive::makeArchiveWithFile(const char *path)
{
    assert(path != NULL);
    
    if (T64File::isT64File(path)) {
        return T64File::makeT64ArchiveWithFile(path);
    }
    if (D64File::isD64File(path)) {
        return D64File::makeObjectWithFile(path);
    }
    if (PRGFile::isPRGFile(path)) {
        return PRGFile::makePRGArchiveWithFile(path);
    }
    if (P00File::isP00File(path)) {
        return P00File::makeP00ArchiveWithFile(path);
    }
    if (G64File::isG64File(path)) {
        return G64File::makeG64ArchiveWithFile(path);
    }
    return NULL;
}

const char *
AnyArchive::getNameOfItem(unsigned n)
{
    assert(n < numberOfItems());
    return "FILE";
}

const unsigned short *
AnyArchive::getUnicodeNameOfItem(unsigned n)
{
    const char *name = getNameOfItem(n);
    translateToUnicode(name, unicode, 0xE000, sizeof(unicode) / 2);
    return unicode;
}

size_t
AnyArchive::getSizeOfItem(unsigned n)
{
    int size = 0;
    
    selectItem(n);
    while (getByte() != EOF)
        size++;

    return size;
}

void
AnyArchive::dumpDirectory()
{
    int numItems = numberOfItems();
    
    msg("Archive:           %s\n", getName());
    msg("-------\n");
    msg("  Path:            %s\n", getPath());
    msg("  Items:           %d\n", numItems);

    for (unsigned i = 0; i < numItems; i++) {
        msg("  Item %2d:      %s (%d bytes, load address: %d)\n",
                i, getNameOfItem(i), getSizeOfItem(i), getDestinationAddrOfItem(i));
        msg("                 ");
        selectItem(i);
        for (unsigned j = 0; j < 8; j++) {
            int byte = getByte();
            if (byte != -1)
                msg("%02X ", byte);
        }
        msg("\n");
    }
}

void
AnyArchive::flash(uint8_t *buffer)
{
    uint16_t addr = getDestinationAddr();
    debug("Flashing to %04X\n", addr);
    AnyC64File::flash(buffer, (size_t)addr);
}

const char *
AnyArchive::hexDump(unsigned n, size_t offset, size_t num)
{
    if (n >= numberOfItems()) return "???";
    
    selectItem(n);
    return AnyC64File::hexDump(offset, num);
}

