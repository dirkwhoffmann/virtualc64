/*!
 * @header      AnyDisk.h
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

#ifndef _ANYDISK_INC
#define _ANYDISK_INC

#include "AnyArchive.h"

/*! @class    AnyDisk
 *  @brief    This class adds an API to AnyArchive for handling file formats
 *            that represent a physical floppy disk.
 */
class AnyDisk : public AnyArchive {
    
protected:
    
    /*! @brief    File pointer
     *  @details  An offset into the data range of the selected track.
     */
    long tFp = -1;
    
    /*! @brief    End of file position
     *  @details  This value equals the last valid offset plus 1
     */
    long tEof = -1;
    
public:
    
    //
    //! @functiongroup Creating and destructing objects
    //
    
    /*! @brief    Factory method
     *  @return   A D64File or G64File,
     *            depending on the type of the specified file.
     */
    static AnyDisk *makeObjectWithFile(const char *filename);
    
    
    //
    //! @functiongroup Selecting a track or halftrack
    //
    
    //! @brief    Returns the number of halftracks of the represented disk.
    virtual int numberOfHalftracks() { return 0; }
    virtual int numberOfTracks() { return (numberOfHalftracks() + 1) / 2; }
    
    /*! @brief    Selects the active track
     *  @details  All track related methods work on the active item.
     */
    virtual void selectHalftrack(Halftrack ht) { };
    virtual void selectTrack(Track t) { selectHalftrack(2 * t - 1); }
        
    
    //
    //! @functiongroup Reading data from a track
    //
    
    //! @brief    Returns the size of the selected haltrack in bytes
    virtual size_t getSizeOfHalftrack() { return 0; }
    virtual size_t getSizeOfTrack() { return getSizeOfHalftrack(); }
                                                
    //! @brief    Moves the file pointer to the specified offset.
    /*! @details  Use seek(0) to return to the beginning of the selected track.
     */
    virtual void seekHalftrack(long offset) { }
    virtual void seekTrack(long offset) { seekHalftrack(offset); }
    
    /*! @brief    Reads a byte from the selected track.
     *  @return   EOF (-1) if all bytes have been read in.
     */
    virtual int readHalftrack();
    virtual int readTrack() { return readHalftrack(); }
    
    /*! @brief    Reads multiple bytes in form of a hex dump string.
     *  @param    Number of bytes ranging from 1 to 85.
     */
    virtual const char *readHalftrackHex(size_t num);
    virtual const char *readTrackHex(size_t num) { return readHalftrackHex(num); }
    
    //! @brief    Copies the selected track into the specified buffer.
    virtual void copyHalftrack(uint8_t *buffer, size_t offset = 0);
    virtual void copyTrack(uint8_t *buffer, size_t offset = 0) { copyHalftrack(buffer, offset); }
};

#endif
