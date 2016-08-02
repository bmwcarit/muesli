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

#ifndef MUESLI_TRAITS_H_
#define MUESLI_TRAITS_H_

#include <type_traits>

namespace muesli
{

// this traits class is used to get the wrapper class for an enum
template <typename Enum>
struct EnumTraits;

// this traits class is used to signal that for this type intro/outro shall not be called
template <typename T>
struct SkipIntroOutroTraits : std::false_type
{
};
} // namespace muesli

#endif // MUESLI_TRAITS_H_
