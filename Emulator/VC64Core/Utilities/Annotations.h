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

//
// Compiler directives
//

#if defined(__clang__)
    #define alwaysinline __attribute__((always_inline))
#elif defined(__GNUC__) || defined(__GNUG__)
    #define alwaysinline __attribute__((always_inline))
#elif defined(_MSC_VER)
    #define alwaysinline __forceinline
#elif
    #define alwaysinline inline
#endif


//
// Syntactic sugar
//

/** @brief  Marks a function a throwing.
 *  It is used to mark all methods that use the exception mechanism to signal
 *  error conditions instead of returning error codes. The keyword is used for
 *  documentary purposes only as C++ implicitly marks all functions with
 *  noexcept(false) by default.
 *  @note   It is used in favor of classic throw lists since the latter causes
 *          the compiler to embed unwanted runtime checks in the code.
 */
#define throws noexcept(false)
