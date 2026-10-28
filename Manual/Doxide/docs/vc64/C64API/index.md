---
generator: doxide
---


# C64API

**`class C64API : public API`**

@}
C64 API


## Functions

| Name | Description |
| ---- | ----------- |
| [getConfig](#getConfig) | @name Analyzing the emulator @{ Returns the component's current configuration. |
| [getInfo](#getInfo) |  Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [dump](#dump) |  Prints debug information about the component :material-location-enter: `category` :       Debug information category :material-location-enter: `os` :    Output stream  |
| [getRomTraits](#getRomTraits) |  Returns information about one of the installed Roms :material-location-enter: `type` :       The ROM type  |
| [getMsg](#getMsg) | @} @name Accessing the message queue @{ Reads a message from the message queue :material-location-enter: `msg` :            The returned message (if any) :material-location-enter: `exception` :      The exception the message reports (null if none) :material-keyboard-return: **Return** :    True if a message could be read, false if the queue was empty  |
| [getAutoInspectionMask](#getAutoInspectionMask) | @} @name Auto-inspecting components @{ Gets the current auto-inspection mask The GUI utilizes Auto-Inspection to display life updates of the internal emulator state in the Inspector panel. |
| [setAutoInspectionMask](#setAutoInspectionMask) |  Sets the current auto-inspection mask Each bit selects one component class (the bit position is the numeric value of the Class entry). |
| [setAutoInspectionMaskAsync](#setAutoInspectionMaskAsync) | Asynchronous version of setAutoInspectionMask(): queues the request and returns at once.  |
| [loadWorkspace](#loadWorkspace) | @} @name Working with workspaces and snapshots @{ Loads a workspace. |
| [saveWorkspace](#saveWorkspace) |  Saves a workspace to disk. |
| [saveWorkspaceAsync](#saveWorkspaceAsync) |  Saves a workspace to disk asynchronously. |
| [loadWorkspace](#loadWorkspace) |  Loads, saves, or asynchronously saves the workspace of an SVM These work on the workspace folder of an SVM that is already open. |
| [saveWorkspace](#saveWorkspace) | Saves the workspace into an SVM that is already open (see loadWorkspace()). |
| [saveWorkspaceAsync](#saveWorkspaceAsync) | Asynchronous version of saveWorkspace(): queues the request and returns at once.  |
| [takeSnapshot](#takeSnapshot) |  Takes a snapshot :material-location-enter: `compressor` :      The snapshot compression method :material-location-enter: `delay` :                 Optional delay in seconds :material-location-enter: `repeat` :               if true, snapshots are taken continuously :material-keyboard-return: **Return** :    A pointer to the created Snapshot object. !!! note The function transfers the ownership to the caller. It is his responsibility of the caller to free the object.  |
| [loadSnapshot](#loadSnapshot) |  Loads a snapshot :material-location-enter: `snapshot` :       Reference to a snapshot  |
| [loadSnapshot](#loadSnapshot) |  Loads a snapshot :material-location-enter: `path` :       Source path :material-alert-circle-outline: **Throw** :    Error if the file cannot be read or does not contain valid data.  |
| [loadSnapshotAsync](#loadSnapshotAsync) |  Asynchronous version of loadSnapshot(): queues the request and returns at once. Success and failure are both reported by the usual message (SNAPSHOT_RESTORED). If the request failed, the message carries the exception.  |
| [saveSnapshot](#saveSnapshot) |  Saves a snapshot to disk :material-location-enter: `path` :           Destination path :material-location-enter: `compressor` :     The compression method applied to the snapshot data. :material-alert-circle-outline: **Throw** :    Error if the file cannot be written.  |
| [saveSnapshotAsync](#saveSnapshotAsync) |  Saves a snapshot to disk asynchronously. |
| [saveSnapshot](#saveSnapshot) |  Saves a snapshot, including a screenshot, into an SVM Works on an SVM that is already open: the snapshot is added to its manifest, and the SVM is persisted. As with hibernateAsync, the object is used on the emulator thread by the asynchronous version and must stay alive, and untouched, until the SNAPSHOT_SAVED message has arrived. :material-alert-circle-outline: **Throw** :    Error if the SVM file cannot be read or written, or if its content is invalid (synchronous versions).  |
| [saveSnapshotAsync](#saveSnapshotAsync) | Asynchronous version of saveSnapshot(): queues the request and returns at once.  |
| [hibernate](#hibernate) |  Hibernates the machine into a virtual machine bundle Saves the workspace (if workspace is true) and a snapshot including a screenshot (if snapshot is true) into the SVM, and updates its manifest. |
| [hibernateAsync](#hibernateAsync) |  Hibernates the machine asynchronously. |
| [hibernate](#hibernate) |  Hibernates the machine into an SVM that is already open Works like the versions that take a path, but updates and persists the given object, so its manifest is current afterwards. |
| [hibernateAsync](#hibernateAsync) | Asynchronous version of hibernate(): queues the request and returns at once.  |
| [loadRom](#loadRom) | @} @name Handling ROMs @{ Loads a ROM from a file The ROM type is determined automatically. |
| [loadRom](#loadRom) | Loads a ROM from a file and installs it as the given ROM type. |
| [loadRomAsync](#loadRomAsync) |  Asynchronous version of loadRom(): queues the request and returns at once. Success and failure are both reported by a ROM_LOADED message. If the request failed, the message carries the exception.  |
| [loadRomAsync](#loadRomAsync) | Asynchronous version of loadRom(path, rom). |
| [deleteRom](#deleteRom) |  Removes an installed ROM The ROM contents is overwritten with zeroes. |
| [deleteRoms](#deleteRoms) |  Removes all installed ROMs including the floppy drive ROMs. |
| [saveRom](#saveRom) |  Saves a ROM to disk :material-alert-circle-outline: **Throw** :    Error if the file cannot be written.  |
| [installOpenRom](#installOpenRom) |  Installes a MEGA65 OpenRom  |
| [installOpenRoms](#installOpenRoms) |  Install all three MEGA65 OpenROMs  |
| [flash](#flash) | @} @name Handling media files @{ Flashes a file into memory  |
| [flashAsync](#flashAsync) |  Asynchronous version of flash(): queues the request and returns at once. The file is read on the emulator thread. Success and failure are both reported by the usual message (e.g., CRT_ATTACHED). If the request failed, the message carries the exception.  |

## Function Details

### deleteRom<a name="deleteRom"></a>
!!! function "void deleteRom(RomType type)"
    
     Removes an installed ROM
             The ROM contents is overwritten with zeroes.
    

### deleteRoms<a name="deleteRoms"></a>
!!! function "void deleteRoms()"
    
     Removes all installed ROMs including the floppy drive ROMs.
             The ROM contents is overwritten with zeroes.
    

### dump<a name="dump"></a>
!!! function "void dump(Category category, std::ostream &amp;os) const"
    
     Prints debug information about the component
    
     
    :material-location-enter: `category`
    :       Debug information category
     
    :material-location-enter: `os`
    :    Output stream
    

### flash<a name="flash"></a>
!!! function "void flash(const std::filesystem::path &amp;path, isize item = 0)"
    
    @}
    @name Handling media files
    @{
     Flashes a file into memory
    

### flashAsync<a name="flashAsync"></a>
!!! function "void flashAsync(const std::filesystem::path &amp;path, isize item = 0)"
    
     Asynchronous version of flash(): queues the request and returns at once.
    
     The file is read on the emulator thread. Success and failure are both
     reported by the usual message (e.g., CRT_ATTACHED). If the request
     failed, the message carries the exception.
    

### getAutoInspectionMask<a name="getAutoInspectionMask"></a>
!!! function "u64 getAutoInspectionMask() const"
    
    @}
    @name Auto-inspecting components
    @{
     Gets the current auto-inspection mask
     The GUI utilizes Auto-Inspection to display life updates of the internal
     emulator state in the Inspector panel. As soon as an auto-inspection
     mask is set, the emulator caches the internal states of the inspected
     components at periodic intervals. The inspected components are
     specified as a bit mask.
    
     
    :material-keyboard-return: **Return**
    :    A bit mask indicating the components under inspection
    

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const C64Info &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getConfig<a name="getConfig"></a>
!!! function "const C64Config &amp;getConfig() const"
    
    @name Analyzing the emulator
    @{
     Returns the component's current configuration.
    

### getInfo<a name="getInfo"></a>
!!! function "const C64Info &amp;getInfo() const"
    
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### getMsg<a name="getMsg"></a>
!!! function "bool getMsg(Message &amp;msg, std::exception_ptr &amp;exception)"
    
    @}
    @name Accessing the message queue
    @{
     Reads a message from the message queue
    
     
    :material-location-enter: `msg`
    :            The returned message (if any)
     
    :material-location-enter: `exception`
    :      The exception the message reports (null if none)
     
    :material-keyboard-return: **Return**
    :    True if a message could be read, false if the queue was empty
    

### getRomTraits<a name="getRomTraits"></a>
!!! function "RomTraits getRomTraits(RomType type) const"
    
     Returns information about one of the installed Roms
    
     
    :material-location-enter: `type`
    :       The ROM type
    

### hibernate<a name="hibernate"></a>
!!! function "void hibernate(const std::filesystem::path &amp;path, bool snapshot, bool workspace) const"
    
     Hibernates the machine into a virtual machine bundle
    
     Saves the workspace (if workspace is true) and a snapshot including a
     screenshot (if snapshot is true) into the SVM, and updates its
     manifest.
    
     
    :material-location-enter: `path`
    :           Path of the SVM
     
    :material-location-enter: `snapshot`
    :       Save a snapshot
     
    :material-location-enter: `workspace`
    :      Save the workspace
    

!!! function "void hibernate(retro::vault::SVMFile &amp;svm, bool snapshot, bool workspace) const"
    
     Hibernates the machine into an SVM that is already open
    
     Works like the versions that take a path, but updates and persists the
     given object, so its manifest is current afterwards. The asynchronous
     version uses the object on the emulator thread: it must stay alive
     and must not be touched until the HIBERNATED message has arrived.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the SVM file cannot be read or written, or if its content
     is invalid (synchronous versions).
    

### hibernateAsync<a name="hibernateAsync"></a>
!!! function "void hibernateAsync(const std::filesystem::path &amp;path, bool snapshot, bool workspace) const"
    
     Hibernates the machine asynchronously.
    
     The request is handed to the emulator thread, which carries it out
     when it processes its command queue. The function returns immediately.
     The GUI is informed by a HIBERNATED message, which carries an error
     code and description if the operation failed.
    

!!! function "void hibernateAsync(retro::vault::SVMFile &amp;svm, bool snapshot, bool workspace) const"
    
    Asynchronous version of hibernate(): queues the request and returns at once.
    

### installOpenRom<a name="installOpenRom"></a>
!!! function "void installOpenRom(RomType type)"
    
     Installes a MEGA65 OpenRom
    

### installOpenRoms<a name="installOpenRoms"></a>
!!! function "void installOpenRoms()"
    
     Install all three MEGA65 OpenROMs
    

### loadRom<a name="loadRom"></a>
!!! function "void loadRom(const std::filesystem::path &amp;path)"
    
    @}
    @name Handling ROMs
    @{
     Loads a ROM from a file
             The ROM type is determined automatically.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be read or does not contain valid data.
    

!!! function "void loadRom(const std::filesystem::path &amp;path, RomType rom)"
    
    Loads a ROM from a file and installs it as the given ROM type.
    

### loadRomAsync<a name="loadRomAsync"></a>
!!! function "void loadRomAsync(const std::filesystem::path &amp;path)"
    
     Asynchronous version of loadRom(): queues the request and returns at once.
    
     Success and failure are both reported by a ROM_LOADED message. If the
     request failed, the message carries the exception.
    

!!! function "void loadRomAsync(const std::filesystem::path &amp;path, RomType rom)"
    
    Asynchronous version of loadRom(path, rom).
    

### loadSnapshot<a name="loadSnapshot"></a>
!!! function "void loadSnapshot(const Snapshot &amp;snapshot)"
    
     Loads a snapshot
    
     
    :material-location-enter: `snapshot`
    :       Reference to a snapshot
    

!!! function "void loadSnapshot(const std::filesystem::path &amp;path)"
    
     Loads a snapshot
    
     
    :material-location-enter: `path`
    :       Source path
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be read or does not contain valid data.
    

### loadSnapshotAsync<a name="loadSnapshotAsync"></a>
!!! function "void loadSnapshotAsync(const std::filesystem::path &amp;path)"
    
     Asynchronous version of loadSnapshot(): queues the request and returns at once.
    
     Success and failure are both reported by the usual message
     (SNAPSHOT_RESTORED). If the request failed, the message carries the exception.
    

### loadWorkspace<a name="loadWorkspace"></a>
!!! function "void loadWorkspace(const std::filesystem::path &amp;path)"
    
    @}
    @name Working with workspaces and snapshots
    @{
     Loads a workspace.
    
     
    :material-location-enter: `path`
    :       Path to a workspace folder
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be read or does not contain valid data.
    

!!! function "void loadWorkspace(retro::vault::SVMFile &amp;svm)"
    
     Loads, saves, or asynchronously saves the workspace of an SVM
    
     These work on the workspace folder of an SVM that is already open.
     Saving replaces the workspace, notes its preview image in the manifest
     and persists the SVM. As with hibernateAsync, the object is used on
     the emulator thread and must stay alive, and untouched, until the
     WORKSPACE_SAVED message has arrived.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the SVM file cannot be read or written, or if its content
     is invalid (synchronous versions).
    

### saveRom<a name="saveRom"></a>
!!! function "void saveRom(const std::filesystem::path &amp;path, RomType rom)"
    
     Saves a ROM to disk
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be written.
    

### saveSnapshot<a name="saveSnapshot"></a>
!!! function "void saveSnapshot(const std::filesystem::path &amp;path, utl::Compressor compressor) const"
    
     Saves a snapshot to disk
    
     
    :material-location-enter: `path`
    :           Destination path
     
    :material-location-enter: `compressor`
    :     The compression method applied to the snapshot data.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be written.
    

!!! function "void saveSnapshot(retro::vault::SVMFile &amp;svm, utl::Compressor compressor) const"
    
     Saves a snapshot, including a screenshot, into an SVM
    
     Works on an SVM that is already open: the snapshot is added to its
     manifest, and the SVM is persisted. As with hibernateAsync, the object
     is used on the emulator thread by the asynchronous version and must
     stay alive, and untouched, until the SNAPSHOT_SAVED message has
     arrived.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the SVM file cannot be read or written, or if its content
     is invalid (synchronous versions).
    

### saveSnapshotAsync<a name="saveSnapshotAsync"></a>
!!! function "void saveSnapshotAsync(const std::filesystem::path &amp;path, utl::Compressor compressor) const"
    
     Saves a snapshot to disk asynchronously.
    
     The request is handed to the emulator thread, which carries it out
     when it processes its command queue. The function returns immediately.
     The GUI is informed by a SNAPSHOT_SAVED message, which carries an
     exception if the operation failed.
    
     
    :material-location-enter: `path`
    :           Destination path
     
    :material-location-enter: `compressor`
    :     The compression method applied to the snapshot data.
    

!!! function "void saveSnapshotAsync(retro::vault::SVMFile &amp;svm, utl::Compressor compressor) const"
    
    Asynchronous version of saveSnapshot(): queues the request and returns at once.
    

### saveWorkspace<a name="saveWorkspace"></a>
!!! function "void saveWorkspace(const std::filesystem::path &amp;path) const"
    
     Saves a workspace to disk.
    
     
    :material-location-enter: `path`
    :       Destination path
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be written.
    

!!! function "void saveWorkspace(retro::vault::SVMFile &amp;svm) const"
    
    Saves the workspace into an SVM that is already open (see loadWorkspace()).
    

### saveWorkspaceAsync<a name="saveWorkspaceAsync"></a>
!!! function "void saveWorkspaceAsync(const std::filesystem::path &amp;path) const"
    
     Saves a workspace to disk asynchronously.
    
     The request is handed to the emulator thread, which carries it out
     when it processes its command queue. The function returns immediately.
     The GUI is informed by a WORKSPACE_SAVED message, which carries an
     exception if the operation failed.
    
     
    :material-location-enter: `path`
    :       Destination path
    

!!! function "void saveWorkspaceAsync(retro::vault::SVMFile &amp;svm) const"
    
    Asynchronous version of saveWorkspace(): queues the request and returns at once.
    

### setAutoInspectionMask<a name="setAutoInspectionMask"></a>
!!! function "void setAutoInspectionMask(u64 mask)"
    
     Sets the current auto-inspection mask
    
     Each bit selects one component class (the bit position is the numeric
     value of the Class entry). A component's getCachedInfo() and
     getCachedMetrics() only return up-to-date snapshots while its class is
     selected.
    
     Example: The following call enables auto-inspections for the CIA chips
     and the CPU:
     setAutoInspectionMask(1LL << long(Class::CIA) | 1LL << long(Class::CPU));
    
     
    :material-location-enter: `mask`
    :    A bit mask indicating the components under inspection
    

### setAutoInspectionMaskAsync<a name="setAutoInspectionMaskAsync"></a>
!!! function "void setAutoInspectionMaskAsync(u64 mask)"
    
    Asynchronous version of setAutoInspectionMask(): queues the request and returns at once.
    

### takeSnapshot<a name="takeSnapshot"></a>
!!! function "std::unique_ptr&lt;Snapshot&gt; takeSnapshot(utl::Compressor compressor, isize delay = 0, bool repeat = false)"
    
     Takes a snapshot
    
     
    :material-location-enter: `compressor`
    :      The snapshot compression method
     
    :material-location-enter: `delay`
    :                 Optional delay in seconds
     
    :material-location-enter: `repeat`
    :               if true, snapshots are taken continuously
    
     
    :material-keyboard-return: **Return**
    :    A pointer to the created Snapshot object.
    
     
    !!! note
       The function transfers the ownership to the caller. It is
                 his responsibility of the caller to free the object.
    

