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

#ifndef MUESLI_STREAMS_STDOSTREAMWRAPPER_H_
#define MUESLI_STREAMS_STDOSTREAMWRAPPER_H_

#include <iosfwd>

#include "muesli/StreamRegistry.h"

namespace muesli
{

/**
 * This template class provides a generic wrapper around streams which derive from
 * std::basic_ostream in order to implement the OutputStream concept.
 */
template <typename Stream>
class StdOStreamWrapper
{
public:
    using Char = typename Stream::char_type;

    explicit StdOStreamWrapper(Stream& stream) : stream(stream)
    {
    }

    void put(Char c)
    {
        stream.put(c);
    }

    void write(const Char* s, std::size_t size)
    {
        stream.write(s, size);
    }

    void flush()
    {
        stream.flush();
    }

    // non-copyable
    StdOStreamWrapper(const StdOStreamWrapper&) = delete;
    StdOStreamWrapper& operator=(const StdOStreamWrapper&) = delete;

    StdOStreamWrapper(StdOStreamWrapper&&) = default;
    StdOStreamWrapper& operator=(StdOStreamWrapper&&) = default;
    ~StdOStreamWrapper() = default;

private:
    Stream& stream;
};
} // namespace muesli

// register the common std::ostream case
MUESLI_REGISTER_OUTPUT_STREAM(muesli::StdOStreamWrapper<std::ostream>)

#endif // MUESLI_STREAMS_STDOSTREAMWRAPPER_H_
