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

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <set>
#include <unordered_set>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/optional.hpp>

#include <gtest/gtest.h>

#include "muesli/exceptions/ParseException.h"
#include "muesli/exceptions/ValueNotFoundException.h"

#include "muesli/archives/json/JsonInputArchive.h"
#include "muesli/archives/json/JsonOutputArchive.h"

#include "muesli/streams/StdIStreamWrapper.h"
#include "muesli/streams/StdOStreamWrapper.h"

#include "muesli/TypeRegistry.h"

#include "testtypes/NestedStructs.h"
#include "testtypes/TStruct.h"
#include "testtypes/TStructExtended.h"
#include "testtypes/TEnum.h"

using OutputStreamImpl = muesli::StdOStreamWrapper<std::ostream>;
using InputStreamImpl = muesli::StdIStreamWrapper<std::istream>;

using JsonOutputArchiveImpl = muesli::JsonOutputArchive<OutputStreamImpl>;
using JsonInputArchiveImpl = muesli::JsonInputArchive<InputStreamImpl>;

using NestedBoostOptionalStruct = muesli::tests::testtypes::NestedBoostOptionalStruct;
using NestedStruct = muesli::tests::testtypes::NestedStruct;
using NestedStructPolymorphic = muesli::tests::testtypes::NestedStructPolymorphic;

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
                R"("_typeName":"muesli.tests.testtypes.NestedStruct",)"
                R"("tStruct":)" +
                expectedSerializedStruct +
                R"(})";
        expectedSerializedNestedExtendedStruct =
                R"({)"
                R"("_typeName":"muesli.tests.testtypes.NestedStruct",)"
                R"("tStruct":)" +
                expectedSerializedStructExtended +
                R"(})";
        expectedSerializedNestedStructWithNullptr =
                R"({)"
                R"("_typeName":"muesli.tests.testtypes.NestedStruct")"
                R"(})";

        tStructVector = {muesli::tests::testtypes::TStruct(0.123456789, 64, "test string data"),
                         muesli::tests::testtypes::TStruct(0.987654321, -64, "second test string")};

        expectedSerializedStructVector =
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
                R"(])";
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
    std::string expectedSerializedNestedStructWithNullptr;
    std::vector<muesli::tests::testtypes::TStruct> tStructVector;
    std::string expectedSerializedStructVector;
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
    jsonOutputArchive(tStructVector);
    EXPECT_EQ(expectedSerializedStructVector, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    std::vector<muesli::tests::testtypes::TStruct> tStructsDeserialized;
    jsonInputArchive(tStructsDeserialized);
    EXPECT_EQ(tStructVector, tStructsDeserialized);
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

TEST_F(JsonArchiveTest, serializeSetOfStrings)
{
    const std::set<std::string> orderedStringSet = {"string_a", "string_b"};

    std::string expectedSerializedStringSet(
            R"([)"
            R"("string_a",)"
            R"("string_b")"
            R"(])");

    jsonOutputArchive(orderedStringSet);
    EXPECT_EQ(expectedSerializedStringSet, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    std::set<std::string> setOfStringDeserialized;
    jsonInputArchive(setOfStringDeserialized);
    EXPECT_EQ(orderedStringSet, setOfStringDeserialized);
}

TEST_F(JsonArchiveTest, serializeUnorderedSetOfStrings)
{
    const std::unordered_set<std::string> unorderedStringSet = {"string_a", "string_b"};

    jsonOutputArchive(unorderedStringSet);
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    std::unordered_set<std::string> unorderedSetOfStringDeserialized;
    jsonInputArchive(unorderedSetOfStringDeserialized);
    EXPECT_EQ(unorderedStringSet, unorderedSetOfStringDeserialized);
}

TEST_F(JsonArchiveTest, clearExistingVectorEntriesDuringDeserialization)
{
    std::vector<std::string> expectedVector;
    std::vector<std::string> vectorDeserialized;
    vectorDeserialized.push_back("existingEntry");

    std::string expectedSerializedVector(
            R"([)"
            R"(])");

    jsonOutputArchive(expectedVector);
    EXPECT_EQ(expectedSerializedVector, stream.str());

    EXPECT_NE(expectedVector, vectorDeserialized);
    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    jsonInputArchive(vectorDeserialized);
    EXPECT_EQ(expectedVector, vectorDeserialized);
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

class TStringStringMap : public std::map<std::string, std::string>
{
};

MUESLI_REGISTER_TYPE(TStringStringMap, "TStringStringMap")

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

TEST_F(JsonArchiveTest, overrideExistingMapEntriesDuringDeserialization)
{
    TStringStringMap expectedMap;
    expectedMap.insert({"existingKey", "overriddenValue"});
    TStringStringMap mapDeserialized;
    mapDeserialized.insert({"existingKey", "existingValue"});

    std::string expectedSerializedMap(
            R"({)"
            R"("_typeName":"TStringStringMap",)"
            R"("existingKey":"overriddenValue")"
            R"(})");

    jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, stream.str());

    EXPECT_NE(expectedMap, mapDeserialized);
    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    jsonInputArchive(mapDeserialized);
    EXPECT_EQ(expectedMap, mapDeserialized);
}

TEST_F(JsonArchiveTest, clearExistingMapEntriesDuringDeserialization)
{
    TStringStringMap expectedMap;
    TStringStringMap mapDeserialized;
    mapDeserialized.insert({"existingKey", "existingValue"});

    std::string expectedSerializedMap(
            R"({)"
            R"("_typeName":"TStringStringMap")"
            R"(})");

    jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, stream.str());

    EXPECT_NE(expectedMap, mapDeserialized);
    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
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

TEST_F(JsonArchiveTest, nullptrSerializationPolymorphism)
{
    NestedStructPolymorphic tNestedStructPolymorphicWithNullptr = {nullptr};
    jsonOutputArchive(tNestedStructPolymorphicWithNullptr);
    EXPECT_EQ(expectedSerializedNestedStructWithNullptr, stream.str());

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    NestedStructPolymorphic tNestedStructPolymorphicDeserialized;
    jsonInputArchive(tNestedStructPolymorphicDeserialized);
    EXPECT_EQ(tNestedStructPolymorphicWithNullptr, tNestedStructPolymorphicDeserialized);
}

struct StateHistoryTestStruct
{
    std::string stringBeforStruct;
    muesli::tests::testtypes::TStruct tstruct;
    std::string stringAfterStruct;

    template <typename Archive>
    void load(Archive& archive)
    {
        archive(muesli::make_nvp("stringBeforStruct", stringBeforStruct));
        archive.pushState();
        archive(muesli::make_nvp("stringAfterStruct", stringAfterStruct));
    }

    template <typename Archive>
    void save(Archive& archive)
    {
        archive(muesli::make_nvp("stringBeforStruct", stringBeforStruct),
                muesli::make_nvp("tstruct", tstruct),
                muesli::make_nvp("stringAfterStruct", stringAfterStruct));
    }

    bool operator==(const StateHistoryTestStruct& other) const
    {
        return stringBeforStruct == other.stringBeforStruct && tstruct == other.tstruct &&
               stringAfterStruct == other.stringAfterStruct;
    }
};

MUESLI_REGISTER_TYPE(StateHistoryTestStruct, "StateHistoryTestStruct")

TEST_F(JsonArchiveTest, pushPopState)
{
    StateHistoryTestStruct expectedStateHistoryStruct;
    expectedStateHistoryStruct.stringBeforStruct = "test string data before struct";
    expectedStateHistoryStruct.tstruct = tStruct;
    expectedStateHistoryStruct.stringAfterStruct = "test string data after struct";

    std::string expectedSerializedStateHistoryStruct(
            R"({)"
            R"("_typeName":"StateHistoryTestStruct",)"
            R"("stringBeforStruct":"test string data before struct",)"
            R"("tstruct":)" +
            expectedSerializedStruct +
            R"(,)"
            R"("stringAfterStruct":"test string data after struct")"
            R"(})");

    jsonOutputArchive(expectedStateHistoryStruct);
    EXPECT_EQ(expectedSerializedStateHistoryStruct, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    StateHistoryTestStruct stateHistoryStructDeserialized;
    jsonInputArchive(stateHistoryStructDeserialized);
    EXPECT_EQ(expectedStateHistoryStruct.stringBeforStruct,
              stateHistoryStructDeserialized.stringBeforStruct);
    EXPECT_EQ(expectedStateHistoryStruct.stringAfterStruct,
              stateHistoryStructDeserialized.stringAfterStruct);

    jsonInputArchive.popState();
    muesli::tests::testtypes::TStruct tstructDeserialized;
    jsonInputArchive(muesli::make_nvp("tstruct", tstructDeserialized));
    EXPECT_EQ(expectedStateHistoryStruct.tstruct, tstructDeserialized);
}

TEST_F(JsonArchiveTest, invalidJsonCausesParseException)
{
    stream.str("}invalid-json{");
    EXPECT_THROW(JsonInputArchiveImpl{inputStreamWrapper}, muesli::exceptions::ParseException);
}

TEST_F(JsonArchiveTest, deserializeThrowsOnMissingField)
{
    std::string expectedSerializedStruct =
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            // missing tDouble field R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(})";
    stream.str(expectedSerializedStruct);
    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    muesli::tests::testtypes::TStruct tStructDeserialized;
    EXPECT_THROW(serialize(jsonInputArchive, tStructDeserialized),
                 muesli::exceptions::ValueNotFoundException);
}

namespace bmi = boost::multi_index;

using TStructMultiIndexContainer = boost::multi_index_container<
        muesli::tests::testtypes::TStruct,
        bmi::indexed_by<bmi::hashed_unique<
                                BOOST_MULTI_INDEX_CONST_MEM_FUN(muesli::tests::testtypes::TStruct,
                                                                const std::int64_t&,
                                                                getTInt64)>,
                        bmi::hashed_unique<
                                BOOST_MULTI_INDEX_CONST_MEM_FUN(muesli::tests::testtypes::TStruct,
                                                                const std::string&,
                                                                getTString)>>>;

TEST_F(JsonArchiveTest, serializeDeserializeBoostMultiIndexContainer)
{
    TStructMultiIndexContainer multiIndexContainer;

    std::copy(tStructVector.begin(),
              tStructVector.end(),
              std::inserter(multiIndexContainer, multiIndexContainer.begin()));

    jsonOutputArchive(multiIndexContainer);
    EXPECT_EQ(expectedSerializedStructVector, stream.str());
    std::cout << stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    TStructMultiIndexContainer multiIndexContainerDeserialized;
    jsonInputArchive(multiIndexContainerDeserialized);
    EXPECT_EQ(multiIndexContainer, multiIndexContainerDeserialized);
}
