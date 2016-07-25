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

#ifndef MUESLI_DETAIL_TYPELIST_H_
#define MUESLI_DETAIL_TYPELIST_H_

#include <boost/mpl/fold.hpp>

namespace muesli
{
namespace detail
{

template <typename... Ts>
struct TypeList
{
};

template <typename T, typename TList>
struct Append;

template <typename T, typename... Ts>
struct Append<T, TypeList<Ts...>>
{
    using type = TypeList<Ts..., T>;
};

template <typename MplSequence>
struct MplSequenceToTypeList
{
    using type = typename boost::mpl::fold<MplSequence,
                                           TypeList<>,
                                           Append<boost::mpl::_2, boost::mpl::_1>>::type;
};

template <typename TList>
struct TypeListToMplVector;

template <typename... Ts>
struct TypeListToMplVector<TypeList<Ts...>>
{
    using type = typename boost::mpl::vector<Ts...>;
};

} // namespace detail
} // namespace muesli

#endif // MUESLI_DETAIL_TYPELIST_H_
