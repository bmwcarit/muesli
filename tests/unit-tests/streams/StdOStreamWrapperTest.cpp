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

#include <fstream>
#include <sstream>
#include <string>

#include <boost/concept_check.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "muesli/concepts/OutputStream.h"
#include "muesli/streams/StdOStreamWrapper.h"

template <typename Stream>
using StdOStreamWrapperTest = ::testing::Test;

using StdOStreamTypes = ::testing::Types<std::ofstream, std::stringstream, std::ostringstream>;

TYPED_TEST_CASE(StdOStreamWrapperTest, StdOStreamTypes);

TYPED_TEST(StdOStreamWrapperTest, conceptCheck)
{
    using WrappedStream = muesli::StdOStreamWrapper<TypeParam>;
    BOOST_CONCEPT_ASSERT((muesli::concepts::OutputStream<WrappedStream>));
}

TEST(StdOStreamWrapperTest, writeCharacterWiseToStringStreamThroughWrapper)
{
    using WrappedOStream = muesli::StdOStreamWrapper<std::stringstream>;
    const std::string expectedStr = "TEST";
    std::stringstream stream;
    WrappedOStream wrappedStream(stream);
    for (const char c : expectedStr) {
        wrappedStream.put(c);
    }
    EXPECT_EQ(expectedStr, stream.str());
}

TEST(StdOStreamWrapperTest, writeMultipleCharactersToStringStreamThroughWrapper)
{
    using WrappedOStream = muesli::StdOStreamWrapper<std::stringstream>;
    const std::string expectedStr = "TEST";
    std::stringstream stream;
    WrappedOStream wrappedStream(stream);
    wrappedStream.write(expectedStr.data(), 4);
    EXPECT_EQ(expectedStr, stream.str());
}
