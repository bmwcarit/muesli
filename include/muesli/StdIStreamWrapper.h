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

#ifndef MUESLI_STDISTREAMWRAPPER_H
#define MUESLI_STDISTREAMWRAPPER_H

namespace muesli
{

/**
 * This template class provides a generic wrapper around streams which derive from std::istream in
 * order to implement the InputStream concept.
 */
template <typename Stream>
class StdIStreamWrapper
{
public:
    using Char = typename Stream::char_type;

    StdIStreamWrapper(Stream& stream) : stream(stream)
    {
    }

    Char get()
    {
        return stream.get();
    }

    void get(Char* s, std::size_t size)
    {
        stream.get(s, size);
    }

    // non-copyable
    StdIStreamWrapper(const StdIStreamWrapper&) = delete;
    StdIStreamWrapper& operator=(const StdIStreamWrapper&) = delete;

private:
    Stream& stream;
};
} // namespace muesli

#endif // MUESLI_STDISTREAMWRAPPER_H
