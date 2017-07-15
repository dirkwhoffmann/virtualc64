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

/*! @class    D64Archive
 *  @brief    The D64Archive class declares the programmatic interface for a file in T64 format.
 */
class T64Archive : public Archive {

private:	

    //! @brief    The raw data of this archive.
    uint8_t *data;

    //! @brief    File size
    int size;

    /*! @brief    File pointer
        @details  An offset into the data array. 
     */
	int fp;
	
    /*! @brief    End of file position
     *  @details  Maximum value for fp. Do we really need this?
     */
	int fp_eof;
	
public:

    //
    //! @functiongroup Creating and destructing T64 archives
    //
    
    //! @brief    Standard constructor
    T64Archive();
    
    //! @brief    Standard destructor
    ~T64Archive();
		
    //! @brief    Returns true iff the specified file is a T64 file
    static bool isT64File(const char *filename);

    //! @brief    Creates a T64 archive from a T64 file located on disk.
    static T64Archive *archiveFromT64File(const char *filename);
    
    /*! @brief    Creates a T64 archive from another archive.
     *  @result A T64 archive that contains the first directory item of the other archive. 
     */
    static T64Archive *archiveFromArchive(Archive *otherArchive);


    //
    // Virtual functions from Container class
    //
    
    void dealloc();
    
    const char *getName();
    const unsigned short *getUnicodeName(size_t maxChars);
    ContainerType getType() { return T64_CONTAINER; }
    const char *getTypeAsString() { return "T64"; }
    
    bool fileIsValid(const char *filename);
    bool readFromBuffer(const uint8_t *buffer, unsigned length);
    unsigned writeToBuffer(uint8_t *buffer);
    
    
    //
    // Virtual functions from Archive class
    //
    
    int getNumberOfItems();
    const char *getNameOfItem(int n);
    const unsigned short *getUnicodeNameOfItem(int n, size_t maxChars);
    const char *getTypeOfItem(int n);
    uint16_t getDestinationAddrOfItem(int n);
    
    void selectItem(int n);
    int getByte();
    
    
    //
    // Custom methods
    //
    
    //! @brief Check if the file header contains information at the specific location
    bool directoryItemIsPresent(int n);

    /*! @brief    Check archive consistency and repair inconsistent information
     *  @details  This method can eliminate the following inconsistencies:
     *            number of files: some archives state falsely in their header that zero
     *            files are present. This value will be fixed.
     *            end loading address: Archives that are created with CONVC64 often contain
     *            a value of 0xC3C6, which is wrong (e.g., paradrd.t64). This value will be
     *            changed such that getByte() will read until the end of the physical file.
     * @result    true, if archive was consistent or could be repaired. false, if an inconsistency
     *            has been detected that could not be repaired.
     */
    bool repair();
};


#endif

