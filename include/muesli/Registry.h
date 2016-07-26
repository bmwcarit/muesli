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

#ifndef MUESLI_REGISTRY_H_
#define MUESLI_REGISTRY_H_

#include <type_traits>

#include <boost/variant.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/size.hpp>

#include "muesli/detail/FlattenMplSequence.h"
#include "muesli/detail/IncrementalTypeList.h"
#include "muesli/detail/TypeList.h"
#include "muesli/detail/CartesianTypeProduct.h"
#include "muesli/Tags.h"

namespace muesli
{

using RegisteredOutputStreams = MUESLI_GET_INCREMENTAL_TYPELIST(muesli::tags::OutputStream);
using RegisteredInputStreams = MUESLI_GET_INCREMENTAL_TYPELIST(muesli::tags::InputStream);

using RegisteredOutputArchives = MUESLI_GET_INCREMENTAL_TYPELIST(muesli::tags::OutputArchive);
using RegisteredInputArchives = MUESLI_GET_INCREMENTAL_TYPELIST(muesli::tags::InputArchive);

namespace detail
{

template <typename List>
using ListSize = boost::mpl::size<List>;

static_assert(0 != ListSize<RegisteredOutputStreams>::value, "no OutputStream registered");
static_assert(0 != ListSize<RegisteredInputStreams>::value, "no InputStream registered");
static_assert(0 != ListSize<RegisteredOutputArchives>::value, "no OutputArchive registered");
static_assert(0 != ListSize<RegisteredInputArchives>::value, "no InputArchive registered");

struct CombineArchiveAndStream
{
    // get actual Archive template out of TemplateHolder
    // instantiate the combination of Archive template and concrete Stream implementation
    template <typename RegisteredArchive, typename Stream>
    struct apply
    {
        using type = typename RegisteredArchive::template type<Stream>;
    };
};
} // namespace detail

using OutputArchiveTypeVector = detail::FlatCartesianTypeProduct<RegisteredOutputArchives,
                                                                 RegisteredOutputStreams,
                                                                 detail::CombineArchiveAndStream>;
using InputArchiveTypeVector = detail::FlatCartesianTypeProduct<RegisteredInputArchives,
                                                                RegisteredInputStreams,
                                                                detail::CombineArchiveAndStream>;

template <typename ArchiveTypeVector, template <typename> class Postprocess = boost::mpl::identity>
using MakeArchiveVariant = typename boost::make_variant_over<
        typename boost::mpl::transform<ArchiveTypeVector, Postprocess<boost::mpl::_1>>::type>::type;

using OutputArchiveVariant = MakeArchiveVariant<OutputArchiveTypeVector, std::add_lvalue_reference>;
using InputArchiveVariant = MakeArchiveVariant<InputArchiveTypeVector, std::add_lvalue_reference>;

} // namespace muesli

#endif // MUESLI_TYPEREGISTRY_H_
