/*
 * (C) 2009 Dirk W. Hoffmann. All rights reserved.
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


#ifndef _SNAPSHOT_INC
#define _SNAPSHOT_INC

#include "basic.h"

class C64;

class Snapshot {
	
private:

	//! Size of a snapshot file in bytes
	static const int MAX_SNAPSHOT_SIZE = 2000000; // 1831822; 
	
	//! Version number (major)
	uint8_t major;
	
	//! Version number (minor)
	uint8_t minor;
	
	//! Binary snapshot data
	uint8_t data[MAX_SNAPSHOT_SIZE];
		
	//! Actual snapshot size
	int size;
	
public:
	
	//! Verify snapshot
	static bool isSnapshot(const char *filename); 

	//! Constructor
	Snapshot();
	
	//! Destructor
	~Snapshot();
			
	//! Load snapshot from file
	bool initWithContentsOfFile(const char *filename);
	
	//! Take snapshot from a virtual computer
	bool initWithContentsOfC64(C64 *c64);

	//! Save snapshot to file
	bool writeToFile(const char *filename);
	
	//! Initialize virtual computer with snapshot data
	bool writeToC64(C64 *c64);

};

#endif
	
