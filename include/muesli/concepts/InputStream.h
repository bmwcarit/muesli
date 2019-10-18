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

#ifndef MUESLI_CONCEPTS_INPUTSTREAM_H_
#define MUESLI_CONCEPTS_INPUTSTREAM_H_

#include <boost/concept_check.hpp>
#include <cstddef>

namespace muesli
{
namespace concepts
{

template <class Stream>
struct InputStream
{
    using Char = typename Stream::Char;

    BOOST_CONCEPT_USAGE(InputStream)
    {
        // extract single character from stream
        _c = _stream.get();

        // extract `size` characters from stream
        _stream.get(_s, _size);

        _c = _stream.peek();

        _pos = _stream.tell();
    }

private:
    Stream _stream;
    Char* _s;
    Char _c;
    std::size_t _size;
    std::size_t _pos;
};

} // namespace concepts
} // namespace muesli

#endif // MUESLI_CONCEPTS_INPUTSTREAM_H_
