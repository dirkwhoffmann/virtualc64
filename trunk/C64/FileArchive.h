/*
 * Author: Dirk W. Hoffmann, www.dirkwhoffmann.de
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

#ifndef _FILEARCHIVE_INC
#define _FILEARCHIVE_INC

#include "Archive.h"

/*! @class FileArchive
 *  @brief The FileArchive class declares the programmatic interface for a file that does not match any of the standard formats.
    @discussion If a file does not match any of the standard formats, each byte is interpreted as raw data and is loaded at the standard memory location.
 */
class FileArchive : public Archive {

private:
	//! @brief The raw data of this archive.
    uint8_t *data;

    /*! @brief File pointer
        @discussion An offset into the data array. */
	int fp;
		
    //! @brief File size
	int size;

public:

    //! @brief Standard constructor.
    FileArchive();
    
    //! @brief Standard destructor.
    ~FileArchive();
    
    //! @brief Returns true if filename points to a loadable file
    static bool isAcceptableFile(const char *filename);
    
    //! @brief Creates an archive from a loadable file.
    static FileArchive *archiveFromRawFiledata(const char *filename);
    
    //
    // Virtual functions from Container class
    //
    
    void dealloc();
    
    // const char *getName();
    ContainerType getType() { return FILE_CONTAINER; }
    const char *getTypeAsString() { return "FILE"; }
    
    bool fileIsValid(const char *filename);
    bool readFromBuffer(const uint8_t *buffer, unsigned length);
    // unsigned writeToBuffer(uint8_t *buffer);
    
    //
    // Virtual functions from Archive class
    //
    
    int getNumberOfItems();
    
    const char *getNameOfItem(int n);
    const char *getTypeOfItem(int n);
    int getSizeOfItem(int n);
    uint16_t getDestinationAddrOfItem(int n);
    
    void selectItem(int n);
    int getByte();
};
#endif
