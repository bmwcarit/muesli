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

#ifndef MUESLI_ARCHIVEREGISTRY_H_
#define MUESLI_ARCHIVEREGISTRY_H_

#include "muesli/detail/IncrementalTypeList.h"
#include "muesli/Tags.h"

namespace muesli
{
namespace detail
{
// wrapper which holds a template as its `type`
template <template <typename...> class T>
struct TemplateHolder;
} // namespace detail
} // namespace muesli

#define MUESLI_REGISTER_ARCHIVE(Category, Archive)                                                 \
    namespace muesli                                                                               \
    {                                                                                              \
    namespace detail                                                                               \
    {                                                                                              \
    template <>                                                                                    \
    struct TemplateHolder<Archive>                                                                 \
    {                                                                                              \
        template <typename... Ts>                                                                  \
        using type = Archive<Ts...>;                                                               \
    };                                                                                             \
    } /*namespace detail */                                                                        \
    } /*namespace muesli */                                                                        \
    MUESLI_ADD_TO_INCREMENTAL_TYPELIST(Category, muesli::detail::TemplateHolder<Archive>)

#define MUESLI_REGISTER_INPUT_ARCHIVE(Archive)                                                     \
    MUESLI_REGISTER_ARCHIVE(muesli::tags::InputArchive, Archive)

#define MUESLI_REGISTER_OUTPUT_ARCHIVE(Archive)                                                    \
    MUESLI_REGISTER_ARCHIVE(muesli::tags::OutputArchive, Archive)

#endif // MUESLI_ARCHIVEREGISTRY_H_
