/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/version.hpp
*
*  Defines macros for tracking the version of the library.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_VERSION_HPP
#define HATN_VALIDATOR_VERSION_HPP

//! @internal
//! Transforms a (version, revision, patchlevel) triple into a number of the
//! form 0xVVRRPPPP to allow comparing versions in a normalized way.
//!
//! See http://sourceforge.net/p/predef/wiki/VersionNormalization.
#define HATN_VALIDATOR_CONFIG_VERSION(version, revision, patch) \
    (((version) << 24) + ((revision) << 16) + (patch))

//! @ingroup group-config
//! Macro expanding to the major version of the library, i.e. the `x` in `x.y.z`.
#define HATN_VALIDATOR_MAJOR_VERSION 2

//! @ingroup group-config
//! Macro expanding to the minor version of the library, i.e. the `y` in `x.y.z`.
#define HATN_VALIDATOR_MINOR_VERSION 1

//! @ingroup group-config
//! Macro expanding to the patch level of the library, i.e. the `z` in `x.y.z`.
#define HATN_VALIDATOR_PATCH_VERSION 1

//! @ingroup group-config
//! Macro expanding to the full version of the library, in hexadecimal
//! representation.
#define HATN_VALIDATOR_VERSION                                                  \
    HATN_VALIDATOR_CONFIG_VERSION(HATN_VALIDATOR_MAJOR_VERSION,             \
                              HATN_VALIDATOR_MINOR_VERSION,                     \
                              HATN_VALIDATOR_PATCH_VERSION)                     \

#endif // HATN_VALIDATOR_VERSION_HPP
