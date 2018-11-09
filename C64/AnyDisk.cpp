/*!
 * @file        AnyDisk.cpp
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

#include "D64File.h"
#include "G64File.h"

AnyDisk *
AnyDisk::makeWithFile(const char *path)
{
    assert(path != NULL);
    
    if (D64File::isD64File(path)) {
        return D64File::makeWithFile(path);
    }
    if (G64File::isG64File(path)) {
        return G64File::makeWithFile(path);
    }
    return NULL;
}

int
AnyDisk::readHalftrack()
{
    int result;
    
    assert(tEof <= size);
    
    if (tFp < 0)
        return -1;
    
    // Get byte
    result = data[tFp++];
    
    // Check for end of file
    if (tFp == tEof)
        tFp = -1;
    
    return result;
}

const char *
AnyDisk::readHalftrackHex(size_t num)
{
    assert(sizeof(name) > 3 * num);
    
    for (unsigned i = 0; i < num; i++) {
        
        int byte = readHalftrack();
        if (byte == EOF) break;
        sprintf(name + (3 * i), "%02X ", byte);
    }
    
    return name;
}

void
AnyDisk::copyHalftrack(uint8_t *buffer, size_t offset)
{
    int byte;
    
    assert(buffer != NULL);
    
    seekHalftrack(0);
    
    while ((byte = readHalftrack()) != EOF) {
        buffer[offset++] = (uint8_t)byte;
    }
}
