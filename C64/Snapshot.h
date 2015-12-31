/*
 * (C) 2009 - 2015 Dirk W. Hoffmann. All rights reserved.
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
#include "VIC_globals.h"

// Snapshot version number of this release
#define V_MAJOR 1
#define V_MINOR 4
#define V_SUBMINOR 2

// Forward declarations
class C64;

/*! @class Snapshot
 *  @brief The Snapshot class declares the programmatic interface for a file that contains an emulator snapshot (a frozen internal state).
 */
class Snapshot : public Container {
	
private:
	
	struct {
		
		//! Magic bytes ('V','C','6','4')
		char magic[4];
		
		//! Version number (V major.minor.subminor)
		uint8_t major;
		uint8_t minor;
        uint8_t subminor;
		
		// Screenshot
		struct { 	
			
			//! Image width and height
			uint16_t width, height;
		
			//! Screen buffer data 
			uint32_t screen[PAL_RASTERLINES * NTSC_PIXELS];
		
		} screenshot;
        
        // Size of internal state
        uint32_t size;

    } header;
	
    // Internal state data
    uint8_t *state;

	//! Date and time of snapshot creation
	time_t timestamp;
	
public:

	//! Constructor
	Snapshot();
	
	//! Destructor
	~Snapshot();
	
    //! Free allocated memory
    void dealloc();

    //! Allocate memory for storing internal state
    bool alloc(unsigned size);

    //! Returns true if file header matches
    static bool isSnapshot(const char *filename);

    //! Returns true if 'fileIsValid' and version number match
    static bool isSnapshot(const char *filename, int major, int minor, int subminor);
    
	//! Factory methods
	static Snapshot *snapshotFromFile(const char *filename);
	static Snapshot *snapshotFromBuffer(const uint8_t *buffer, unsigned size);
	
	//! Virtual functions from Container class
	bool fileIsValid(const char *filename);
	bool readFromBuffer(const uint8_t *buffer, unsigned length);
	unsigned writeToBuffer(uint8_t *buffer);
    unsigned sizeOnDisk() { return getHeaderSize() + getDataSize(); }
    
    ContainerType getType();
	const char *getTypeAsString();

    //! Returns size of header
    uint32_t getHeaderSize() { return sizeof(header); }

    //! Returns pointer to header data
    uint8_t *getHeader() { return (uint8_t *)&header; }

    //! Returns size of core data
    uint32_t getDataSize() { return header.size; }

    //! Returns pointer to core data
	uint8_t *getData() { return state; }

	//! Return timestamp
	time_t getTimestamp() { return timestamp; }

	//! Set timestamp
	void setTimestamp(time_t value) { timestamp = value; }

	//! Return PAL/NTSC flag
	// bool isPAL() { return (bool)header.isPAL; }
	
	//! Set PAL/NTSC flag
	// void setPAL(bool value) { header.isPAL = (uint8_t)value; }
	
	//! Returns true, if snapshot does not contain data yet
	bool isEmpty() { return timestamp == 0; }
	
	//! Return screen buffer
	unsigned char *getImageData() { return (unsigned char *)header.screenshot.screen; }

    //! Return image width
    unsigned getImageWidth() { return header.screenshot.width; }

    //! Return image height
    unsigned getImageHeight() { return header.screenshot.height; }

    //! Take screenshot
    void takeScreenshot(uint32_t *buf, bool pal);

};

#endif
	
