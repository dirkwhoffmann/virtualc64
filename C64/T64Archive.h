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

#ifndef _T64ARCHIVE_INC
#define _T64ARCHIVE_INC

#include "Archive.h"

/*! @class D64Archive
 *  @brief The D64Archive class declares the programmatic interface for a file in T64 format.
 */
class T64Archive : public Archive {

private:	

    //! @brief The raw data of this archive.
    uint8_t *data;

    //! @brief File size
    int size;

    /*! @brief File pointer
        @discussion An offset into the data array. */
	int fp;
	
    /*! @brief End of file position
        @discussion Maximum value for fp. Do we really need this? */
	int fp_eof;
	
	//! @brief Don't know. Do we really need this?
	bool directoryItemIsPresent(int n);

public:

    //
    //! @functiongroup Creating and destructing T64 archives
    //
    
    //! @brief Standard constructor.
    T64Archive();
    
    //! @brief Standard destructor.
    ~T64Archive();
		
    //! @brief Returns true iff the specified file is a T64 file
    static bool isT64File(const char *filename);

    //! @brief Creates a T64 archive from a T64 file located on disk.
    static T64Archive *archiveFromT64File(const char *filename);
    
    /*! @brief Creates a T64 archive from another archive.
     @result A T64 archive that contains the first directory item of the other archive. */
    static T64Archive *archiveFromArchive(Archive *otherArchive);


    //
    // Virtual functions from Container class
    //
    
    void dealloc();
    
    const char *getName();
    ContainerType getType() { return T64_CONTAINER; }
    const char *getTypeAsString() { return "T64"; }
    
    bool fileIsValid(const char *filename);
    bool readFromBuffer(const uint8_t *buffer, unsigned length);
    unsigned writeToBuffer(uint8_t *buffer);
    
    //
    // Virtual functions from Archive class
    //
    
    int getNumberOfItems();
    // int getSizeOfItem(int n);
    
    const char *getNameOfItem(int n);
    const char *getTypeOfItem(int n);
    uint16_t getDestinationAddrOfItem(int n);
    
    void selectItem(int n);
    int getByte();
};


#endif

