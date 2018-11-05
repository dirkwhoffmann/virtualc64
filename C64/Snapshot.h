/*!
 * @header      Snapshot.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
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


#ifndef _SNAPSHOT_INC
#define _SNAPSHOT_INC

#include "AnyC64File.h"

// Forward declarations
class C64;

// Snapshot header
typedef struct {
    
    //! @brief    Magic bytes ('V','C','6','4')
    char magic[4];
    
    //! @brief    Version number (V major.minor.subminor)
    uint8_t major;
    uint8_t minor;
    uint8_t subminor;
    
    //! @brief    Screenshot
    struct {
        
        //! @brief    Image width and height
        uint16_t width, height;
        
        //! @brief    Screen buffer data
        uint32_t screen[PAL_RASTERLINES * NTSC_PIXELS];
        
    } screenshot;
    
    //! @brief    Date and time of snapshot creation
    time_t timestamp;
    
} SnapshotHeader;


/*! @class   Snapshot
 *  @brief   The Snapshot class declares the programmatic interface for a file
 *           in V64 format (VirtualC64 snapshot files).
 */
class Snapshot : public AnyC64File {
    
    private:
    
    //! @brief    Header signature
    static const uint8_t magicBytes[];
    
    
    //
    //! @functiongroup Class methods
    //
    
    public:
    
    //! @brief    Returns true iff buffer contains a snapshot.
    static bool isSnapshot(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff buffer contains a snapshot of a specific version.
    static bool isSnapshot(const uint8_t *buffer, size_t length,
                           uint8_t major, uint8_t minor, uint8_t subminor);
    
    //! @brief    Returns true iff buffer contains a snapshot with a supported version number.
    static bool isSupportedSnapshot(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff buffer contains a snapshot with an outdated version number.
    static bool isUnsupportedSnapshot(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true if path points to a snapshot file.
    static bool isSnapshotFile(const char *path);
    
    //! @brief    Returns true if file points to a snapshot file of a specific version.
    static bool isSnapshotFile(const char *path, uint8_t major, uint8_t minor, uint8_t subminor);
    
    //! @brief    Returns true if file is a snapshot with a supported version number.
    static bool isSupportedSnapshotFile(const char *path);
    
    //! @brief    Returns true if file is a snapshot with an outdated version number.
    static bool isUnsupportedSnapshotFile(const char *path);
    
    
    //
    //! @functiongroup Creating and destructing
    //
    
    //! @brief    Standard Constructor
    Snapshot();

    //! @brief    Custom Constructor
    Snapshot(size_t capacity);

    //! @brief    Allocates memory for storing the emulator state.
    bool setCapacity(size_t size);
    
    //! @brief    Factory method
    static Snapshot *makeWithFile(const char *filename);
    
    //! @brief    Factory method
    static Snapshot *makeWithBuffer(const uint8_t *buffer, size_t size);

    //! @brief    Factory method
    static Snapshot *makeWithC64(C64 *c64);
    
    
    //
    //! @functiongroup Methods from AnyC64File
    //
    
    C64FileType type() { return V64_FILE; }
    const char *typeAsString() { return "V64"; }
    bool hasSameType(const char *filename);
    
    
    //
    //! @functiongroup Accessing snapshot properties
    //
    
    public:
    
    //! @brief    Returns pointer to header data
    SnapshotHeader *getHeader() { return (SnapshotHeader *)data; }
    
    //! @brief    Returns pointer to core data
    uint8_t *getData() { return data + sizeof(SnapshotHeader); }
    
    //! @brief    Returns the timestamp
    time_t getTimestamp() { return getHeader()->timestamp; }
        
    //! @brief    Returns a pointer to the screenshot data.
    unsigned char *getImageData() { return (unsigned char *)(getHeader()->screenshot.screen); }
    
    //! @brief    Returns the screenshot image width
    unsigned getImageWidth() { return getHeader()->screenshot.width; }
    
    //! @brief    Returns the screenshot image height
    unsigned getImageHeight() { return getHeader()->screenshot.height; }
    
    //! @brief    Stores a screenshot inside this snapshot
    void takeScreenshot(C64 *c64);
    
};

#endif

