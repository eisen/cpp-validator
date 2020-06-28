/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/extract.hpp
*
*  Defines method to extract value from operand
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_EXTRACT_HPP
#define DRACOSHA_VALIDATOR_EXTRACT_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/lazy.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Extract value from input
  @param v Input
  @return Either input argument or result of handler evaluation if input is a lazy handler
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto extract = [](auto&& v) ->decltype(auto)
{
  return hana::if_(hana::is_a<lazy_tag,decltype(v)>,
    [](auto&& x) -> decltype(auto) { return x(); },
    [](auto&& x) -> decltype(auto) { return hana::id(std::forward<decltype(x)>(x)); }
  )(std::forward<decltype(v)>(v));
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_EXTRACT_HPP