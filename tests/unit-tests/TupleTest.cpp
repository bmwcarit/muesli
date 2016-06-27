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
#include <chrono>
#include <tuple>

#include <rapidjson/stringbuffer.h>

#include "muesli/streams/StringIStream.h"
#include "muesli/streams/StringOStream.h"

#include "muesli/archives/json/JsonInputArchive.h"
#include "muesli/archives/json/JsonOutputArchive.h"

#include "muesli/exceptions/ParseException.h"

#include "testtypes/TStruct.h"
#include "testtypes/TStructExtended.h"
#include "testtypes/TEnum.h"

using namespace ::testing;

class TupleTest : public testing::Test
{
public:
    TupleTest()
            : testStruct(0.123456789, 64, "test string data"),
              testEnumeration(muesli::tests::testtypes::TEnum::Enum::TLITERALA),
              simpleTuple(1, "test"),
              complexTuple(123, testEnumeration, testStruct, "teststring"),
              serializedSimpleTuple("[1,\"test\"]"),
              serializedComplexTuple(
                      R"([)"
                      R"(123,)"
                      R"("TLITERALA",)"
                      R"({"_typeName":"muesli.tests.testtypes.TStruct","tDouble":0.123456789,")"
                      R"(tInt64":64,")"
                      R"(tString":"test string data"},)"
                      R"("teststring")"
                      R"(])")
    {
    }

protected:
    muesli::tests::testtypes::TStruct testStruct;
    muesli::tests::testtypes::TEnum::Enum testEnumeration;

    std::tuple<std::int32_t, std::string> simpleTuple;
    std::tuple<std::int32_t,
               muesli::tests::testtypes::TEnum::Enum,
               muesli::tests::testtypes::TStruct,
               std::string> complexTuple;

    std::string serializedSimpleTuple;
    std::string serializedComplexTuple;
};

TEST_F(TupleTest, writeSimpleTuple)
{
    muesli::StringOStream stream;
    muesli::JsonOutputArchive<muesli::StringOStream> jsonOutputArchive(stream);

    jsonOutputArchive(simpleTuple);

    ASSERT_EQ(serializedSimpleTuple, stream.getString());
}

TEST_F(TupleTest, writeComplexTuple)
{
    muesli::StringOStream stream;
    muesli::JsonOutputArchive<muesli::StringOStream> jsonOutputArchive(stream);

    jsonOutputArchive(complexTuple);

    ASSERT_EQ(serializedComplexTuple, stream.getString());
}

TEST_F(TupleTest, readSimpleTuple)
{
    muesli::StringIStream stream(serializedSimpleTuple);
    muesli::JsonInputArchive<muesli::StringIStream> jsonInputArchive(stream);

    std::tuple<std::int32_t, std::string> readTuple;

    jsonInputArchive(readTuple);

    ASSERT_EQ(simpleTuple, readTuple);
}

TEST_F(TupleTest, readComplexTuple)
{
    muesli::StringIStream stream(serializedComplexTuple);
    muesli::JsonInputArchive<muesli::StringIStream> jsonInputArchive(stream);

    std::tuple<std::int32_t,
               muesli::tests::testtypes::TEnum::Enum,
               muesli::tests::testtypes::TStruct,
               std::string> readTuple;

    jsonInputArchive(readTuple);

    ASSERT_EQ(complexTuple, readTuple);
}

TEST_F(TupleTest, throwExceptionForInvalidTupleLength)
{
    muesli::StringIStream stream(serializedSimpleTuple);
    muesli::JsonInputArchive<muesli::StringIStream> jsonInputArchive(stream);

    std::tuple<std::int32_t, std::string, std::int32_t> readTuple;

    EXPECT_THROW(jsonInputArchive(readTuple), muesli::exceptions::ParseException);
}
