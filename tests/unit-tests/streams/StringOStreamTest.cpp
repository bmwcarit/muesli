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

#include <array>
#include <boost/concept_check.hpp>
#include <gtest/gtest.h>

#include "muesli/concepts/OutputStream.h"
#include "muesli/streams/StringOStream.h"

TEST(StringOStreamTest, conceptCheck)
{
    BOOST_CONCEPT_ASSERT((muesli::concepts::OutputStream<muesli::StringOStream>));
}

TEST(StringOStreamTest, getEmptyString)
{
    muesli::StringOStream stream;

    ASSERT_EQ("", stream.getString());
}

TEST(StringOStreamTest, putChar)
{
    muesli::StringOStream stream;

    stream.put('x');
    ASSERT_EQ("x", stream.getString());

    stream.put('y');
    ASSERT_EQ("xy", stream.getString());
}

TEST(StringOStreamTest, write)
{
    muesli::StringOStream stream;
    std::array<std::string::value_type, 2> charArray{{'x', 'y'}};

    stream.write(charArray.data(), charArray.size());

    ASSERT_EQ("xy", stream.getString());
}

TEST(StringOStreamTest, writeAndTriggerBufferResize)
{
    muesli::StringOStream stream(2);

    stream.put('x');
    stream.put('y');
    stream.put('z');
    stream.put('1');
    stream.put('2');
    stream.put('3');
    stream.put('4');

    ASSERT_EQ("xyz1234", stream.getString());
}
