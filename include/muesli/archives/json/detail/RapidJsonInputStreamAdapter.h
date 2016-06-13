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

#ifndef MUESLI_ARCHIVES_JSON_DETAIL_RAPIDJSONINPUTSTREAMADAPTER_H_
#define MUESLI_ARCHIVES_JSON_DETAIL_RAPIDJSONINPUTSTREAMADAPTER_H_

#include <cassert>

namespace muesli
{
namespace json
{
namespace detail
{

// provides a wrapper around a muesli InputStream and implements RapidJSON's stream concept
template <typename InputStream>
class RapidJsonInputStreamAdapter
{
public:
    using Ch = typename InputStream::Char;

    explicit RapidJsonInputStreamAdapter(InputStream& inputStream) : inputStream(inputStream)
    {
    }

    Ch Peek() const
    {
        return inputStream.peek();
    }

    std::size_t Tell() const
    {
        return inputStream.tell();
    }

    Ch Take()
    {
        return inputStream.get();
    }

    // this method has to be implemented but is not currently not used
    // as long as we do not use RapidJSON's insitu parsing
    Ch* PutBegin()
    {
        assert(false);
        return nullptr;
    }

    // this method has to be implemented but is not currently not used
    // as long as we do not use RapidJSON's insitu parsing
    std::size_t PutEnd(Ch*)
    {
        assert(false);
        return 0;
    }

    // this method has to be implemented but is not currently not used
    void Put(Ch)
    {
        assert(false);
    }

private:
    InputStream& inputStream;
};

} // namespace detail
} // namespace json
} // namespace muesli

#endif // MUESLI_ARCHIVES_JSON_DETAIL_RAPIDJSONINPUTSTREAMADAPTER_H_
