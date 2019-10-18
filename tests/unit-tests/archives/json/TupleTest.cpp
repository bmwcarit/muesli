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

#include <cstdint>
#include <tuple>

#include <gtest/gtest.h>

#include "muesli/streams/StringIStream.h"
#include "muesli/streams/StringOStream.h"

#include "muesli/archives/json/JsonInputArchive.h"
#include "muesli/archives/json/JsonOutputArchive.h"

#include "muesli/exceptions/ParseException.h"

#include "testtypes/TEnum.h"
#include "testtypes/TStruct.h"

using namespace ::testing;

class TupleTest : public testing::Test
{
public:
    TupleTest()
            : _testStruct(0.123456789, 64, "test string data"),
              _testEnumeration(muesli::tests::testtypes::TEnum::Enum::TLITERALA),
              _simpleTuple(1, "test"),
              _complexTuple(123, _testEnumeration, _testStruct, "teststring"),
              _serializedSimpleTuple("[1,\"test\"]"),
              _serializedComplexTuple(
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
    muesli::tests::testtypes::TStruct _testStruct;
    muesli::tests::testtypes::TEnum::Enum _testEnumeration;

    std::tuple<std::int32_t, std::string> _simpleTuple;
    std::tuple<std::int32_t,
               muesli::tests::testtypes::TEnum::Enum,
               muesli::tests::testtypes::TStruct,
               std::string> _complexTuple;

    std::string _serializedSimpleTuple;
    std::string _serializedComplexTuple;
};

TEST_F(TupleTest, writeSimpleTuple)
{
    muesli::StringOStream stream;
    muesli::JsonOutputArchive<muesli::StringOStream> jsonOutputArchive(stream);

    jsonOutputArchive(_simpleTuple);

    ASSERT_EQ(_serializedSimpleTuple, stream.getString());
}

TEST_F(TupleTest, writeComplexTuple)
{
    muesli::StringOStream stream;
    muesli::JsonOutputArchive<muesli::StringOStream> jsonOutputArchive(stream);

    jsonOutputArchive(_complexTuple);

    ASSERT_EQ(_serializedComplexTuple, stream.getString());
}

TEST_F(TupleTest, readSimpleTuple)
{
    muesli::StringIStream stream(_serializedSimpleTuple);
    muesli::JsonInputArchive<muesli::StringIStream> jsonInputArchive(stream);

    std::tuple<std::int32_t, std::string> readTuple;

    jsonInputArchive(readTuple);

    ASSERT_EQ(_simpleTuple, readTuple);
}

TEST_F(TupleTest, readComplexTuple)
{
    muesli::StringIStream stream(_serializedComplexTuple);
    muesli::JsonInputArchive<muesli::StringIStream> jsonInputArchive(stream);

    std::tuple<std::int32_t,
               muesli::tests::testtypes::TEnum::Enum,
               muesli::tests::testtypes::TStruct,
               std::string> readTuple;

    jsonInputArchive(readTuple);

    ASSERT_EQ(_complexTuple, readTuple);
}

TEST_F(TupleTest, throwExceptionForInvalidTupleLength)
{
    muesli::StringIStream stream(_serializedSimpleTuple);
    muesli::JsonInputArchive<muesli::StringIStream> jsonInputArchive(stream);

    std::tuple<std::int32_t, std::string, std::int32_t> readTuple;

    EXPECT_THROW(jsonInputArchive(readTuple), muesli::exceptions::ParseException);
}
