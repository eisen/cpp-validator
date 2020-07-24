/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/format_value.hpp
*
* Defines helper for value formatting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMAT_VALUE_HPP
#define DRACOSHA_VALIDATOR_FORMAT_VALUE_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct string_true_t : public enable_to_string<string_true_t>
{
    constexpr static const char* description="true";
};
constexpr string_true_t string_true{};
struct string_false_t : public enable_to_string<string_false_t>
{
    constexpr static const char* description="false";
};
constexpr string_false_t string_false{};

namespace detail
{

template <typename T, typename =hana::when<true>>
struct format_value_t
{
    template <typename T1, typename TranslatorT, typename DecoratorT>
    auto operator () (T1&& val, const TranslatorT&, const DecoratorT& decorator) const -> decltype(auto)
    {
        return decorator(std::forward<T1>(val));
    }
};

/**
 * @brief  Formatter of values the std::string can be constructed from
 */
template <typename T>
struct format_value_t<T,hana::when<
            std::is_constructible<std::string,T>::value>
        >
{
    template <typename T1, typename TranslatorT, typename DecoratorT>
    auto operator () (T1&& val, const TranslatorT&, const DecoratorT& decorator) const -> decltype(auto)
    {
        return decorator(std::string(std::forward<T1>(val)));
    }
};

/**
 * @brief  Formatter of boolean values
 */
template <typename T>
struct format_value_t<T,hana::when<std::is_same<bool,std::decay_t<T>>::value>>
{
    template <typename T1, typename TranslatorT, typename DecoratorT>
    auto operator () (T1&& val, const TranslatorT& translator, const DecoratorT& decorator) const -> decltype(auto)
    {
        return decorator(translator(val?std::string(string_true):std::string(string_false)));
    }
};

/**
 * @brief Default formatter of values
 */
template <typename T>
constexpr format_value_t<T> format_value{};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMAT_VALUE_HPP
