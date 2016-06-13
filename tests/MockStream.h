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

#ifndef MUESLI_TESTS_MOCKSTREAM_H_
#define MUESLI_TESTS_MOCKSTREAM_H_

#include <gmock/gmock.h>

#include "muesli/StreamRegistry.h"

// implements OutputStream concept
class MockOutputStream
{
public:
    using Char = char;
    MOCK_METHOD1(put, void(char c));
    MOCK_METHOD2(write, void(const char* s, std::size_t size));
    MOCK_METHOD0(flush, void());
};

MUESLI_REGISTER_OUTPUT_STREAM(MockOutputStream);

// implements InputStream concept
class MockInputStream
{
public:
    using Char = char;
    MOCK_METHOD0(get, char());
    MOCK_METHOD2(get, void(char* s, std::size_t size));
    MOCK_METHOD0(peek, char());
    MOCK_METHOD0(tell, std::size_t());
};

MUESLI_REGISTER_INPUT_STREAM(MockInputStream);

#endif // MUESLI_TESTS_MOCKSTREAM_H_
