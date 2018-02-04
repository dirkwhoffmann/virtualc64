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

#ifndef _G64ARCHIVE_INC
#define _G64ARCHIVE_INC

#include "Archive.h"

/*! @class    G64Archive
 *  @brief    The G64Archive class declares the programmatic interface for a file in G64 format.
 */
class G64Archive : public Archive {

private:	

    //! @brief    Header signature
    static const uint8_t magicBytes[];
    
    //! @brief    The raw data of this archive.
    uint8_t *data;

    //! @brief Size of G64 file
    size_t size;

    /*! @brief    File pointer
     *  @details  An offset into the data array. 
     */
	long fp;
	
    /*! @brief    End of file position
     *  @details  This value equals the last valid offset plus 1 
     */
	long fp_eof;
	
public:

    //
    //! @functiongroup Creating and destructing G64 archives
    //
    
    //! @brief    Standard constructor
    G64Archive();
    
    //! @brief    Standard destructor
    ~G64Archive();
		
    //! @brief    Returns true iff buffer contains a G64 file
    static bool isG64(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff the specified file is a G64 file
    static bool isG64File(const char *filename);

    //! @brief    Creates a G64 archive from a G64 file located on disk.
    static G64Archive *archiveFromG64File(const char *filename);
    
    
    //
    // Virtual functions from Container class
    //
    
    void dealloc();
    
    const char *getName();
    // const char *getNameAsPETString();
    ContainerType type() { return G64_CONTAINER; }
    const char *typeAsString() { return "G64"; }
    
    bool hasSameType(const char *filename);
    bool readFromBuffer(const uint8_t *buffer, size_t length);
    size_t writeToBuffer(uint8_t *buffer);
    
    
    //
    // Virtual functions from Archive class
    //
    
    int getNumberOfItems();
    size_t getSizeOfItem(int n);
    const char *getNameOfItem(int n);
    const unsigned short *getUnicodeNameOfItem(int n, size_t maxChars);
    const char *getTypeOfItem(int n);
    uint16_t getDestinationAddrOfItem(int n) { return 0; }
    
    void selectItem(int n);
    int getByte();

    
    uint32_t getStartOfItem(int n);
};


#endif

