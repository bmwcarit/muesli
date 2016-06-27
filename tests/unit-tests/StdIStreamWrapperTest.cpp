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

#include <string>
#include <sstream>
#include <fstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/concept_check.hpp>

#include "muesli/concepts/InputStream.h"
#include "muesli/streams/StdIStreamWrapper.h"

template <typename Stream>
using StdIStreamWrapperTest = ::testing::Test;

using StdIStreamTypes = ::testing::Types<std::ifstream, std::stringstream, std::istringstream>;
TYPED_TEST_CASE(StdIStreamWrapperTest, StdIStreamTypes);

TYPED_TEST(StdIStreamWrapperTest, conceptCheck)
{
    using WrappedIStream = muesli::StdIStreamWrapper<TypeParam>;
    BOOST_CONCEPT_ASSERT((muesli::concepts::InputStream<WrappedIStream>));
}

TEST(StdIStreamWrapperTest, readCharacterWiseFromStringStreamThroughWrapper)
{
    using WrappedIStream = muesli::StdIStreamWrapper<std::stringstream>;
    const std::string expectedStr = "TEST";
    std::stringstream stream(expectedStr);
    WrappedIStream wrappedStream(stream);
    for (const char c : expectedStr) {
        EXPECT_EQ(c, wrappedStream.get());
    }
}

TEST(StdIStreamWrapperTest, readMultipleCharactersFromStringStreamThroughWrapper)
{
    using WrappedIStream = muesli::StdIStreamWrapper<std::stringstream>;
    const std::string expectedStr = "TEST";
    std::stringstream stream(expectedStr);
    WrappedIStream wrappedStream(stream);
    char str[5];
    wrappedStream.get(&str[0], 5);
    ASSERT_STREQ(expectedStr.c_str(), str);
}
