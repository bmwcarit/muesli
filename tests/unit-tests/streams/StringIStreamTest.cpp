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

#include "muesli/concepts/InputStream.h"
#include "muesli/streams/StringIStream.h"

using namespace ::testing;

template <typename Stream>
using StringIStreamTest = ::testing::Test;

struct RValueReferenceString
{
    static auto getStream(std::string str)
    {
        return muesli::StringIStream(std::move(str));
    }
};

struct LValueReferenceString
{
    static auto getStream(std::string str)
    {
        return muesli::StringIStream(str);
    }
};

using StringInitializationTypes = ::testing::Types<RValueReferenceString, LValueReferenceString>;
TYPED_TEST_CASE(StringIStreamTest, StringInitializationTypes);

TEST(StringIStreamTest, conceptCheck)
{
    BOOST_CONCEPT_ASSERT((muesli::concepts::InputStream<muesli::StringIStream>));
}

TYPED_TEST(StringIStreamTest, getOnEmptyStringReturnsEof)
{
    auto stream = TypeParam::getStream(std::string());

    ASSERT_EQ('\0', stream.get());
}

TYPED_TEST(StringIStreamTest, getCharsOnEmptyStringReturnsEof)
{
    auto stream = TypeParam::getStream(std::string());
    std::array<muesli::StringIStream::Char, 1> dest;

    stream.get(dest.data(), dest.size());

    ASSERT_EQ('\0', dest.at(0));
}

TYPED_TEST(StringIStreamTest, getCharUntilEof)
{
    auto stream = TypeParam::getStream("12");

    ASSERT_EQ('1', stream.get());
    ASSERT_EQ('2', stream.get());
    ASSERT_EQ('\0', stream.get());
}

TYPED_TEST(StringIStreamTest, getChars_RequestMoreCharsThanAvailable)
{
    auto stream = TypeParam::getStream("12");
    std::array<muesli::StringIStream::Char, 3> dest;

    stream.get(dest.data(), dest.size());

    ASSERT_EQ('1', dest.at(0));
    ASSERT_EQ('2', dest.at(1));
    ASSERT_EQ('\0', dest.at(2));
}

TYPED_TEST(StringIStreamTest, getChars_RequestAllChars)
{
    auto stream = TypeParam::getStream("12");
    std::array<muesli::StringIStream::Char, 2> dest;

    stream.get(dest.data(), dest.size());

    ASSERT_EQ('1', dest.at(0));
    ASSERT_EQ('2', dest.at(1));
}

TYPED_TEST(StringIStreamTest, getCharsThanGetChar)
{
    auto stream = TypeParam::getStream("123");
    std::array<muesli::StringIStream::Char, 2> dest;

    stream.get(dest.data(), dest.size());

    ASSERT_EQ('1', dest.at(0));
    ASSERT_EQ('2', dest.at(1));
    ASSERT_EQ('3', stream.get());
}

TYPED_TEST(StringIStreamTest, getCharThanGetChars)
{
    auto stream = TypeParam::getStream("123");
    std::array<muesli::StringIStream::Char, 2> dest;

    ASSERT_EQ('1', stream.get());

    stream.get(dest.data(), dest.size());

    ASSERT_EQ('2', dest.at(0));
    ASSERT_EQ('3', dest.at(1));
}

TYPED_TEST(StringIStreamTest, tell)
{
    auto stream = TypeParam::getStream("12");

    ASSERT_EQ(0, stream.tell());

    stream.get();
    ASSERT_EQ(1, stream.tell());

    stream.get();
    ASSERT_EQ(2, stream.tell());

    muesli::StringIStream::Char output = stream.get();
    ASSERT_EQ('\0', output);
}

TYPED_TEST(StringIStreamTest, peekDoesNotChangePosition)
{
    auto stream = TypeParam::getStream("12");

    ASSERT_EQ('1', stream.peek());
    ASSERT_EQ(0, stream.tell());

    ASSERT_EQ('1', stream.peek());
    ASSERT_EQ(0, stream.tell());
}

TYPED_TEST(StringIStreamTest, peekReturnsNullTerminalAtTheEnd)
{
    auto stream = TypeParam::getStream("1");

    stream.get();

    ASSERT_EQ('\0', stream.peek());
}

TYPED_TEST(StringIStreamTest, initWithLongString)
{
    auto stream = TypeParam::getStream(std::string(1000, '#'));
    ASSERT_EQ('#', stream.peek());
}
