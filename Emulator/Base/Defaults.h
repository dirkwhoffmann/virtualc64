// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "CoreObject.h"
#include "OptionTypes.h"
#include "Concurrency.h"
#include "IOUtils.h"

namespace vc64 {

/** The user's defaults storage
 *
 *  The defaults storage manages all configuration settings that persist across
 *  multiple application launches. It provides the following functionality:
 *
 *  - **Loading and saving the storage data**
 *
 *    You can persist the user's defaults storage in a file, a stream, or a
 *    string stream.
 *
 *  - **Reading and writing key-value pairs**
 *
 *    The return value is read from the user's defaults storage for registered
 *    keys. For unknown keys, an exception is thrown.
 *
 *  - **Registerung fallback values**
 *
 *    The fallback value is used for registered keys with no custom value set.
 *
 *    @note Setting a fallback value for an unknown key is permitted. In this
 *    case, a new key is registered together with the provided default value.
 *    The GUI utilizes this feature to register additional keys, such as keys
 *    storing shader-relevant parameters that are irrelevant to the emulation
 *    core.
 */
class Defaults final : public CoreObject, public Dumpable {

    mutable util::ReentrantMutex mutex;

    /// The key-value storage
    std::map <string, string> values;

    /// The default value storage
    std::map <string, string> fallbacks;


    //
    // Methods
    //

public:

    Defaults();
    Defaults(Defaults const&) = delete;
    const char *objectName() const override { return "Defaults"; }
    void operator=(Defaults const&) = delete;

private:

    void _dump(Category category, std::ostream& os) const override;


    ///
    /// @{
    /// @name Loading and saving the key-value storage

public:

    /** @brief  Loads a storage file from disk
     *  @throw  VC64Error (#ERROR_FILE_NOT_FOUND)
     *  @throw  VC64Error (#ERROR_SYNTAX)
     */
    void load(const fs::path &path);

    /** @brief  Loads a storage file from a stream
     *  @throw  VC64Error (#ERROR_SYNTAX)
     */
    void load(std::ifstream &stream);

    /** @brief  Loads a storage file from a string stream
     *  @throw  VC64Error (#ERROR_SYNTAX)
     */
    void load(std::stringstream &stream);

    /** @brief  Saves a storage file to disk
     *  @throw  VC64Error (#ERROR_FILE_CANT_WRITE)
     */
    void save(const fs::path &path);

    /** @brief  Saves a storage file to stream
     */
    void save(std::ofstream &stream);

    /** @brief  Saves a storage file to a string stream
     */
    void save(std::stringstream &stream);


    /// @}
    /// @{
    /// @name Reading key-value pairs

public:

    /** @brief  Queries a key-value pair.
     *  @param  key     The key.
     *  @result The value as a string.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    string getString(const string &key) const;

    /** @brief  Queries a key-value pair.
     *  @param  key     The key.
     *  @result The value as an integer. 0 if the value cannot not be parsed.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 getInt(const string &key) const;

    /** @brief  Queries a key-value pair.
     *  @param  option  A config option whose name is used as the key.
     *  @result The value as an integer.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 get(Option option) const;

    /** @brief  Queries a key-value pair.
     *  @param  option  A config option whose name is used as the prefix of the key.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @result The value as an integer.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 get(Option option, isize nr) const;

    /** @brief  Queries a fallback key-value pair.
     *  @param  key     The key.
     *  @result The value as a string.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    string getFallbackString(const string &key) const;

    /** @brief  Queries a fallback key-value pair.
     *  @param  key     The key.
     *  @result The value as an integer. 0 if the value cannot not be parsed.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 getFallbackInt(const string &key) const;

    /** @brief  Queries a fallback key-value pair.
     *  @param  option  A config option whose name is used as the key.
     *  @result The value as an integer.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 getFallback(Option option) const;

    /** @brief  Queries a fallback key-value pair.
     *  @param  option  A config option whose name is used as the prefix of the key.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @result The value as an integer.
     *  @throw  VC64Error (#ERROR\_INVALID\_KEY)
     */
    i64 getFallback(Option option, isize nr) const;


    /// @}
    /// @{
    /// @name Writing key-value pairs

    /** @brief  Writes a key-value pair into the user storage.
     *  @param  key     The key, given as a string.
     *  @param  value   The value, given as a string.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void setString(const string &key, const string &value);

    /** @brief  Writes a key-value pair into the user storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  value   The value, given as an integer.
     */
    void set(Option opt, i64 value);

    /** @brief  Writes a key-value pair into the user storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @param  value   The value, given as an integer.
     */
    void set(Option opt, isize nr, i64 value);

    /** @brief  Writes multiple key-value pairs into the user storage.
     *  @param  opt     The option's name forms the prefix of the keys.
     *  @param  nrs     The keys are parameterized by adding the vector values as suffixes.
     *  @param  value   The shared value for all pairs, given as an integer.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void set(Option opt, std::vector <isize> nrs, i64 value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  key     The key, given as a string.
     *  @param  value   The value, given as a string.
     */
    void setFallback(const string &key, const string &value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  value   The value, given as a string.
     */
    void setFallback(Option opt, const string &value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  value   The value, given as an integer.
     */
    void setFallback(Option opt, i64 value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @param  value   The value, given as a string.
     */
    void setFallback(Option opt, isize nr, const string &value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  opt     The option's name forms the keys.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @param  value   The value, given as an integer.
     */
    void setFallback(Option opt, isize nr, i64 value);

    /** @brief  Writes multiple key-value pairs into the fallback storage.
     *  @param  opt     The option's name forms the prefix of the keys.
     *  @param  nrs     The keys are parameterized by adding the vector values as suffixes.
     *  @param  value   The shared value for all pairs.
     */
    void setFallback(Option opt, std::vector <isize> nrs, const string &value);

    /// @copydoc setFallback(Option, std::vector <isize>, const string &);
    void setFallback(Option opt, std::vector <isize> nrs, i64 value);


    /// @}
    /// @{
    /// @name Deleting key-value pairs

    /** @brief  Deletes all key-value pairs.
     */
    void remove();

    /** @brief  Deletes a key-value pair
     *  @param  key     The key of the key-value pair.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void remove(const string &key) throws;

    /** @brief  Deletes a key-value pair
     *  @param  option  The option's name forms the prefix of the key.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void remove(Option option) throws;

    /** @brief  Deletes a key-value pair.
     *  @param  option  The option's name forms the prefix of the key.
     *  @param  nr      The key is parameterized by adding this value as suffix.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void remove(Option option, isize nr) throws;

    /** @brief  Deletes multiple key-value pairs.
     *  @param  option  The option's name forms the prefix of the keys.
     *  @param  nrs     The keys are parameterized by adding the vector values as suffixes.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void remove(Option option, std::vector <isize> nrs) throws;

    /// @}
};

}
