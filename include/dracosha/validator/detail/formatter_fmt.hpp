/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/formatter_fmt.hpp
*
*  Defines formatter that uses fmt library for strings formatting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP
#define DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP

#include <fmt/format.h>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Append arguments to destination object
 */
template <typename DstT, typename ...Args>
void fmt_append_args(DstT& dst, Args&&... args)
{
    if (!dst.empty())
    {
        fmt::format_to(std::back_inserter(dst),", ");
    }

    constexpr auto fmt_str1 = BOOST_HANA_STRING("{}");
    constexpr auto fmt_str_rest= hana::replicate<hana::tuple_tag>(
                    BOOST_HANA_STRING(" {}"),
                    hana::size_c<sizeof...(Args)-1>
                );
    constexpr auto fmt_str_all=hana::prepend(fmt_str_rest,fmt_str1);
    constexpr auto fmt_str=hana::sum<hana::string_tag>(fmt_str_all);

    fmt::format_to(std::back_inserter(dst),hana::to<const char*>(fmt_str),std::forward<Args>(args)...);
}

/**
 * @brief Prepend arguments to destination object
 */
template <typename DstT, typename ...Args>
void fmt_prepend_args(DstT& dst, Args&&... args)
{
    typename std::decay<DstT>::type tmp;
    fmt_append_args(tmp,std::forward<Args>(args)...);
    fmt::format_to(tmp,": ");
    dst.insert(dst.begin(),tmp.begin(),tmp.end());
}

/**
 * @brief Format string from variadic arguments using fmt backend
 */
template <typename DstT, typename ...Args>
struct fmt_formatter_t
{
    static void append(DstT& dst, Args&&... args)
    {
        return fmt_append_args(dst,std::forward<Args>(args)...);
    }
    static void prepend(DstT& dst, Args&&... args)
    {
        return fmt_prepend_args(dst,std::forward<Args>(args)...);
    }
};

/**
 * @brief Format string from hana tuple using fmt backend
 */
template <typename DstT, typename Ts>
struct fmt_formatter_t<DstT,Ts,
                    hana::when<hana::is_a<hana::tuple_tag,Ts>>
                    >
{
    static void append(DstT& dst, Ts&& ts)
    {
        hana::unpack(std::forward<Ts>(ts),hana::partial(fmt_append_args,dst));
    }

    static void prepend(DstT& dst, Ts&& ts)
    {
        hana::unpack(std::forward<Ts>(ts),hana::partial(fmt_prepend_args,dst));
    }
};

/**
 * @brief Append arguments to string usnig fmt backend
 */
struct fmt_append_t
{
    template <typename ...Args>
    void operator () (Args&&... args) const
    {
        fmt_formatter_t<Args...>::append(std::forward<Args>(args)...);
    }
};

/**
 * @brief Prepend arguments to string usnig fmt backend
 */
struct fmt_prepend_t
{
    template <typename ...Args>
    void operator () (Args&&... args) const
    {
        fmt_formatter_t<Args...>::prepend(std::forward<Args>(args)...);
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP
