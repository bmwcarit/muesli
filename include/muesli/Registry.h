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

#include <string>
#include <type_traits>

#include <boost/variant.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/identity.hpp>

#include "muesli/detail/FlattenMplSequence.h"
#include "muesli/detail/IncrementalTypeList.h"
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
constexpr std::size_t ListSize = boost::mpl::size<List>::value;

static_assert(0 != ListSize<RegisteredOutputStreams>, "no OutputStream registered");
static_assert(0 != ListSize<RegisteredInputStreams>, "no InputStream registered");
static_assert(0 != ListSize<RegisteredOutputArchives>, "no OutputArchive registered");
static_assert(0 != ListSize<RegisteredInputArchives>, "no InputArchive registered");

template <typename Stream, typename RegisteredArchive>
struct ApplyStream
{
    // get actual Archive template out of TemplateHolder
    // instantiate the combination of Archive template and concrete Stream implementation
    using type = typename RegisteredArchive::template type<Stream>;
};

template <typename RegisteredArchive, typename RegisteredStreams>
struct CartesianStreamProduct
{
    // iterate over registered streams
    using type =
            typename boost::mpl::transform<RegisteredStreams,
                                           ApplyStream<boost::mpl::_1, RegisteredArchive>>::type;
};

// iterate over registered archives
template <typename RegisteredArchives, typename RegisteredStreams>
using CartesianStreamArchiveProduct = typename boost::mpl::transform<
        RegisteredArchives,
        CartesianStreamProduct<boost::mpl::_1, RegisteredStreams>>::type;

// flatten nested type lists
template <typename RegisteredArchives, typename RegisteredStreams>
using FlatCartesianStreamArchiveProduct = typename FlattenMplSequence<
        typename CartesianStreamArchiveProduct<RegisteredArchives, RegisteredStreams>::type>::type;
} // namespace detail

template <typename RegisteredArchives,
          typename RegisteredStreams,
          template <typename> class Postprocess = boost::mpl::identity>
using MakeArchiveVariant = typename boost::make_variant_over<typename boost::mpl::transform<
        detail::FlatCartesianStreamArchiveProduct<RegisteredArchives, RegisteredStreams>,
        Postprocess<boost::mpl::_1>>::type>::type;

using OutputArchiveVariant = MakeArchiveVariant<RegisteredOutputArchives,
                                                RegisteredOutputStreams,
                                                std::add_lvalue_reference>;
using InputArchiveVariant = MakeArchiveVariant<RegisteredInputArchives,
                                               RegisteredInputStreams,
                                               std::add_lvalue_reference>;

} // namespace muesli

#endif // MUESLI_TYPEREGISTRY_H_
