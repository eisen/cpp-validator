/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/reorder_and_present.hpp
*
*  Defines helpers for adjusting strings order and presentation.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_REORDER_AND_PRESENT_HPP
#define DRACOSHA_VALIDATOR_REORDER_AND_PRESENT_HPP

#include <iostream>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/operators/flag.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_operand.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/operand_formatter.hpp>
#include <dracosha/validator/reporting/report_aggregation.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename DstT, typename FormatterTs, typename ... Args>
void format_join(DstT& dst, FormatterTs&& formatters, Args&&... args)
{
    auto pairs=hana::zip(
        std::forward<FormatterTs>(formatters),
        make_cref_tuple(std::forward<Args>(args)...)
    );
    auto parts=hana::fold(
        pairs,
        hana::make_tuple(),
        [](auto&& prev, auto&& current)
        {
            return hana::append(std::forward<decltype(prev)>(prev),apply_ref(hana::front(current),hana::back(current),last_word_attributes(prev,0)));
        }
    );
    hana::unpack(
        parts,
        hana::partial(
                formatter_append_join_args,
                ref(dst),
                " "
            )
    );
}

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report for 1 argument, which must be an aggregation operator
 */
template <typename AggregationItemT>
struct apply_reorder_present_1arg_t
{
    template <typename DstT, typename StringsT>
    void operator () (
                        DstT& dst,
                        const StringsT& strings,
                        const AggregationItemT& aggregation_item
                     ) const
    {
        if ((!aggregation_item.single && aggregation_item.parts.size()>1)
                ||
                aggregation_item.aggregation.id==aggregation_id::NOT
                ||
                aggregation_item.aggregation.id==aggregation_id::ANY
                ||
                aggregation_item.aggregation.id==aggregation_id::ALL
            )
        {
            backend_formatter.append(
                dst,
                strings.aggregation_open(aggregation_item.aggregation)
            );
        }
        backend_formatter.append_join(
            dst,
            strings.aggregation_conjunction(aggregation_item.aggregation),
            aggregation_item.parts
        );
        if (!aggregation_item.single && aggregation_item.parts.size()>1)
        {
            backend_formatter.append(
                dst,
                strings.aggregation_close(aggregation_item.aggregation)
            );
        }
    }
};

template <typename OpT>
constexpr apply_reorder_present_1arg_t<OpT> apply_reorder_present_1arg{};

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report for 2 arguments without property and without member
 */
template <typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_2args_t
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst,
                        FormatterTs&& formatters,
                        const OpT& op,
                        const T2& b
                     ) const
    {
        format_join(dst,
            std::forward<FormatterTs>(formatters),
            op,
            b
        );
    }
};

template <typename OpT, typename T2>
struct apply_reorder_present_2args_t<
                        OpT,T2,
                        hana::when<
                            std::is_base_of<flag_t,std::decay_t<OpT>>::value
                        >
                    >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const OpT& op, const T2& b
                    ) const
    {
        format_join(dst,
            hana::make_tuple(
                hana::at(formatters,hana::size_c<0>)
            ),
            op.str(value,b)
        );
    }
};

template <typename OpT, typename T2>
constexpr apply_reorder_present_2args_t<OpT,T2> apply_reorder_present_2args{};

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report for 3 arguments with property but without member
 */
template <typename PropT, typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_3args_t
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const PropT& prop, const OpT& op, const T2& b
                     ) const
    {
        format_join(dst,
            std::forward<FormatterTs>(formatters),
            prop,
            op,
            b
        );
    }
};

template <typename PropT, typename OpT, typename T2>
struct apply_reorder_present_3args_t<
                        PropT,OpT,T2,
                        hana::when<
                            std::is_base_of<flag_t,std::decay_t<OpT>>::value
                        >
                    >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const PropT& prop, const OpT& op, const T2& b
                    ) const
    {
        hana::eval_if(
            std::decay_t<OpT>::prepend_property(prop),
            [&dst,&formatters,&op,&prop,&b](auto&&)
            {
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<0>)
                    ),
                    prop,
                    op.str(prop,b)
                );
            },
            [&dst,&formatters,&op,&prop,&b](auto&&)
            {
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>)
                    ),
                    op.str(prop,b)
                );
            }
        );
    }
};

template <typename PropT, typename OpT, typename T2>
constexpr apply_reorder_present_3args_t<PropT,OpT,T2> apply_reorder_present_3args{};

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report for 4 arguments with member
 */
template <typename MemberT, typename PropT, typename OpT, typename T2, typename = hana::when<true>>
struct apply_reorder_present_4args_t
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                    ) const
    {
        hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // member op b
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<3>)
                    ),
                    member,
                    op,
                    b
                );
            },
            [&](auto)
            {
                // prop of member op b
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<3>)
                    ),
                    make_member_property(member,prop),
                    op,
                    b
                );
            }
        );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 4 arguments with a member is compared to other member
 */
template <typename MemberT, typename PropT, typename OpT, typename T2>
struct apply_reorder_present_4args_t<
                MemberT,PropT,OpT,T2,hana::when<hana::is_a<member_operand_tag,T2>>
            >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                    DstT& dst, FormatterTs&& formatters,
                    const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                ) const
    {
        hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // member op member_operand(b)
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<3>)
                    ),
                    member,
                    op,
                    b.get()
                );
            },
            [&](auto)
            {
                // prop of member op prop of member_operand(b)
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<0>)
                    ),
                    make_member_property(member,prop),
                    op,
                    make_member_property(b.get(),prop)
                );
            }
        );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 4 arguments with a member is compared to the same member of sample object
 */
template <typename MemberT, typename PropT, typename OpT, typename T2>
struct apply_reorder_present_4args_t<
                MemberT,PropT,OpT,T2,hana::when<
                    (std::is_same<std::decay_t<T2>,string_master_sample_t>::value
                     ||
                     is_master_sample<T2>::value
                     )
                >
            >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                    ) const
    {
        hana::eval_if(
            std::is_same<std::decay_t<PropT>,type_p_value>::value,
            [&](auto)
            {
                // member op member of sample
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<3>)
                    ),
                    member,
                    op,
                    member,
                    string_conjunction_of,
                    b
                );
            },
            [&](auto)
            {
                // prop of member op prop of member of sample
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<2>),
                        hana::at(formatters,hana::size_c<3>)
                    ),
                    make_member_property(member,prop),
                    op,
                    make_member_property(member,prop),
                    string_conjunction_of,
                    b
                );
            }
        );
    }
};

/**
 * @brief Adjust presentation and order of validation report for flag operator
 */
template <typename MemberT, typename PropT, typename OpT, typename T2>
struct apply_reorder_present_4args_t<
                        MemberT,PropT,OpT,T2,
                        hana::when<
                            std::is_base_of<flag_t,std::decay_t<OpT>>::value
                        >
                    >
{
    template <typename DstT, typename FormatterTs>
    void operator () (
                        DstT& dst, FormatterTs&& formatters,
                        const MemberT& member, const PropT& prop, const OpT& op, const T2& b
                    ) const
    {
        hana::eval_if(
            !std::decay_t<OpT>::prepend_property(prop),
            [&dst,&member,&formatters,&op,&prop,&b](auto&&)
            {
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<1>)
                    ),
                    member,
                    op.str(prop,b)
                );
            },
            [&dst,&member,&formatters,&op,&prop,&b](auto&&)
            {
                format_join(dst,
                    hana::make_tuple(
                        hana::at(formatters,hana::size_c<0>),
                        hana::at(formatters,hana::size_c<1>)
                    ),
                    make_member_property(member,prop),
                    op.str(prop,b)
                );
            }
        );
    }
};
template <typename MemberT, typename PropT, typename OpT, typename T2>
constexpr apply_reorder_present_4args_t<MemberT,PropT,OpT,T2> apply_reorder_present_4args{};

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report for arbitrary number of arguments
 */
template <typename ...Args>
struct apply_reorder_present_t
{
    template <typename DstT, typename FormatterTs>
    void operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const
    {
        format_join(dst,
            std::forward<FormatterTs>(formatters),
            std::forward<Args>(args)...
        );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 4 arguments with member
 */
template <typename T1, typename T2, typename T3, typename T4>
struct apply_reorder_present_t<T1,T2,T3,T4>
{
    template <typename DstT, typename FormatterTs, typename ... Args>
    void operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const
    {
        apply_reorder_present_4args<T1,T2,T3,T4>(
            dst,
            std::forward<FormatterTs>(formatters),
            std::forward<Args>(args)...
        );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 3 arguments with property but without member
 */
template <typename T1, typename T2, typename T3>
struct apply_reorder_present_t<T1,T2,T3>
{
    template <typename DstT, typename FormatterTs, typename ... Args>
    void operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const
    {
        apply_reorder_present_3args<T1,T2,T3>(
            dst,
            std::forward<FormatterTs>(formatters),
            std::forward<Args>(args)...
        );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 2 arguments without property and without member
 */
template <typename T1, typename T2>
struct apply_reorder_present_t<T1,T2>
{
    template <typename DstT, typename FormatterTs, typename ... Args>
    void operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const
    {
        apply_reorder_present_2args<T1,T2>(
            dst,
            std::forward<FormatterTs>(formatters),
            std::forward<Args>(args)...
        );
    }
};

/**
 * @brief Adjust presentation and order of validation report for 1 argument
 */
template <typename T1>
struct apply_reorder_present_t<T1>
{
    template <typename DstT, typename FormatterTs, typename ... Args>
    void operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const
    {
        apply_reorder_present_1arg<T1>(
            dst,
            std::forward<FormatterTs>(formatters),
            std::forward<Args>(args)...
        );
    }
};

template <typename ... Args>
constexpr apply_reorder_present_t<Args...> apply_reorder_present{};

//-------------------------------------------------------------

/**
 * @brief Adjust presentation and order of validation report
 */
struct reorder_and_present_t
{
    template <typename DstT, typename FormatterTs, typename ...Args>
    void operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const
    {
        apply_reorder_present<Args...>(dst,std::forward<FormatterTs>(formatters),std::forward<Args>(args)...);
    }
};
constexpr reorder_and_present_t reorder_and_present{};

//-------------------------------------------------------------

}

//-------------------------------------------------------------
DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REORDER_AND_PRESENT_HPP
