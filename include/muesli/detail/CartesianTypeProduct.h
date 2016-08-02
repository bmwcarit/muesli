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

#ifndef MUESLI_DETAIL_CARTESIANTYPEPRODUCT_H_
#define MUESLI_DETAIL_CARTESIANTYPEPRODUCT_H_

#include <boost/mpl/transform.hpp>

#include "muesli/detail/FlattenMplSequence.h"

namespace muesli
{
namespace detail
{

template <typename X, typename Y, typename CombineOperation>
struct ApplyCombineOperation
{
    using type = typename CombineOperation::template apply<X, Y>::type;
};

// iterate over Ys and apply Transformation
template <typename X, typename Ys, typename CombineOperation>
struct InnerCartesianTypeProduct
{
    using type = typename boost::mpl::transform<
            Ys,
            ApplyCombineOperation<X, boost::mpl::_1, CombineOperation>>::type;
};

// iterate over Xs
template <typename Xs, typename Ys, typename CombineOperation>
using CartesianTypeProduct = typename boost::mpl::transform<
        Xs,
        InnerCartesianTypeProduct<boost::mpl::_1, Ys, CombineOperation>>::type;

// flatten nested type lists
template <typename Xs, typename Ys, typename CombineOperation>
using FlatCartesianTypeProduct = typename FlattenMplSequence<
        typename CartesianTypeProduct<Xs, Ys, CombineOperation>::type>::type;

} // namespace detail
} // namespace muesli

#endif // MUESLI_DETAIL_CARTESIANTYPEPRODUCT_H_
