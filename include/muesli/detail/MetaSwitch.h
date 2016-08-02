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

#ifndef MUESLI_DETAIL_METASWITCH_H_
#define MUESLI_DETAIL_METASWITCH_H_

#include <type_traits>

namespace muesli
{
namespace detail
{

template <bool V, typename T>
struct MetaCase
{
    static constexpr bool value = V;
    using type = T;
};

template <typename T>
struct MetaDefault
{
    static constexpr bool value = true;
    using type = T;
};

template <typename Head, typename... Tail>
struct MetaSwitch : std::conditional_t<Head::value, Head, MetaSwitch<Tail...>>
{
};

template <typename T>
struct MetaSwitch<MetaDefault<T>>
{
    using type = T;
};

template <typename Head, typename... Tail>
using MetaSwitchT = typename MetaSwitch<Head, Tail...>::type;
} // namespace detail
} // namespace muesli

#endif // MUESLI_DETAIL_METASWITCH_H_
