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

/** User's defaults storage
 *
 *  The user's defaults storages saves all configuration settings that persist
 *  across launches of the application. In addition, it provides a default
 *  value for all configuration options.
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
    void operator=(Defaults const&) = delete;

private:

    const char *getDescription() const override { return "Properties"; }
    void _dump(Category category, std::ostream& os) const override;


    ///
    /// @name Loading and saving the key-value storage
    /// @{

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
    /// @name Reading key-value pairs
    /// @{

public:

    /** @brief  Queries a key-value pair.
     *  @param  key     The key.
     *  @result The value as a string.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    string getString(const string &key) const;

    /** @brief  Queries a key-value pair.
     *  @param  key     The key.
     *  @result The value as an integer. 0 if the value cannot not be parsed.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    i64 getInt(const string &key) const;

    /** @brief  Queries a key-value pair.
     *  @param  option  A config option whose name is used as the key.
     *  @result The value as an integer.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    i64 get(Option option) const;

    /** @brief  Queries a key-value pair.
     *  @param  option  A config option whose name is used as the prefix of the key.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @result The value as an integer.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    i64 get(Option option, isize nr) const;

    /** @brief  Reads the default value for a key from the fallback storage.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    string getFallback(const string &key) const;


    /// @}
    /// @name Writing key-value pairs
    /// @{

    /** @brief  Writes a key-value pair into the user storage.
     *  @param  key     The key, given as a string.
     *  @param  value   The value, given as a string.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void setString(const string &key, const string &value);

    /** @brief  Writes a key-value pair into the user storage.
     *  @param  option  The option's name forms the keys.
     *  @param  value   The value, given as an integer.
     */
    void set(Option option, i64 value);

    /** @brief  Writes a key-value pair into the user storage.
     *  @param  option  The option's name forms the keys.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @param  value   The value, given as an integer.
     */
    void set(Option option, isize nr, i64 value);

    /** @brief  Writes multiple key-value pairs into the user storage.
     *  @param  option  The option's name forms the prefix of the keys.
     *  @param  nrs     The keys are parameterized by adding the vector values as suffixes.
     *  @param  value   The shared value for all pairs, given as an integer.
     *  @throw  VC64Error (#ERROR_INVALID_KEY)
     */
    void set(Option option, std::vector <isize> nrs, i64 value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  key     The key, given as a string.
     *  @param  value   The value, given as a string.
     */
    void setFallback(const string &key, const string &value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  option  The option's name forms the keys.
     *  @param  value   The value, given as a string.
     */
    void setFallback(Option option, const string &value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  option  The option's name forms the keys.
     *  @param  value   The value, given as an integer.
     */
    void setFallback(Option option, i64 value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  option  The option's name forms the keys.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @param  value   The value, given as a string.
     */
    void setFallback(Option option, isize nr, const string &value);

    /** @brief  Writes a key-value pair into the fallback storage.
     *  @param  option  The option's name forms the keys.
     *  @param  nr      The key is parameterized by adding the value as suffix.
     *  @param  value   The value, given as an integer.
     */
    void setFallback(Option option, isize nr, i64 value);

    /** @brief  Writes multiple key-value pairs into the fallback storage.
     *  @param  option  The option's name forms the prefix of the keys.
     *  @param  nrs     The keys are parameterized by adding the vector values as suffixes.
     *  @param  value   The shared value for all pairs.
     */
    void setFallback(Option option, std::vector <isize> nrs, const string &value);

    /// @copydoc setFallback(Option, std::vector <isize>, const string &);
    void setFallback(Option option, std::vector <isize> nrs, i64 value);


    /// @}
    /// @name Deleting key-value pairs
    /// @{

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
