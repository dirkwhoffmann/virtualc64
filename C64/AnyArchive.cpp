/*!
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

#include "T64File.h"
#include "D64File.h"
#include "PRGFile.h"
#include "P00File.h"
#include "G64File.h"

Archive::Archive()
{
}

Archive::~Archive()
{
}

Archive *
Archive::makeArchiveWithFile(const char *path)
{
    assert(path != NULL);
    
    if (T64Archive::isT64File(path)) {
        return T64Archive::makeT64ArchiveWithFile(path);
    }
    if (D64Archive::isD64File(path)) {
        return D64Archive::makeD64ArchiveWithFile(path);
    }
    if (PRGArchive::isPRGFile(path)) {
        return PRGArchive::makePRGArchiveWithFile(path);
    }
    if (P00Archive::isP00File(path)) {
        return P00Archive::makeP00ArchiveWithFile(path);
    }
    if (G64Archive::isG64File(path)) {
        return G64Archive::makeG64ArchiveWithFile(path);
    }
    return NULL;
}

const char *
Archive::getNameOfItem(unsigned n)
{
    assert(n < getNumberOfItems());
    return "FILE";
}

const unsigned short *
Archive::getUnicodeNameOfItem(unsigned n)
{
    const char *name = getNameOfItem(n);
    translateToUnicode(name, unicode, 0xE000, sizeof(unicode) / 2);
    return unicode;
}

size_t
Archive::getSizeOfItem(unsigned n)
{
    int size = 0;
    
    selectItem(n);
    while (getByte() != EOF)
        size++;

    return size;
}

void
Archive::dumpDirectory()
{
    int numberOfItems = getNumberOfItems();
    
    msg("Archive:           %s\n", getName());
    msg("-------\n");
    msg("  Path:            %s\n", getPath());
    msg("  Items:           %d\n", numberOfItems);

    for (unsigned i = 0; i < numberOfItems; i++) {
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
Archive::flash(unsigned item, uint8_t *buffer)
{
    assert(buffer != NULL);
    
    uint16_t addr = getDestinationAddrOfItem(item);
    debug("Flashing item %d to %p with offset %04X\n", item, buffer, addr);
    
    selectItem(item);
    while (1) {
        int data = getByte();
        if (data < 0) break;
        buffer[addr] = (uint8_t)data;
        if (addr == 0xFFFF) break;
        addr++;
    }
}

const char *
Archive::byteStream(unsigned n, size_t offset, size_t num)
{
    if (n >= getNumberOfItems()) {
        return "???";
    }
    
    selectItem(n);
    skip((unsigned)offset);
    
    assert(sizeof(name) > 3 * num);
    
    for (unsigned i = 0; i < num; i++) {
        
        int byte = getByte();
        if (byte == -1) break;
        sprintf(name + (3 * i), "%02X ", byte);
    }
    
    return name;
}

