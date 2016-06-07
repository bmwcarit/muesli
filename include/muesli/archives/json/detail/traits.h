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

#ifndef MUESLI_ARCHIVES_JSON_DETAIL_TRAITS_H_
#define MUESLI_ARCHIVES_JSON_DETAIL_TRAITS_H_

#include <type_traits>
#include <string>
#include <vector>

namespace muesli
{
namespace json
{
namespace detail
{

template <typename T>
struct IsArray : std::false_type
{
};

template <typename T>
struct IsArray<std::vector<T>> : std::true_type
{
};

template <typename T>
struct IsPrimitive
{
    static constexpr bool value = std::is_same<std::string, T>::value || !std::is_class<T>::value;
};

template <typename T>
struct IsObject
{
    static constexpr bool value = !IsPrimitive<T>::value && !IsArray<T>::value;
};

// this traits class is used to get the wrapper class for an enum
template <typename Enum>
struct EnumTraits;

} // namespace detail
} // namespace json
} // namespace muesli

#endif // MUESLI_ARCHIVES_JSON_DETAIL_TRAITS_H_
