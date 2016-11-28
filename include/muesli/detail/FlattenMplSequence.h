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

#ifndef MUESLI_DETAIL_FLATTENMPLSEQUENCE_H_
#define MUESLI_DETAIL_FLATTENMPLSEQUENCE_H_

#include <type_traits>

#include <boost/mpl/copy.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/vector.hpp>

namespace muesli
{
namespace detail
{

// forward declare for recursion
template <typename Sequence>
struct FlattenMplSequence;

struct FlattenImpl
{
    template <class Seq1, class Seq2, typename Enable = void>
    struct apply
    {
        using type = typename boost::mpl::push_back<Seq1, Seq2>::type;
    };

    template <class Seq1, class Seq2>
    struct apply<Seq1, Seq2, std::enable_if_t<boost::mpl::is_sequence<Seq2>::value>>
    {
        using type = typename boost::mpl::copy<typename FlattenMplSequence<Seq2>::type,
                                               boost::mpl::back_inserter<Seq1>>::type;
    };
};

template <typename Sequence>
struct FlattenMplSequence : boost::mpl::fold<Sequence, boost::mpl::vector<>, FlattenImpl>
{
};

} // namespace detail
} // namespace muesli

#endif // MUESLI_DETAIL_FLATTENMPLSEQUENCE_H_
