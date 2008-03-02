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


char *
Archive::getDirectory(int *no_of_bytes)
{
	int items, length, i;
	char *name;
	
	char header_template[32] = "\001\004\001\001\0\0\022\042\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\042\04000 2A";
	char footer_template[32] = "\001\001\0\0BLOCKS FREE.             \0\0";
	char entry_template[32];

	if (dir != NULL)
		free(dir);
	
	// Compute length and allocate memory
	items  = getNumberOfItems();
	length = 32 * (1 /* header */ + items + 1 /* footer */);
	
	if ((dir = (char *)malloc(length)) == NULL)
		return NULL;
		
	// write header 
    if ((name = getName()) != NULL) {
		int name_length = strlen(name);
		memcpy(&header_template[8], name, name_length < 16 ? name_length : 16);
	}
	memcpy(dir, header_template, 32);
	
	// write directory entries	
	for (i = 0; i < items; i++) {		
		int offset, j;
		char *item_name = getNameOfItem(i);
		int name_length = strlen(item_name);
		
		memset(entry_template, 32, 32);
		entry_template[0] = 0x01;
		entry_template[1] = 0x01;
		entry_template[31] = 0x00;
		
		// Number of blocks
		int blocks = getSizeOfItemInBlocks(i);
		entry_template[2] = blocks & 0xff;
		entry_template[3] = (blocks >> 8) & 0xff;
		offset = 5 + (blocks < 10) + (blocks < 100);
		
		// File name
		item_name = getNameOfItem(i);
		entry_template[offset] = '\"';
		for (j = 0; j < name_length && j < 16; j++) {
			entry_template[offset+j+1] = item_name[j];
		}
		entry_template[offset+j+1] = '\"';

		// File type
		entry_template[offset+19] = 'P';
		entry_template[offset+20] = 'R';
		entry_template[offset+21] = 'G';
		
		// Copy all over 
		debug("Entry: %s\n", entry_template);
		memcpy(32 * (i+1) + dir, entry_template, 32);
	}
	
	// write footer
	debug("writing footer\n");
	memcpy(32 * (i+1) + dir, footer_template, 32);
	
	*no_of_bytes = length;
	return dir;
}

int 
Archive::getItemWithName(char *pattern)
{
	int i, j, length, pattern_length, no_of_items = getNumberOfItems();
	char *name;
	
	assert(pattern != NULL);
	pattern_length = strlen(pattern);

	debug("Searching for %s\n", pattern);
	
	// Iterate through all directory items
	for (i = 0; i < no_of_items; i++) {
	
		name   = getNameOfItem(i);
		length = strlen(name);
		
		debug("Name of item: %s\n", name);
		 
		// Pattern can't match if it has more characters
		if (length < pattern_length) {
			debug("Pattern is smaller than file name.\n");
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
			debug("Heureka! %s matches %s\n", pattern, name);
		}
	}
	
	return -1;
}


