#include "C64.h"

Archive::Archive()
{
	dir = NULL;
}

Archive::~Archive()
{
	if (dir)
		free(dir);
}

int 
Archive::getItemWithName(char *pattern)
{
	int i, j, length, pattern_length, no_of_items = getNumberOfItems();
	char *name;
	
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


