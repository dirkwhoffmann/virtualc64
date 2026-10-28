---
generator: doxide
---


# DefaultsAPI

**`class DefaultsAPI : public API`**

@}
The user's defaults storage

 The defaults storage manages all configuration settings that persist across
 multiple application launches. It provides the following functionality:

 - **Loading and saving the storage data**

   You can persist the user's defaults storage in a file, a stream, or a
   string stream.

 - **Reading and writing key-value pairs**

   The return value is read from the user's defaults storage for registered
   keys. For unknown keys, an exception is thrown.

 - **Registerung fallback values**

   The fallback value is used for registered keys with no custom value set.

   
!!! note
 Setting a fallback value for an unknown key is permitted. In this
       case, a new key is registered together with the provided default value.
       The GUI utilizes this feature to register additional keys, such as keys
       storing shader-relevant parameters that are irrelevant to the emulation
       core.


## Functions

| Name | Description |
| ---- | ----------- |
| [load](#load) |  @{ @name Loading and saving the key-value storage Loads a storage file from disk :material-alert-circle-outline: **Throw** :    Error if the file cannot be read or does not contain valid data.  |
| [load](#load) |  Loads a storage file from a stream :material-alert-circle-outline: **Throw** :    Error if the file cannot be read or does not contain valid data.  |
| [load](#load) |  Loads a storage file from a string stream :material-alert-circle-outline: **Throw** :    Error if the file cannot be read or does not contain valid data.  |
| [save](#save) |  Saves a storage file to disk :material-alert-circle-outline: **Throw** :    Error if the file cannot be written.  |
| [save](#save) |  Saves a storage file to stream :material-alert-circle-outline: **Throw** :    Error if the file cannot be written.  |
| [save](#save) |  Saves a storage file to a string stream :material-alert-circle-outline: **Throw** :    Error if the file cannot be written.  |
| [getRaw](#getRaw) | @} @{ @name Reading key-value pairs Queries a key-value pair. |
| [get](#get) |  Queries a key-value pair. |
| [get](#get) |  Queries a key-value pair. |
| [getFallbackRaw](#getFallbackRaw) |  Queries a fallback key-value pair. |
| [getFallback](#getFallback) |  Queries a fallback key-value pair. |
| [getFallback](#getFallback) |  Queries a fallback key-value pair. |
| [set](#set) | @} @{ @name Writing key-value pairs Writes a key-value pair into the user storage. |
| [set](#set) |  Writes a key-value pair into the user storage. |
| [set](#set) |  Writes multiple key-value pairs into the user storage. |
| [set](#set) |  Writes a key-value pair into the user storage. |
| [set](#set) |  Writes multiple key-value pairs into the user storage. |
| [setFallback](#setFallback) |  Writes a key-value pair into the fallback storage. |
| [setFallback](#setFallback) |  Writes a key-value pair into the fallback storage. |
| [setFallback](#setFallback) |  Writes multiple key-value pairs into the fallback storage. |
| [setFallback](#setFallback) |  Writes a key-value pair into the fallback storage. |
| [setFallback](#setFallback) |  Writes multiple key-value pairs into the fallback storage. |
| [remove](#remove) | @} @{ @name Deleting key-value pairs Deletes all key-value pairs. |
| [remove](#remove) |  Deletes a key-value pair :material-location-enter: `key` :        The key of the key-value pair.  |
| [remove](#remove) |  Deletes a key-value pair :material-location-enter: `opt` :        The option's name forms the key.  |
| [remove](#remove) |  Deletes multiple key-value pairs. |

## Function Details

### get<a name="get"></a>
!!! function "i64 get(const string &amp;key) const"
    
     Queries a key-value pair.
     
    :material-location-enter: `key`
    :        The key.
     
    :material-keyboard-return: **Return**
    :    The value as an integer. 0 if the value cannot not be parsed.
    

!!! function "i64 get(Opt opt, isize nr = 0) const"
    
     Queries a key-value pair.
     
    :material-location-enter: `opt`
    :        A config option whose name is used as the prefix of the key.
     
    :material-location-enter: `nr`
    :         Optional number that is appened to the key as suffix.
     
    :material-keyboard-return: **Return**
    :    The value as an integer.
    

### getFallback<a name="getFallback"></a>
!!! function "i64 getFallback(const string &amp;key) const"
    
     Queries a fallback key-value pair.
     
    :material-location-enter: `key`
    :        The key.
     
    :material-keyboard-return: **Return**
    :    The value as an integer. 0 if the value cannot not be parsed.
    

!!! function "i64 getFallback(Opt opt, isize nr = 0) const"
    
     Queries a fallback key-value pair.
     
    :material-location-enter: `opt`
    :        A config option whose name is used as the key.
     
    :material-location-enter: `nr`
    :         Optional number that is appened to the key as suffix.
     
    :material-keyboard-return: **Return**
    :    The value as an integer.
    

### getFallbackRaw<a name="getFallbackRaw"></a>
!!! function "string getFallbackRaw(const string &amp;key) const"
    
     Queries a fallback key-value pair.
     
    :material-location-enter: `key`
    :        The key.
     
    :material-keyboard-return: **Return**
    :    The value as a string.
    

### getRaw<a name="getRaw"></a>
!!! function "string getRaw(const string &amp;key) const"
    
    @}
    @{
    @name Reading key-value pairs
     Queries a key-value pair.
     
    :material-location-enter: `key`
    :        The key.
     
    :material-keyboard-return: **Return**
    :    The value as a string.
    

### load<a name="load"></a>
!!! function "void load(const std::filesystem::path &amp;path)"
    
    
    @{
    @name Loading and saving the key-value storage
     Loads a storage file from disk
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be read or does not contain valid data.
    

!!! function "void load(std::ifstream &amp;stream)"
    
     Loads a storage file from a stream
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be read or does not contain valid data.
    

!!! function "void load(std::stringstream &amp;stream)"
    
     Loads a storage file from a string stream
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be read or does not contain valid data.
    

### remove<a name="remove"></a>
!!! function "void remove()"
    
    @}
    @{
    @name Deleting key-value pairs
     Deletes all key-value pairs.
    

!!! function "void remove(const string &amp;key)"
    
     Deletes a key-value pair
     
    :material-location-enter: `key`
    :        The key of the key-value pair.
    

!!! function "void remove(Opt opt)"
    
     Deletes a key-value pair
     
    :material-location-enter: `opt`
    :        The option's name forms the key.
    

!!! function "void remove(Opt opt, std::vector &lt;isize&gt; objids)"
    
     Deletes multiple key-value pairs.
     
    :material-location-enter: `opt`
    :        The option's name forms the prefix of the keys.
     
    :material-location-enter: `objids`
    :     The keys are parameterized by adding the vector values as suffixes.
    

### save<a name="save"></a>
!!! function "void save(const std::filesystem::path &amp;path)"
    
     Saves a storage file to disk
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be written.
    

!!! function "void save(std::ofstream &amp;stream)"
    
     Saves a storage file to stream
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be written.
    

!!! function "void save(std::stringstream &amp;stream)"
    
     Saves a storage file to a string stream
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be written.
    

### set<a name="set"></a>
!!! function "void set(const string &amp;key, const string &amp;value)"
    
    @}
    @{
    @name Writing key-value pairs
     Writes a key-value pair into the user storage.
     
    :material-location-enter: `key`
    :        The key, given as a string.
     
    :material-location-enter: `value`
    :      The value, given as a string.
    

!!! function "void set(Opt opt, const string &amp;value)"
    
     Writes a key-value pair into the user storage.
     
    :material-location-enter: `opt`
    :        The option's name forms the prefix of the keys.
     
    :material-location-enter: `value`
    :      The value, given as a string.
    

!!! function "void set(Opt opt, const string &amp;value, std::vector&lt;isize&gt; objids)"
    
     Writes multiple key-value pairs into the user storage.
     
    :material-location-enter: `opt`
    :        The option's name forms the prefix of the keys.
     
    :material-location-enter: `value`
    :      The value for all pairs, given as a string.
     
    :material-location-enter: `objids`
    :     The keys are parameterized by adding the vector values as suffixes.
    

!!! function "void set(Opt opt, i64 value)"
    
     Writes a key-value pair into the user storage.
     
    :material-location-enter: `opt`
    :        The option's name forms the prefix of the keys.
     
    :material-location-enter: `value`
    :      The value, given as an integer.
    

!!! function "void set(Opt opt, i64 value, std::vector&lt;isize&gt; objids)"
    
     Writes multiple key-value pairs into the user storage.
     
    :material-location-enter: `opt`
    :        The option's name forms the prefix of the keys.
     
    :material-location-enter: `value`
    :      The value for all pairs, given as an integer.
     
    :material-location-enter: `objids`
    :     The keys are parameterized by adding the vector values as suffixes.
    

### setFallback<a name="setFallback"></a>
!!! function "void setFallback(const string &amp;key, const string &amp;value)"
    
     Writes a key-value pair into the fallback storage.
     
    :material-location-enter: `key`
    :        The key, given as a string.
     
    :material-location-enter: `value`
    :      The value, given as a string.
    

!!! function "void setFallback(Opt opt, const string &amp;value)"
    
     Writes a key-value pair into the fallback storage.
     
    :material-location-enter: `opt`
    :        The option's name forms the prefix of the keys.
     
    :material-location-enter: `value`
    :      The value, given as an integer.
    

!!! function "void setFallback(Opt opt, const string &amp;value, std::vector&lt;isize&gt; objids)"
    
     Writes multiple key-value pairs into the fallback storage.
     
    :material-location-enter: `opt`
    :        The option's name forms the prefix of the keys.
     
    :material-location-enter: `value`
    :      The shared value for all pairs, given as a string.
     
    :material-location-enter: `objids`
    :     The keys are parameterized by adding the vector values as suffixes.
    

!!! function "void setFallback(Opt opt, i64 value)"
    
     Writes a key-value pair into the fallback storage.
     
    :material-location-enter: `opt`
    :        The option's name forms the prefix of the keys.
     
    :material-location-enter: `value`
    :      The value, given as an integer.
    

!!! function "void setFallback(Opt opt, i64 value, std::vector&lt;isize&gt; objids)"
    
     Writes multiple key-value pairs into the fallback storage.
     
    :material-location-enter: `opt`
    :        The option's name forms the prefix of the keys.
     
    :material-location-enter: `value`
    :      The shared value for all pairs, given as an integer.
     
    :material-location-enter: `objids`
    :     The keys are parameterized by adding the vector values as suffixes.
    

