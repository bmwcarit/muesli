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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/concept_check.hpp>

#include <muesli/concepts/InputStream.h>

// implements InputStream concept
class MockInputStream
{
public:
    using Char = char;
    MOCK_METHOD0(get, char());
    MOCK_METHOD2(get, void(char* s, std::size_t size));
};

TEST(InputStreamTest, ConceptCheck)
{
    BOOST_CONCEPT_ASSERT((muesli::concepts::InputStream<MockInputStream>));
}
