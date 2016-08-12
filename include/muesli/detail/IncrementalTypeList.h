/*
 * #%L
 * %%
 * Copyright (C) 2011 - 2016 BMW Car IT GmbH
 * %%
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * #L%
 */

#ifndef MUESLI_DETAIL_INCREMENTALTYPELIST_H_
#define MUESLI_DETAIL_INCREMENTALTYPELIST_H_

#include <utility>

#include <boost/mpl/push_back.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>

namespace muesli
{
namespace detail
{

using MaxIncrementalTypeListSize = std::integral_constant<std::size_t, BOOST_MPL_LIMIT_VECTOR_SIZE>;

template <std::size_t i>
struct IncrementalTypeListIndex : IncrementalTypeListIndex<i - 1>
{
};

template <>
struct IncrementalTypeListIndex<0>
{
};

// initially empty type list
template <class TypeListTag>
boost::mpl::vector<> IncrementalTypeList(TypeListTag&, IncrementalTypeListIndex<0>);

} // namespace detail
} // namespace muesli

// retrieve a boost::mpl::vector which contains all types that were added to this specific list
// this has to be a macro instead of an alias template due to GCC's delayed template instantiation
#define MUESLI_GET_INCREMENTAL_TYPELIST(TypeListTag)                                               \
    decltype(muesli::detail::IncrementalTypeList(                                                  \
            std::declval<TypeListTag&>(),                                                          \
            muesli::detail::IncrementalTypeListIndex<                                              \
                    muesli::detail::MaxIncrementalTypeListSize::value>{}))

#define MUESLI_ADD_TO_INCREMENTAL_TYPELIST(TypeListTag, Type)                                      \
    namespace muesli                                                                               \
    {                                                                                              \
    namespace detail                                                                               \
    {                                                                                              \
    static_assert(boost::mpl::size<MUESLI_GET_INCREMENTAL_TYPELIST(TypeListTag)>::value <          \
                          MaxIncrementalTypeListSize::value,                                       \
                  "max types exceeded for tag " #TypeListTag);                                     \
    boost::mpl::push_back<MUESLI_GET_INCREMENTAL_TYPELIST(TypeListTag), Type>::type                \
    IncrementalTypeList(                                                                           \
            TypeListTag&,                                                                          \
            IncrementalTypeListIndex<                                                              \
                    boost::mpl::size<MUESLI_GET_INCREMENTAL_TYPELIST(TypeListTag)>::value + 1>);   \
    } /* namespace detail */                                                                       \
    } /* namespace muesli */

#endif // MUESLI_DETAIL_INCREMENTALTYPELIST_H_
