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

#include <iostream>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "muesli/archives/json/JsonInputArchive.h"
#include "muesli/archives/json/JsonOutputArchive.h"

#include "muesli/streams/StdIStreamWrapper.h"
#include "muesli/streams/StdOStreamWrapper.h"

#include "testtypes/TStruct.h"
#include "testtypes/TStructExtended.h"
#include "testtypes/TEnum.h"

#include "muesli/TypeRegistry.h"

using OutputStreamImpl = muesli::StdOStreamWrapper<std::ostream>;
using InputStreamImpl = muesli::StdIStreamWrapper<std::istream>;

using JsonOutputArchiveImpl = muesli::JsonOutputArchive<OutputStreamImpl>;
using JsonInputArchiveImpl = muesli::JsonInputArchive<InputStreamImpl>;

struct NestedStruct
{
    muesli::tests::testtypes::TStruct tStruct;

    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(muesli::make_nvp("tStruct", tStruct));
    }

    bool operator==(const NestedStruct& other) const
    {
        return tStruct == other.tStruct;
    }
};
MUESLI_REGISTER_TYPE(NestedStruct, "NestedStruct");

struct NestedStructPolymorphic
{
    std::shared_ptr<muesli::tests::testtypes::TStruct> tStruct;

    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(muesli::make_nvp("tStruct", tStruct));
    }

    bool operator==(const NestedStructPolymorphic& other) const
    {
        // value comparison
        return *tStruct == *(other.tStruct);
    }
};
// register with same name as above in order to use the same string comparison of expected JSON
MUESLI_REGISTER_TYPE(NestedStructPolymorphic, "NestedStruct");

class JsonArchiveTest : public ::testing::Test
{
public:
    JsonArchiveTest()
            : Test(),
              stream(),
              outputStreamWrapper(stream),
              jsonOutputArchive(outputStreamWrapper),
              inputStreamWrapper(stream),
              tStruct(0.123456789, 64, "test string data"),
              tStructExtended(0.123456789,
                              64,
                              "test string data",
                              muesli::tests::testtypes::TEnum::TLITERALB,
                              32)
    {
        expectedSerializedStruct =
                R"({)"
                R"("_typeName":"muesli.tests.testtypes.TStruct",)"
                R"("tDouble":0.123456789,)"
                R"("tInt64":64,)"
                R"("tString":"test string data")"
                R"(})";

        expectedSerializedStructExtended =
                R"({)"
                R"("_typeName":"muesli.tests.testtypes.TStructExtended",)"
                R"("tDouble":0.123456789,)"
                R"("tInt64":64,)"
                R"("tString":"test string data",)"
                R"("tEnum":"TLITERALB",)"
                R"("tInt32":32)"
                R"(})";
        expectedSerializedNestedStruct =
                R"({)"
                R"("_typeName":"NestedStruct",)"
                R"("tStruct":)" +
                expectedSerializedStruct +
                R"(})";
        expectedSerializedNestedExtendedStruct =
                R"({)"
                R"("_typeName":"NestedStruct",)"
                R"("tStruct":)" +
                expectedSerializedStructExtended +
                R"(})";
    }

protected:
    std::stringstream stream;
    OutputStreamImpl outputStreamWrapper;
    JsonOutputArchiveImpl jsonOutputArchive;
    InputStreamImpl inputStreamWrapper;
    muesli::tests::testtypes::TStruct tStruct;
    muesli::tests::testtypes::TStructExtended tStructExtended;
    std::string expectedSerializedStruct;
    std::string expectedSerializedStructExtended;
    std::string expectedSerializedNestedStruct;
    std::string expectedSerializedNestedExtendedStruct;
};

TEST_F(JsonArchiveTest, serializeStruct)
{
    jsonOutputArchive(tStruct);
    EXPECT_EQ(expectedSerializedStruct, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    muesli::tests::testtypes::TStruct tStructDeserialized;
    serialize(jsonInputArchive, tStructDeserialized);
    EXPECT_EQ(tStruct, tStructDeserialized);
}

TEST_F(JsonArchiveTest, serializeStructExtended)
{
    jsonOutputArchive(tStructExtended);
    EXPECT_EQ(expectedSerializedStructExtended, stream.str());

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    muesli::tests::testtypes::TStructExtended tStructExtendedDeserialized;
    jsonInputArchive(tStructExtendedDeserialized);
    EXPECT_EQ(tStructExtended, tStructExtendedDeserialized);
}

TEST_F(JsonArchiveTest, serializeNestedStruct)
{
    NestedStruct tNestedStruct = {tStruct};
    jsonOutputArchive(tNestedStruct);
    EXPECT_EQ(expectedSerializedNestedStruct, stream.str());

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    NestedStruct tStructDeserialized;
    jsonInputArchive(tStructDeserialized);
    std::cout << stream.str() << std::endl;
    EXPECT_EQ(tNestedStruct, tStructDeserialized);
}

TEST_F(JsonArchiveTest, serializeVectorOfStruct)
{
    std::vector<muesli::tests::testtypes::TStruct> tStructs = {
            muesli::tests::testtypes::TStruct(0.123456789, 64, "test string data"),
            muesli::tests::testtypes::TStruct(0.987654321, -64, "second test string")};

    std::string expectedSerializedStructs(
            R"([)"
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(},)"
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.987654321,)"
            R"("tInt64":-64,)"
            R"("tString":"second test string")"
            R"(})"
            R"(])");

    jsonOutputArchive(tStructs);
    EXPECT_EQ(expectedSerializedStructs, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    std::vector<muesli::tests::testtypes::TStruct> tStructsDeserialized;
    jsonInputArchive(tStructsDeserialized);
    EXPECT_EQ(tStructs, tStructsDeserialized);
}

TEST_F(JsonArchiveTest, serializeVectorOfEnums)
{
    std::vector<muesli::tests::testtypes::TEnum::Enum> tEnums = {
            muesli::tests::testtypes::TEnum::TLITERALA, muesli::tests::testtypes::TEnum::TLITERALB};

    std::string expectedSerializedEnums(
            R"([)"
            R"("TLITERALA",)"
            R"("TLITERALB")"
            R"(])");

    jsonOutputArchive(tEnums);
    EXPECT_EQ(expectedSerializedEnums, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    std::vector<muesli::tests::testtypes::TEnum::Enum> tEnumsDeserialized;
    jsonInputArchive(tEnumsDeserialized);
    EXPECT_EQ(tEnums, tEnumsDeserialized);
}

class TIntegerKeyMap : public std::map<std::int32_t, std::string>
{
};

MUESLI_REGISTER_TYPE(TIntegerKeyMap, "TIntegerKeyMap")

class TStructMap : public std::map<std::string, muesli::tests::testtypes::TStruct>
{
};

MUESLI_REGISTER_TYPE(TStructMap, "TStructMap")

class TEnumKeyMap
        : public std::map<muesli::tests::testtypes::TEnum::Enum, muesli::tests::testtypes::TStruct>
{
};

MUESLI_REGISTER_TYPE(TEnumKeyMap, "TEnumKeyMap")

class TEnumValueMap : public std::map<std::string, muesli::tests::testtypes::TEnum::Enum>
{
};

MUESLI_REGISTER_TYPE(TEnumValueMap, "TEnumValueMap")

TEST_F(JsonArchiveTest, serializeIntegerMap)
{
    TIntegerKeyMap expectedMap;
    expectedMap.insert({1, "StringValue1"});
    expectedMap.insert({2, "StringValue2"});

    std::string expectedSerializedMap(
            R"({)"
            R"("_typeName":"TIntegerKeyMap",)"
            R"("1":"StringValue1",)"
            R"("2":"StringValue2")"
            R"(})");

    jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    TIntegerKeyMap mapDeserialized;
    jsonInputArchive(mapDeserialized);
    EXPECT_EQ(expectedMap, mapDeserialized);
}

TEST_F(JsonArchiveTest, serializeStructMap)
{
    TStructMap expectedMap;

    expectedMap.insert(
            {"key1", muesli::tests::testtypes::TStruct(0.123456789, 64, "test string data")});
    expectedMap.insert(
            {"key2", muesli::tests::testtypes::TStruct(0.987654321, -64, "second test string")});

    std::string expectedSerializedMap(
            R"({)"
            R"("_typeName":"TStructMap",)"
            R"("key1":)"
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(},)"
            R"("key2":)"
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.987654321,)"
            R"("tInt64":-64,)"
            R"("tString":"second test string")"
            R"(})"
            R"(})");

    jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    TStructMap mapDeserialized;
    jsonInputArchive(mapDeserialized);
    EXPECT_EQ(expectedMap, mapDeserialized);
}

TEST_F(JsonArchiveTest, serializeEnumKeyMap)
{
    TEnumKeyMap expectedMap;

    expectedMap.insert({muesli::tests::testtypes::TEnum::TLITERALA,
                        muesli::tests::testtypes::TStruct(0.123456789, 64, "test string data")});
    expectedMap.insert({muesli::tests::testtypes::TEnum::TLITERALB,
                        muesli::tests::testtypes::TStruct(0.987654321, -64, "second test string")});

    std::string expectedSerializedMap(
            R"({)"
            R"("_typeName":"TEnumKeyMap",)"
            R"("TLITERALA":)"
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(},)"
            R"("TLITERALB":)"
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.987654321,)"
            R"("tInt64":-64,)"
            R"("tString":"second test string")"
            R"(})"
            R"(})");

    jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    TEnumKeyMap mapDeserialized;
    jsonInputArchive(mapDeserialized);
    EXPECT_EQ(expectedMap, mapDeserialized);
}

TEST_F(JsonArchiveTest, serializeEnumValueMap)
{
    TEnumValueMap expectedMap;

    expectedMap.insert({"key0", muesli::tests::testtypes::TEnum::TLITERALA});
    expectedMap.insert({"key1", muesli::tests::testtypes::TEnum::TLITERALB});

    std::string expectedSerializedMap(
            R"({)"
            R"("_typeName":"TEnumValueMap",)"
            R"("key0":"TLITERALA",)"
            R"("key1":"TLITERALB")"
            R"(})");

    jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    TEnumValueMap mapDeserialized;
    jsonInputArchive(mapDeserialized);
    EXPECT_EQ(expectedMap, mapDeserialized);
}

TEST_F(JsonArchiveTest, polymorphismBase)
{
    NestedStructPolymorphic tNestedStructPolymorphic = {
            std::make_shared<muesli::tests::testtypes::TStruct>(tStruct)};
    jsonOutputArchive(tNestedStructPolymorphic);
    EXPECT_EQ(expectedSerializedNestedStruct, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    NestedStructPolymorphic tNestedStructPolymorphicDeserialized;
    jsonInputArchive(tNestedStructPolymorphicDeserialized);
    EXPECT_EQ(tNestedStructPolymorphic, tNestedStructPolymorphicDeserialized);
}

TEST_F(JsonArchiveTest, polymorphismDerived)
{
    NestedStructPolymorphic tNestedStructPolymorphic = {
            std::make_shared<muesli::tests::testtypes::TStructExtended>(tStructExtended)};
    jsonOutputArchive(tNestedStructPolymorphic);
    EXPECT_EQ(expectedSerializedNestedExtendedStruct, stream.str());

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    NestedStructPolymorphic tNestedStructPolymorphicDeserialized;
    jsonInputArchive(tNestedStructPolymorphicDeserialized);
    EXPECT_EQ(tNestedStructPolymorphic, tNestedStructPolymorphicDeserialized);
}
