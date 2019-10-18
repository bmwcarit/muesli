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

#ifndef MUESLI_CONCEPTS_OUTPUTSTREAM_H_
#define MUESLI_CONCEPTS_OUTPUTSTREAM_H_

#include <boost/concept_check.hpp>

namespace muesli
{
namespace concepts
{

template <class Stream>
struct OutputStream
{
    using Char = typename Stream::Char;

    BOOST_CONCEPT_USAGE(OutputStream)
    {
        // insert a single character `c` into `stream`
        _stream.put(_c);

        // insert `size` characters pointed to by `s` into `stream`
        _stream.write(_s, _size);

        _stream.flush();
    }

private:
    Stream _stream;
    Char _c;

    const Char* _s;
    std::size_t _size;
};

} // namespace concepts
} // namespace muesli

#endif // MUESLI_CONCEPTS_OUTPUTSTREAM_H_
