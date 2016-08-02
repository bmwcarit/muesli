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

#include "muesli/Tags.h"

#include "muesli/detail/IncrementalTypeList.h"
#include "muesli/detail/DelayStaticAssert.h"
#include "muesli/detail/TemplateHolder.h"

namespace muesli
{
template <typename ArchiveTag>
struct InputArchiveTraits
{
    static_assert(detail::DelayStaticAssert<ArchiveTag>::value,
                  "no InputArchive registered for this ArchiveTag");
};

template <typename ArchiveTag>
struct OutputArchiveTraits
{
    static_assert(detail::DelayStaticAssert<ArchiveTag>::value,
                  "no OutputArchive registered for this ArchiveTag");
};

template <typename Archive>
struct TagTraits
{
    static_assert(detail::DelayStaticAssert<Archive>::value, "no Tag registered for this Archive");
};
} // namespace muesli

#define MUESLI_REGISTER_ARCHIVE(Category, Archive, ArchiveTag)                                     \
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
    template <>                                                                                    \
    struct Category##Traits<ArchiveTag>                                                            \
    {                                                                                              \
        template <typename... Ts>                                                                  \
        using type = Archive<Ts...>;                                                               \
    };                                                                                             \
    template <typename... Ts>                                                                      \
    struct TagTraits<Archive<Ts...>>                                                               \
    {                                                                                              \
        using type = ArchiveTag;                                                                   \
    };                                                                                             \
    } /*namespace muesli */                                                                        \
    MUESLI_ADD_TO_INCREMENTAL_TYPELIST(                                                            \
            muesli::tags::Category, muesli::detail::TemplateHolder<Archive>)

#define MUESLI_REGISTER_INPUT_ARCHIVE(Archive, ArchiveTag)                                         \
    MUESLI_REGISTER_ARCHIVE(InputArchive, Archive, ArchiveTag)

#define MUESLI_REGISTER_OUTPUT_ARCHIVE(Archive, ArchiveTag)                                        \
    MUESLI_REGISTER_ARCHIVE(OutputArchive, Archive, ArchiveTag)

#endif // MUESLI_ARCHIVEREGISTRY_H_
