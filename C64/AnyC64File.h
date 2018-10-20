/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, All rights reserved
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
#include "C64_types.h"

/*! @class    AnyC64File
 *  @brief    Base class for all supported file types.
 *  @details  Provides the basic functionality for reading and writing files.
 */
class AnyC64File : public VC64Object {
    
protected:
	     
    //! @brief    The physical name (full path) of this file.
    char *path = NULL;
    
    //! @brief    The raw data of this file.
    uint8_t *data = NULL;
    
    //! @brief    The size of this file in bytes.
    size_t size = 0;
    
    /*! @brief    The logical name of the container.
     *  @details  Some archives store a logical name in their header section.
     *            If they don't store a name, the raw filename is used (path
     *            and extension stripped off).
     */
    char name[256];
    
    /*! @brief    Unicode representation of the logical name.
     *  @seealso  getUnicodeName
     */
    unsigned short unicode[256];
    
protected:
    
    /*! @brief    Checks the header signature of a buffer.
     *  @details  This functions is used to determine if the buffer contains the
     *            binary representation of a special file type, e.g., the
     *            representation of a T64 file.
     *  @param    buffer    Pointer to buffer, must not be NULL
     *  @param    length    Length of the buffer
     *  @param    header    Expected byte sequence, terminated by EOF
     *  @return   Returns   true iff magic bytes match.
     *  @seealso  Container::typeOfBuffer
     */
    static bool checkBufferHeader(const uint8_t *buffer, size_t length,
                                  const uint8_t *header);

public:
    
    //
    //! @functiongroup Creating and destructing containers
    //

    //! @brief    Constructor
    AnyC64File();

    //! @brief    Destructor
    virtual ~AnyC64File();
    
    //! @brief    Frees the memory allocated by this object.
    virtual void dealloc();

    
    //
    //! @functiongroup Accessing container attributes
    //
    
    //! @brief    Returns the type of this container.
    virtual ContainerType type() { return UNKNOWN_CONTAINER_FORMAT; }
    
    //! @brief      Returns the string representation of the container's type.
    /*! @details    E.g., a T64 container returns "T64"
     */
    virtual const char *typeAsString() { return "???"; }

	//! @brief    Returns the physical name of this container.
    const char *getPath() { return path ? path : ""; }

    //! @brief    Sets the physical name of this container.
    void setPath(const char *path);

    //! @brief    Returns the logical name of this container.
    virtual const char *getName();

    /*! @brief    Returns the logical name as unicode character array.
     *  @details  The provides unicode format is compatible with font C64ProMono
     *            which is used, e.g., in the mount dialogs preview panel.
     */
    virtual const unsigned short *getUnicodeName();

    //! @brief    Sets the logical name.
    // void setName(const char *name);
	
    
    //
    //! @functiongroup Serializing a container
    //
    
    //! @brief    Required buffer size for this container
    size_t sizeOnDisk() { return writeToBuffer(NULL); }

    /*! @brief    Returns true iff this container has the same type as the
     *            container stored in the specified file.
     */
    virtual bool hasSameType(const char *filename) { return false; }

    /*! @brief    Read container contents from a memory buffer.
     *  @param    buffer The address of a binary representation in memory.
     *  @param    length The size of the binary representation.
     */
    virtual bool readFromBuffer(const uint8_t *buffer, size_t length) { return false; }
	
    /*! @brief    Read container contents from a file.
     *  @details  This function requires no custom implementation. It first reads in the file contents 
     *            in memory and invokes readFromBuffer afterwards. 
     *  @param    filename The name of a file containing a binary representation.
     */
	bool readFromFile(const char *filename);

    /*! @brief    Write container contents into a memory buffer.
     *  @details  If a NULL pointer is passed in, a test run is performed.
     *            Test runs are performed to determine the size of the
     *            container on disk.
     *   @param   buffer The address of the buffer in memory.
     */
	virtual size_t writeToBuffer(uint8_t *buffer);

    /*! @brief    Write container contents to a file.
     *  @details  This function requires no custom implementation. It invokes
     *            writeToBuffer first and writes the data to disk afterwards.
     *  @param    filename The name of a file to be written.
     */
	bool writeToFile(const char *filename);
};

#endif
