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

#ifndef MUESLI_STREAMS_STRINGOSTREAM_H_
#define MUESLI_STREAMS_STRINGOSTREAM_H_

#include <string>
#include "muesli/StreamRegistry.h"

namespace muesli
{

template <typename StringType>
class BasicStringOStream
{
public:
    using Char = typename StringType::value_type;

    explicit BasicStringOStream(std::size_t initialSize = 4096) : _buffer()
    {
        _buffer.reserve(initialSize);
    }

    void put(Char c)
    {
        // If the string implementation uses an exponential growth strategy
        // (most std::basic_string implementations do), this operation is
        // fast enough.
        _buffer += c;
    }

    void write(const Char* s, std::size_t size)
    {
        _buffer.append(s, size);
    }

    void flush()
    {
        // This method is expected by muesli but doesn't do anything for this stream type.
    }

    const std::string& getString() const
    {
        return _buffer;
    }

    // non-copyable
    BasicStringOStream(const BasicStringOStream&) = delete;
    BasicStringOStream& operator=(const BasicStringOStream&) = delete;

    BasicStringOStream(BasicStringOStream&&) = default;
    BasicStringOStream& operator=(BasicStringOStream&&) = default;
    ~BasicStringOStream() = default;

private:
    StringType _buffer;
};

using StringOStream = BasicStringOStream<std::string>;

} // namespace muesli

MUESLI_REGISTER_OUTPUT_STREAM(muesli::StringOStream)

#endif // MUESLI_STREAMS_STRINGOSTREAM_H_
