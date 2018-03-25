/*!
 * @header      VC64Object.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2015 Dirk W. Hoffmann
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

/*! @brief    Common functionality of all VirtualC64 objects.
 *  @details  This class defines the base functionality of all objects such as 
 *            printing debug messages.
 */
class VC64Object {

private:

    /*! @brief    Log file.
     *  @details  By default, this variable is NULL and all debug and trace messages are sent to
     *            stdout or stderr. Assign a file handle, if you wish to send debug output to a file.
     *  @note     logfile is a class member, i.e., it is shared among all objects
     */
    static FILE *logfile;

    /*! @brief    Tracing ringbuffer
     *  @details  All trace messages are written to a ringbuffer.
     *  @seealso  backtrace()
     *  @note     traceBuffer is a class member, i.e., it is shared among all objects
     */
    static char traceBuffer[512][256];
    static unsigned tracePtr;
    
    /*! @brief    Default debug level
     *  @details  On object creation, this value is used as debug level.
     */
    static unsigned defaultDebugLevel;

    /*! @brief    Debug level
     *  @details  Debug messages are written either to console or a logfile. Set to 0 to omit messages.
     */
    unsigned debugLevel;

    /*! @brief    Indicates whether the component should print trace messages.
     *  @details  If set to 0, no trace messages are printed when calling trace().
     *            Otherwise, trace() prints a message into the ringbuffer and to stderr
     *            Set to a negative value to trace forever.
     */
     int traceCounter;

    //! @brief    If set, printing trace messages to stderr is omitted.
    bool silentTracing;

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
    
    /*! @brief    Sets the logfile.
     */
    static void setLogfile(FILE *file) { logfile = file; }

    /*! @brief    Sets the default debug level.
     */
    static void setDefaultDebugLevel(unsigned level) { defaultDebugLevel = level; }

    /*! @brief    Changes the debug level for a specific object.
     */
    void setDebugLevel(unsigned level) { debugLevel = level; }

    /*! @brief    Returns the textual description.
     */
    const char *getDescription() { return description ? description : ""; }

    /*! @brief    Assigns a textual description.
     */
    void setDescription(const char *desc) { description = desc; }

    
    //
    //! @functiongroup Debugging the component
    //
    
    /*! @brief    Returns true iff trace mode is enabled.
     */
    bool tracingEnabled() { return traceCounter != 0; }
    
    /*! @brief    Enables or disables trace mode.
     */
    void startTracing(int count = -1);
    void stopTracing();
    void startSilentTracing(int count = -1);
    void stopSilentTracing();
    void backtrace(int count);
    
    //
    //! @functiongroup Printing messages to console
    //
    
    /*! @brief    Prints message to console or a log file.
     */
    void msg(const char *fmt, ...);
    
    /*! @brief    Prints message to console or a log file if debug level is high enough.
     */
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
    
    /*! @brief    Prints a trace message
     */
    void trace(const char *fmt, ...);
};

#endif
