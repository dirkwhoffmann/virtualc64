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

#include "Archive.h"

Archive::Archive()
{
}

Archive::~Archive()
{
}

#if 0
int 
Archive::getItemWithName(char *pattern)
{
    unsigned i,j;
	int no_of_items = getNumberOfItems();
	
	assert(pattern != NULL);
	size_t pattern_length = strlen(pattern);
	
	// Iterate through all directory items
	for (i = 0; i < no_of_items; i++) {
	
		const char *name = getNameOfItem(i);
		size_t length    = strlen(name);
		
		// Pattern can't match if it has more characters
		if (length < pattern_length) {
			continue;
		}
		
		// Iterate through all characters of the search pattern
		for (j = 0; j < pattern_length; j++) {

			// '*' matches everything
			if (pattern[j] == '*')
				return i;
			
			// '?' matches an arbitrary character
			if (pattern[j] == '?') 
				continue;
			
			if (pattern[j] != name[j])
				break;
		}
		
		if (j == length) {
			return i;
		}
	}
	
	return -1;
}
#endif 

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

