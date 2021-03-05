/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/aggregation.ipp
*
*  Defines aggregation used for logical operators.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_AGGREGATION_IPP
#define DRACOSHA_VALIDATOR_AGGREGATION_IPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/aggregation.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/adapters/impl/reporting_adapter_impl.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename AdapterT, typename=hana::when<true>>
struct aggregate_report
{
    template <typename AdapterT1, typename AggregationT, typename MemberT>
    static void open(AdapterT1&&, AggregationT&&, MemberT&&)
    {}

    template <typename AdapterT1>
    static void close(AdapterT1&&, status)
    {}
};

template <typename AdapterT>
struct aggregate_report<AdapterT,
        hana::when<
            std::is_base_of<reporting_adapter_tag,typename std::decay_t<AdapterT>::type>::value
        >>
{
    template <typename AdapterT1, typename AggregationT, typename PathT>
    static void open(AdapterT1&& adapter, AggregationT&& str, PathT&& path)
    {
        auto& reporter=adapter.traits().reporter();
        using type=std::decay_t<decltype(reporter)>;
        auto& mutable_reporter=const_cast<type&>(reporter);
        hana::eval_if(
            hana::equal(hana::size(path),hana::size_c<0>),
            [&](auto&&)
            {
                mutable_reporter.aggregate_open(str);
            },
            [&](auto&& _)
            {
                mutable_reporter.aggregate_open(str,make_member(_(path)));
            }
        );
    }

    template <typename AdapterT1>
    static void close(AdapterT1&& adapter, status ret)
    {
        auto& reporter=adapter.traits().reporter();
        using type=std::decay_t<decltype(reporter)>;
        auto& mutable_reporter=const_cast<type&>(reporter);
        mutable_reporter.aggregate_close(ret);
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_AGGREGATION_IPP
