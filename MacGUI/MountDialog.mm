/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

#import "MyDocument.h"

@implementation MountDialog

- (void) initialize:(Archive *)a
{
	assert(a != NULL);
	
	int numberOfFiles = a->getNumberOfItems();
	
	archive = a;
	[archiveName setStringValue:[NSString stringWithFormat:@"%s", archive->getName()]];
	[archiveType setStringValue:[NSString stringWithFormat:@"%s (%d %s)", archive->getTypeOfContainer(), 
		numberOfFiles, numberOfFiles == 1 ? "file" : "files"]];
	if (archive->getNumberOfItems() == 0) {
		[mountButton setEnabled:false];
		[flashButton setEnabled:false];
	} else {
		[mountButton setEnabled:archive->isMountable()];
		[flashButton setEnabled:archive->isFlashable()];
	}
	[directory reloadData];
}

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	// SEVERE PROBLEM HERE:
	// THIS FUNCTION IS INVOKED BEFORE ARCHIVE IS INITIALIZED
	// IF THE VALUE HAPPENS TO BE GREATER ZERO, WE'LL CRASH HERE!!!!
	if (archive == NULL) {
		return 0;
	} else {
		return archive->getNumberOfItems();
	}
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	if ([[aTableColumn identifier] isEqual:@"filename"]) {
		return [NSString stringWithFormat:@"%s", archive->getNameOfItem(row)];
	}
	if ([[aTableColumn identifier] isEqual:@"filesize"]) {
		return [NSNumber numberWithInt:archive->getSizeOfItem(row)];
	}
	if ([[aTableColumn identifier] isEqual:@"filetype"]) {
		return [NSString stringWithFormat:@"%s", archive->getTypeOfItem(row)];
	}
	return @"???";
}

- (int)getSelectedFile
{
	int row = [directory selectedRow];
	if (row < 0) 
		return 0; // flash first file by default
	else
		return row;
}


@end
