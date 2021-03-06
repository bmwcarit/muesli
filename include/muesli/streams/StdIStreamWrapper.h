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

#ifndef MUESLI_STREAMS_STDISTREAMWRAPPER_H_
#define MUESLI_STREAMS_STDISTREAMWRAPPER_H_

#include <cstdint>
#include <iosfwd>

#include "muesli/StreamRegistry.h"

namespace muesli
{

/**
 * This template class provides a generic wrapper around streams which derive from
 * std::basic_istream in order to implement the InputStream concept.
 */
template <typename Stream>
class StdIStreamWrapper
{
public:
    using Char = typename Stream::char_type;

    explicit StdIStreamWrapper(Stream& stream) : _stream(stream)
    {
    }

    Char get()
    {
        return _stream.get();
    }

    void get(Char* s, std::size_t size)
    {
        _stream.get(s, static_cast<std::int64_t>(size));
    }

    std::size_t tell() const
    {
        return static_cast<std::size_t>(_stream.tellg());
    }

    Char peek() const
    {
        typename Stream::int_type res = _stream.peek();
        if (res != Stream::traits_type::eof()) {
            return static_cast<Char>(res);
        }
        return '\0';
    }

    // non-copyable
    StdIStreamWrapper(const StdIStreamWrapper&) = delete;
    StdIStreamWrapper& operator=(const StdIStreamWrapper&) = delete;

    StdIStreamWrapper(StdIStreamWrapper&&) = default;
    StdIStreamWrapper& operator=(StdIStreamWrapper&&) = default;
    ~StdIStreamWrapper() = default;

private:
    Stream& _stream;
};
} // namespace muesli

MUESLI_REGISTER_INPUT_STREAM(muesli::StdIStreamWrapper<std::istream>)

#endif // MUESLI_STREAMS_STDISTREAMWRAPPER_H_
