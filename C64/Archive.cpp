/*
 * (C) 2007 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
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

#include "T64Archive.h"
#include "D64Archive.h"
#include "PRGArchive.h"
#include "P00Archive.h"
#include "G64Archive.h"
#include "NIBArchive.h"

Archive::Archive()
{
}

Archive::~Archive()
{
}

Archive *
Archive::makeArchiveFromFile(const char *filename)
{
    assert(filename != NULL);
    
    if (T64Archive::isT64File(filename)) {
        return T64Archive::archiveFromT64File(filename);
    }
    if (D64Archive::isD64File(filename)) {
        return D64Archive::archiveFromD64File(filename);
    }
    if (PRGArchive::isPRGFile(filename)) {
        return PRGArchive::archiveFromPRGFile(filename);
    }
    if (P00Archive::isP00File(filename)) {
        return P00Archive::archiveFromP00File(filename);
    }
    if (G64Archive::isG64File(filename)) {
        return G64Archive::archiveFromG64File(filename);
    }
    if (NIBArchive::isNIBFile(filename)) {
        return NIBArchive::archiveFromNIBFile(filename);
    }
    return NULL;
}

int
Archive::getSizeOfItem(int n)
{
    int size = 0;

    if (n < getNumberOfItems()) {
        
        selectItem(n);
        while (getByte() != EOF)
            size++;
        }

    return size;
}

void
Archive::dumpDirectory()
{
    msg("Archive:           %s\n", getName());
    msg("-------\n");
    msg("  Path:            %s\n", getPath());
    msg("  Items:           %d\n", getNumberOfItems());

    for (unsigned i = 0; i < getNumberOfItems(); i++) {
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

