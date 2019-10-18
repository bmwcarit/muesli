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

#ifndef MUESLI_STREAMS_STRINGISTREAM_H_
#define MUESLI_STREAMS_STRINGISTREAM_H_

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

    explicit BasicStringIStream(const StringType& input)
            : _currentCharIndex(0), _inputLength(input.length()), _input(input)
    {
        checkInputForNullCharTermination();
    }

    explicit BasicStringIStream(StringType&& input)
            : _currentCharIndex(0), _inputLength(input.length()), _input(std::move(input))
    {
        checkInputForNullCharTermination();
    }

    Char peek() const
    {
        // No bound check because we assume that the string implementation returns
        // \0 for length + 1
        return _input[_currentCharIndex];
    }

    Char get()
    {
        // No bound check because we assume that the string implementation returns
        // \0 for length + 1
        return _input[_currentCharIndex++];
    }

    void get(Char* destination, std::size_t destinationSize)
    {
        if (destination == nullptr || destinationSize == 0) {
            return;
        }

        std::size_t copyableCharacterCount =
                std::min(destinationSize, _inputLength - _currentCharIndex);

        _input.copy(destination, copyableCharacterCount, _currentCharIndex);

        _currentCharIndex += copyableCharacterCount;

        if (copyableCharacterCount < destinationSize) {
            destination[copyableCharacterCount] = '\0';
        }
    }

    std::size_t tell() const
    {
        return _currentCharIndex;
    }

    // non-copyable
    BasicStringIStream(const BasicStringIStream&) = delete;
    BasicStringIStream& operator=(const BasicStringIStream&) = delete;

    BasicStringIStream(BasicStringIStream&&) = default;
    BasicStringIStream& operator=(BasicStringIStream&&) = default;
    ~BasicStringIStream() = default;

private:
    void checkInputForNullCharTermination() const
    {
        assert(_input[_inputLength] == '\0');
    }

    std::size_t _currentCharIndex;
    const std::size_t _inputLength;

    const StringType _input;
};

using StringIStream = BasicStringIStream<std::string>;

} // namespace muesli

MUESLI_REGISTER_INPUT_STREAM(muesli::StringIStream)

#endif // MUESLI_STREAMS_STRINGISTREAM_H_
