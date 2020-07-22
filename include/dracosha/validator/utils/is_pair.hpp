/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/utils/is_pair.hpp
*
*  Defines helper to check if object is of std::pair type.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_IS_PAIR_HPP
#define DRACOSHA_VALIDATOR_IS_PAIR_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T, typename=void>
struct has_first
{
    constexpr static const bool value=false;
};
template <typename T>
struct has_first<T,
        decltype((void)std::declval<std::decay_t<T>>().first)
        >
{
    constexpr static const bool value=true;
};

template <typename T, typename=void>
struct has_second
{
    constexpr static const bool value=false;
};
template <typename T>
struct has_second<T,
        decltype((void)std::declval<std::decay_t<T>>().second)>
{
    constexpr static const bool value=true;
};

template <typename T, typename =hana::when<true>>
struct is_pair_t
{
    constexpr static const bool value=false;
};
template <typename T>
struct is_pair_t<T,
            hana::when<has_first<T>::value && has_second<T>::value>
        >
{
    constexpr static const bool value=true;
};

template <typename T>
constexpr auto is_pair(T&&)
{
    return is_pair_t<T>::value;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_IS_PAIR_HPP