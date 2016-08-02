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

#ifndef MUESLI_BASICSTRINGISTREAM_H_
#define MUESLI_BASICSTRINGISTREAM_H_

#include <string>
#include "muesli/StreamRegistry.h"

namespace muesli
{

/*
 * The used string type must return '\0' for string[string.length]
 */
template <typename StringType>
class BasicStringIStream
{
public:
    using Char = typename StringType::value_type;

    BasicStringIStream(const StringType& input)
            : currentCharIndex(0), inputLength(input.length()), input(input)
    {
        assert(input[inputLength] == '\0');
    }

    BasicStringIStream(const StringType&& input)
            : currentCharIndex(0), inputLength(input.length()), input(std::move(input))
    {
        assert(input[inputLength] == '\0');
    }

    Char peek() const
    {
        // No bound check because we assume that the string implementation returns
        // \0 for length + 1
        return input[currentCharIndex];
    }

    Char get()
    {
        // No bound check because we assume that the string implementation returns
        // \0 for length + 1
        return input[currentCharIndex++];
    }

    void get(Char* destination, std::size_t destinationSize)
    {
        if (destination == nullptr || destinationSize == 0) {
            return;
        }

        std::size_t copyableCharacterCount =
                std::min(destinationSize, inputLength - currentCharIndex);

        input.copy(destination, copyableCharacterCount, currentCharIndex);

        currentCharIndex += copyableCharacterCount;

        if (copyableCharacterCount < destinationSize) {
            destination[copyableCharacterCount] = '\0';
        }
    }

    std::size_t tell() const
    {
        return currentCharIndex;
    }

private:
    std::size_t currentCharIndex;
    const std::size_t inputLength;

    const StringType input;
};

using StringIStream = BasicStringIStream<std::string>;

} // namespace muesli

MUESLI_REGISTER_INPUT_STREAM(muesli::StringIStream)

#endif // MUESLI_BASICSTRINGISTREAM_H_
