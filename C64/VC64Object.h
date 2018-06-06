/*!
 * @header      VC64Object.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2018 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _VC64OBJECT_INC
#define _VC64OBJECT_INC

#include "basic.h"
#include "C64_types.h"

/*! @brief    Common functionality of all VirtualC64 objects.
 *  @details  This class defines the base functionality of all objects such as 
 *            printing debug messages.
 */
class VC64Object {

private:
    
    /*! @brief    Default debug level
     *  @details  On object creation, this value is used as debug level.
     */
    static unsigned defaultDebugLevel;

    /*! @brief    Debug level
     *  @details  Debug messages are written either to console or a logfile.
     *            Set to 0 to omit messages.
     */
    unsigned debugLevel;

    /*! @brief    Stores how many trace messages are left to be printed
     *  @details  If positive, this value is decremented in every call to tracingEnabled()
     *  @details  A negative value indicates that tracing should continue forever
     */
     int traceCounter;

    /*! @brief    Textual description of this object
     *  @details  Most debug output methods preceed their output with this string.
     *  @note     The default value is NULL. In that case, no prefix is printed.
     */
    const char *description;

public:

    //! Constructor
    VC64Object();
    
    //! Destructor
    virtual ~VC64Object();

    
    //
    //! @functiongroup Initializing the component
    //
    
    //! @brief    Sets the default debug level.
    static void setDefaultDebugLevel(unsigned level) { defaultDebugLevel = level; }

    //! @brief    Changes the debug level for a specific object.
    void setDebugLevel(unsigned level) { debugLevel = level; }

    //! @brief    Returns the textual description.
    const char *getDescription() { return description ? description : ""; }

    //! @brief    Assigns a textual description.
    void setDescription(const char *desc) { description = desc; }

    
    //
    //! @functiongroup Debugging the component
    //
    
    //! @brief    Returns true iff trace mode is enabled.
    bool tracingEnabled();
    
    //! @brief    Sets the trace counter.
    void setTraceCounter(int count) { traceCounter = count; }
    
    //! @brief    Starts tracing.
    void startTracing() { setTraceCounter(-1); }

    //! @brief    Stops tracing.
    void stopTracing() { setTraceCounter(0); }
    
    
    //
    //! @functiongroup Printing messages to console
    //
    
    //! @brief    Prints message to console or a log file.
    void msg(const char *fmt, ...);
    
    //! @brief    Prints message to console or a log file if debug level is high enough.
    void msg(int level, const char *fmt, ...);
    
    /*! @brief    Prints debug message to console or a log file
     *  @details  Debug messages are prefixed by a custom string naming the component.
     */
    void debug(const char *fmt, ...);
    
    /*! @brief    Prints debug message to console or a log file if debug level is high enogh.
     *  @details  Debug messages are prefixed by a custom string naming the component.
     */
    void debug(int level, const char *fmt, ...);
    
    /*! @brief    Prints warning message to console or a log file.
     *  @details  Warning messages are prefixed by a custom string naming the component.
     *            Warning messages are printed when something unexpected is encountered.
     */
    void warn(const char *fmt, ...);
    
    /*! @brief    Prints a panic message to console or a log file.
     *  @details  Panic messages are prefixed by a custom string naming the component.
     *            Panic messages indicate that a code bug is encountered.
     */
    void panic(const char *fmt, ...);    
};

#endif
