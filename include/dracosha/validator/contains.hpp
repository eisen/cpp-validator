/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/contains.hpp
*
*  Defines helper to check if object contains a member.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CONTAINS_HPP
#define DRACOSHA_VALIDATOR_CONTAINS_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/can_check_contains.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Helper for checking if object contains a member
 */
struct contains_t
{
    /**
     * @brief Check if object contains property
     *
     * Check is performed at compile time using types of object and property.
     */
    template <typename T1, typename T2>
    constexpr bool operator () (
                                const T1&,
                                const T2&,
                                std::enable_if_t<has_property<T1,T2>(),
                                                                    void*> =nullptr
                             ) const
    {
        return true;
    }

    /**
     *  @brief Check if object contains member by key
     *  @param a Object under validation
     *  @param b Key
     *  @return Operation status - true if found, false if not found
     */
    template <typename T1, typename T2>
    constexpr bool operator () (
                                const T1& a,
                                const T2& b,
                                std::enable_if_t<!has_property<T1,T2>() && can_check_contains<T1,T2>(),
                                                                    void*> =nullptr
                             ) const
    {
        return hana::if_(detail::has_find(a,b),
            [](auto&& a1, auto&& b1) { return a1.find(std::forward<decltype(b1)>(b1))!=a1.end(); },
            hana::if_(detail::has_has(a,b),
                [](auto&& a1, auto&& b1) { return a1.has(b1); },
                hana::if_(detail::has_contains(a,b),
                    [](auto&& a1, auto&& b1) { return a1.contains(b1); },
                        hana::if_(detail::has_isSet(a,b),
                            [](auto&& a1, auto&& b1) { return a1.isSet(b1); },
                            [](auto&&, auto&&) { return false; }
                        )
                    )
                )
        )(std::forward<decltype(a)>(a),std::forward<decltype(b)>(b));
    }

    /**
     *  @brief Compile time check if operation is not applicable.
     */
    template <typename T1, typename T2>
    constexpr bool operator () (
                                const T1&,
                                const T2&,
                                std::enable_if_t<!can_check_contains<T1,T2>(),
                                                                    void*> =nullptr
                             ) const
    {
        return false;
    }
};
/**
  Instance to use as contains() operator.
*/
constexpr contains_t contains{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CONTAINS_HPP
