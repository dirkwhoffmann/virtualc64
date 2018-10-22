/*!
 * @header      AnyC64File.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/*
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

#ifndef _ANYC64FILE_INC
#define _ANYC64FILE_INC

#include "VC64Object.h"

/*! @class    AnyC64File
 *  @brief    Base class for all supported file types.
 *  @details  Provides the basic functionality for reading and writing files.
 */
class AnyC64File : public VC64Object {
    
protected:
	     
    //! @brief    The physical name (full path) of this file.
    char *path = NULL;
    
    /*! @brief    The logical name of this file.
     *  @details  Some archives store a logical name in their header section.
     *            If they don't store a name, the raw filename is used (path
     *            and extension stripped off).
     */
    char name[256];
    
    /*! @brief    Unicode representation of the logical name.
     *  @seealso  getUnicodeName
     */
    unsigned short unicode[256];
    
    //! @brief    The raw data of this file.
    uint8_t *data = NULL;
    
    //! @brief    The size of this file in bytes.
    size_t size = 0;
    
    /*! @brief    File pointer
     *  @details  An offset into the data array.
     */
    long fp = -1;
    
    /*! @brief    End of file position
     *  @details  This value equals the last valid offset plus 1
     */
    long eof = -1;
    
    
protected:
    
    /*! @brief    Checks the header signature of a buffer.
     *  @details  This functions is used to determine if the buffer contains the
     *            binary representation of a special file type, e.g., the
     *            representation of a T64 file.
     *  @param    buffer    Pointer to buffer, must not be NULL
     *  @param    length    Length of the buffer
     *  @param    header    Expected byte sequence, terminated by EOF
     *  @return   Returns   true iff magic bytes match.
     */
    static bool checkBufferHeader(const uint8_t *buffer, size_t length,
                                  const uint8_t *header);

    
    //
    //! @functiongroup Creating and destructing objects
    //
    
public:
    
    //! @brief    Constructor
    AnyC64File();

    //! @brief    Destructor
    virtual ~AnyC64File();
    
    //! @brief    Frees the memory allocated by this object.
    virtual void dealloc();

    
    //
    //! @functiongroup Accessing file attributes
    //
    
    //! @brief    Returns the type of this file.
    virtual C64FileType type() { return UNKNOWN_FILE_FORMAT; }
    
    /*! @brief      Returns a string representation of the file type.
     *  @details    E.g., a T64 file returns "T64".
     */
    virtual const char *typeAsString() { return ""; }

	//! @brief    Returns the physical name of this file.
    const char *getPath() { return path ? path : ""; }

    //! @brief    Sets the physical name of this file.
    void setPath(const char *path);

    //! @brief    Returns the logical name of this file.
    virtual const char *getName() { return name; }

    /*! @brief    Returns the logical name as unicode character array.
     *  @details  The provides unicode format is compatible with font C64ProMono
     *            which is used, e.g., in the mount dialogs preview panel.
     */
    const unsigned short *getUnicodeName();
	
    
    //
    //! @functiongroup Reading data from the file
    //
    
    /*! @brief    Returns the number of bytes in this file.
     *  @details  After getSize() calls to read(), EOF is returned.
     */
    virtual size_t getSize() { return size; }

    //! @brief    Moves the file pointer to the specified offset.
    /*! @details  Use seek(0) to return to the beginning of the file.
     */
    virtual void seek(long offset);
    
    /*! @brief    Reads a byte.
     *  @return   EOF (-1) if the end of file has been reached.
     */
    virtual int read();

    /*! @brief    Reads multiple bytes in form of a hex dump string.
     *  @param    Number of bytes ranging from 1 to 85.
     */
    const char *readHex(size_t num = 1);

    /*! @brief    Uses getByte() to copy the file into the C64 memory.
     *  @param    buffer must be a pointer to RAM or ROM
     */
    virtual void flash(uint8_t *buffer, size_t offset = 0);

    
    //
    //! @functiongroup Serializing
    //
    
    //! @brief    Required buffer size for this file
    size_t sizeOnDisk() { return writeToBuffer(NULL); }

    /*! @brief    Returns true iff this file has the same type as the
     *            file stored in the specified file.
     */
    virtual bool hasSameType(const char *filename) { return false; }

    /*! @brief    Reads the file contents from a memory buffer.
     *  @param    buffer The address of a binary representation in memory.
     *  @param    length The size of the binary representation.
     */
    virtual bool readFromBuffer(const uint8_t *buffer, size_t length);
	
    /*! @brief    Reads the file contents from a file.
     *  @details  This function requires no custom implementation. It first
     *            reads in the file contents in memory and invokes
     *            readFromBuffer afterwards.
     *  @param    filename The name of a file on disk.
     */
	bool readFromFile(const char *filename);

    /*! @brief    Writes the file contents into a memory buffer.
     *  @details  If a NULL pointer is passed in, a test run is performed. Test
     *            runs are performed to determine the size of the file on disk.
     *   @param   buffer The address of the buffer in memory.
     */
	virtual size_t writeToBuffer(uint8_t *buffer);

    /*! @brief    Writes the file contents to a file.
     *  @details  This function requires no custom implementation. It invokes
     *            writeToBuffer first and writes the data to disk afterwards.
     *  @param    filename The name of a file to be written.
     */
	bool writeToFile(const char *filename);
};

#endif
