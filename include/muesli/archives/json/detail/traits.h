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
#include <memory>
#include <cstddef>

#include <boost/optional.hpp>

#include "muesli/Traits.h"
#include "muesli/TypeRegistryFwd.h"

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
    static constexpr bool value = std::is_same<std::vector<bool>::const_reference, T>::value ||
                                  std::is_same<std::vector<bool>::reference, T>::value ||
                                  std::is_same<std::string, T>::value || !std::is_class<T>::value ||
                                  std::is_same<std::nullptr_t, T>::value;
};

template <typename T>
struct IsNullable : std::false_type
{
};

template <typename T>
struct IsNullable<std::shared_ptr<T>> : std::true_type
{
};

template <typename T>
struct IsNullable<std::unique_ptr<T>> : std::true_type
{
};

template <typename T>
struct IsNullable<boost::optional<T>> : std::true_type
{
};

template <typename T>
struct IsObject
{
    static constexpr bool value =
            !IsNullable<T>::value && !IsPrimitive<T>::value && !IsArray<T>::value;
};

// any signed integer which is below int64_t
template <typename T>
struct IsSignedIntegerUpTo32bit
{
    static constexpr bool value =
            IsTypeWithinList<T, std::int8_t, std::int16_t, std::int32_t>::value;
};

// any unsigned integer which is below uint64_t
template <typename T>
struct IsUnsignedIntegerUpTo32bit
{
    static constexpr bool value =
            IsTypeWithinList<T, std::uint8_t, std::uint16_t, std::uint32_t>::value;
};

template <typename T, typename Enable = void>
struct IsMap : std::false_type
{
};

template <typename T>
struct IsMap<T, VoidT<typename std::decay_t<T>::mapped_type>> : std::true_type
{
};

template <typename T, typename Enable = void>
struct HasRegisteredTypeName : std::false_type
{
};

template <typename T>
struct HasRegisteredTypeName<T, VoidT<decltype(muesli::RegisteredType<std::decay_t<T>>::name())>>
        : std::true_type
{
};

} // namespace detail
} // namespace json
} // namespace muesli

#endif // MUESLI_ARCHIVES_JSON_DETAIL_TRAITS_H_
