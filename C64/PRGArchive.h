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

#ifndef _PRGARCHIVE_INC
#define _PRGARCHIVE_INC

#include "Archive.h"

/*! @class D64Archive
 *  @brief The D64Archive class declares the programmatic interface for a file in PRG format.
 */
class PRGArchive : public Archive {

private:

	//! @brief The raw data of this archive.
    uint8_t *data;
		
    //! @brief File size
	int size;

    /*! @brief File pointer
     @discussion An offset into the data array. */
    int fp;

public:

    //
    //! @functiongroup Creating and destructing PRG archives
    //
    
    //! @brief Standard constructor.
    PRGArchive();
    
    //! @brief Standard destructor.
    ~PRGArchive();

    //! @brief Returns true iff the specified file is a PRG file
    static bool isPRGFile(const char *filename);

    //! @brief Creates a PRG archive from a PRG file.
    static PRGArchive *archiveFromPRGFile(const char *filename);

    /*! @brief Creates a PRG archive from another archive.
        @result A PRG archive that contains the first directory item of the other archive. */
    static PRGArchive *archiveFromArchive(Archive *otherArchive);


    //
    // Virtual functions from Container class
    //
    
    void dealloc();
    
    ContainerType getType() { return PRG_CONTAINER; }
    const char *getTypeAsString() { return "PRG"; }
    
    bool fileIsValid(const char *filename);
    bool readFromBuffer(const uint8_t *buffer, unsigned length);
    unsigned writeToBuffer(uint8_t *buffer);
    
    //
    // Virtual functions from Archive class
    //
    
    int getNumberOfItems();
    
    const char *getNameOfItem(int n);
    const char *getTypeOfItem(int n);
    uint16_t getDestinationAddrOfItem(int n);
    
    void selectItem(int n);
    int getByte();
};
#endif
