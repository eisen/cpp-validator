/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/prevalidation/prevalidation_adapter.hpp
*
*  Defines adapter for prevalidation of a single member with reporting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/string_view.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/utils/value_as_container.hpp>
#include <dracosha/validator/adapters/adapter.hpp>
#include <dracosha/validator/reporting/reporter.hpp>
#include <dracosha/validator/adapters/impl/reporting_adapter_impl.hpp>
#include <dracosha/validator/adapters/adapter_with_aggregation_iterator.hpp>
#include <dracosha/validator/prevalidation/prevalidation_adapter_impl.hpp>
#include <dracosha/validator/prevalidation/prevalidation_adapter_tag.hpp>
#include <dracosha/validator/prevalidation/strict_any.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Traits of prevalidation adapter.
 */
template <typename MemberT, typename T, typename ReporterT, typename WrappedT>
class prevalidation_adapter_traits : public adapter_traits,
                                     public object_wrapper<WrappedT>,
                                     public reporting_adapter_impl<ReporterT,prevalidation_adapter_impl<MemberT>>,
                                     public with_check_member_exists<adapter<prevalidation_adapter_traits<MemberT,T,ReporterT,WrappedT>>>
{
    public:

        using hana_tag=prevalidation_adapter_tag;

        using expand_aggregation_members=std::integral_constant<bool,false>;

        /**
         * @brief Constructor
         * @param member Member to validate
         * @param val Value to validate with
         * @param reporter Reporter to use for report construction if validation fails
         */
        prevalidation_adapter_traits(
                    adapter<prevalidation_adapter_traits<MemberT,T,ReporterT,WrappedT>>& adpt,
                    MemberT&& member,
                    T&& val,
                    ReporterT&& reporter
                ) : object_wrapper<WrappedT>(std::forward<T>(val)),
                    reporting_adapter_impl<ReporterT,prevalidation_adapter_impl<MemberT>>(
                        std::forward<ReporterT>(reporter),
                        std::forward<MemberT>(member)
                    ),
                    with_check_member_exists<adapter<prevalidation_adapter_traits<MemberT,T,ReporterT,WrappedT>>>(adpt)
        {
            this->set_check_member_exists_before_validation(true);
        }
};

/**
 * @brief Prevalidation adapter to validate single member before update.
 */
template <typename MemberT, typename T, typename ReporterT, typename WrappedT=T>
class prevalidation_adapter : public adapter<prevalidation_adapter_traits<MemberT,T,ReporterT,WrappedT>>
{
    public:

        using reporter_type=ReporterT;

        /**
         * @brief Constructor
         * @param member Member to validate
         * @param val Value to validate with
         * @param reporter Reporter to use for report construction if validation fails
         */
        prevalidation_adapter(
                MemberT&& member,
                T&& val,
                ReporterT&& reporter
            ) : adapter<prevalidation_adapter_traits<MemberT,T,ReporterT,WrappedT>>(std::forward<MemberT>(member),std::forward<T>(val),std::forward<ReporterT>(reporter))
        {}
};

/**
 * @brief Create prevalidation adapter.
 * @param member Member to validate.
 * @param val Value to validate with.
 * @param reporter Reporter to use for report construction if validation fails.
 */
template <typename MemberT, typename T, typename ReporterT>
auto make_prevalidation_adapter(
                            MemberT&& member,
                            T&& val,
                            ReporterT&& reporter,
                            std::enable_if_t<
                                    hana::is_a<reporter_tag,ReporterT> && !hana::is_a<strict_any_tag,T>,
                                    void*>
                            =nullptr)
{
    using value_type=decltype(adjust_view_type(std::forward<T>(val)));
    return prevalidation_adapter<MemberT,value_type,ReporterT>(std::forward<MemberT>(member),adjust_view_type(std::forward<T>(val)),std::forward<ReporterT>(reporter));
}

/**
 * @brief Create prevalidation adapter.
 * @param member Member to validate.
 * @param val Value to validate with.
 * @param reporter Reporter to use for report construction if validation fails.
 */
template <typename MemberT, typename T, typename ReporterT>
auto make_prevalidation_adapter(
                            MemberT&& member,
                            T&& val,
                            ReporterT&& reporter,
                            std::enable_if_t<
                                    hana::is_a<reporter_tag,ReporterT> && hana::is_a<strict_any_tag,T>,
                                    void*>
                            =nullptr)
{
    auto res=prevalidation_adapter<MemberT,T,ReporterT,typename std::decay_t<T>::type>(std::forward<MemberT>(member),std::forward<T>(val),std::forward<ReporterT>(reporter));
    res.traits().next_adapter_impl().set_strict_any(true);
    return res;
}

/**
 * @brief Create prevalidation adapter with default reporter.
 * @param member Member to validate.
 * @param val Value to validate with.
 * @param dst Destination object where to put validation report if validation fails.
 */
template <typename MemberT, typename T, typename DstT>
auto make_prevalidation_adapter(
                            MemberT&& member,
                            T&& val,
                            DstT& dst,
                            std::enable_if_t<
                                    !hana::is_a<reporter_tag,DstT>,
                                    void*>
                            =nullptr)
{
    return make_prevalidation_adapter(std::forward<MemberT>(member),std::forward<T>(val),make_reporter(dst));
}

template <typename AdapterT, typename IteratorT>
struct adapter_with_aggregation_iterator_t<AdapterT,IteratorT,
                                            hana::when<hana::is_a<prevalidation_adapter_tag,typename AdapterT::type>>
                                          >
{
    template <typename AdapterT1, typename IteratorT1>
    auto operator() (AdapterT1&& adapter, IteratorT1&& it) const
    {
        // cleanup traits type
        auto& traits=std::add_lvalue_reference_t<std::decay_t<decltype(adapter.traits())>>(adapter.traits());

        // create adapter with iterator element
        auto res=make_prevalidation_adapter(traits.next_adapter_impl().member(),*it,traits.reporter());

        // set member_checked flag
        res.traits().next_adapter_impl().set_member_checked(true);

        // done
        return res;
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PREVALIDATION_ADAPTER_HPP
