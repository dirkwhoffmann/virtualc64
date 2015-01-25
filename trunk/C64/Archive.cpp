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

#include "C64.h"

Archive::Archive()
{
    // writeProtection = false;
}

Archive::~Archive()
{
}

int 
Archive::getItemWithName(char *pattern)
{
	int i, j, length, pattern_length, no_of_items = getNumberOfItems();
	const char *name;
	
	assert(pattern != NULL);
	pattern_length = strlen(pattern);
	
	// Iterate through all directory items
	for (i = 0; i < no_of_items; i++) {
	
		name   = getNameOfItem(i);
		length = strlen(name);
		
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
		
		if (j == pattern_length) {
			return i;
		}
	}
	
	return -1;
}


