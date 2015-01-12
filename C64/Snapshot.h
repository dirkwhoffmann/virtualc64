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

#include "Container.h"

class C64;

class Snapshot : public Container {
	
private:

	//! Size of a snapshot file in bytes
	static const int MAX_SNAPSHOT_SIZE = 850000; // 783342; 

private:
	
	struct {
		
		//! Magic bytes ('V','C','6','4')
		char magic[4];
		
		//! Version number (major)
		uint8_t major;
		
		//! Version number (minor)
		uint8_t minor;
		
		//! Is this a snapshot of a PAL machine or an NTSC machine?
		uint8_t isPAL;
		
		// Screenshot
		struct { 	
			
			//! Image width and height
			uint16_t width, height;
		
			//! Screen buffer data 
			uint32_t screen[512 * 512];
		
		} screenshot;
		
		//! Internal state
		uint8_t data[MAX_SNAPSHOT_SIZE];
	} fileContents;
	
	//! Date and time of snapshot creation
	time_t timestamp;
	
public:

	//! Constructor
	Snapshot();
	
	//! Destructor
	~Snapshot();
			
	//! Factory methods
	static Snapshot *snapshotFromFile(const char *filename);
	static Snapshot *snapshotFromBuffer(const void *buffer, unsigned size);
	
	//! Virtual functions from Container class
	bool fileIsValid(const char *filename);
	bool readFromBuffer(const void *buffer, unsigned length);
	bool writeToBuffer(void *buffer);
	unsigned sizeOnDisk();
		
	bool writeDataToFile(FILE *file, struct stat fileProperties);
	
	void dealloc();
    ContainerType getType();
	const char *getTypeAsString();
	
	//! Returns pointer to core data
	uint8_t *getData() { return fileContents.data; }

	//! Returns pointer to file contents
	uint8_t *getFileContents() { return (uint8_t *)&fileContents; }

	//! Returns size of file contents
	unsigned getFileContentsSize() { return sizeof(fileContents); }

	//! Return timestamp
	time_t getTimestamp() { return timestamp; }

	//! Set timestamp
	void setTimestamp(time_t value) { timestamp = value; }

	//! Return PAL/NTSC flag
	bool isPAL() { return (bool)fileContents.isPAL; }
	
	//! Set PAL/NTSC flag
	void setPAL(bool value) { fileContents.isPAL = (uint8_t)value; }
	
	//! Returns true, if snapshot does not contain data yet
	bool isEmpty() { return timestamp == 0; }

	//! Take screenshot
	// DEPRECATED. Already stored in snapshot	
	void takeScreenshot(uint32_t *buf) { memcpy(fileContents.screenshot.screen, buf, sizeof(fileContents.screenshot.screen)); }
	
	//! Return screen buffer
	unsigned char *getImageData() { return (unsigned char *)fileContents.screenshot.screen; }
};

#endif
	
