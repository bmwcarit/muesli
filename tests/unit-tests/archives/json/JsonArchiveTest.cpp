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
              _stream(),
              _outputStreamWrapper(_stream),
              _jsonOutputArchive(_outputStreamWrapper),
              _inputStreamWrapper(_stream),
              _tStruct(0.123456789, 64, "test string data"),
              _tStructExtended(0.123456789,
                               64,
                               "test string data",
                               muesli::tests::testtypes::TEnum::TLITERALB,
                               32)
    {
        _expectedSerializedStruct =
                R"({)"
                R"("_typeName":"muesli.tests.testtypes.TStruct",)"
                R"("tDouble":0.123456789,)"
                R"("tInt64":64,)"
                R"("tString":"test string data")"
                R"(})";

        _expectedSerializedStructExtended =
                R"({)"
                R"("_typeName":"muesli.tests.testtypes.TStructExtended",)"
                R"("tDouble":0.123456789,)"
                R"("tInt64":64,)"
                R"("tString":"test string data",)"
                R"("tEnum":"TLITERALB",)"
                R"("tInt32":32)"
                R"(})";
        _expectedSerializedNestedStruct =
                R"({)"
                R"("_typeName":"muesli.tests.testtypes.NestedStruct",)"
                R"("_tStruct":)" +
                _expectedSerializedStruct +
                R"(})";
        _expectedSerializedNestedExtendedStruct =
                R"({)"
                R"("_typeName":"muesli.tests.testtypes.NestedStruct",)"
                R"("_tStruct":)" +
                _expectedSerializedStructExtended +
                R"(})";
        _expectedSerializedNestedStructWithNullptr =
                R"({)"
                R"("_typeName":"muesli.tests.testtypes.NestedStruct")"
                R"(})";

        _tStructVector = {
                muesli::tests::testtypes::TStruct(0.123456789, 64, "test string data"),
                muesli::tests::testtypes::TStruct(0.987654321, -64, "second test string")};

        _expectedSerializedStructVector =
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
    std::stringstream _stream;
    OutputStreamImpl _outputStreamWrapper;
    JsonOutputArchiveImpl _jsonOutputArchive;
    InputStreamImpl _inputStreamWrapper;
    muesli::tests::testtypes::TStruct _tStruct;
    muesli::tests::testtypes::TStructExtended _tStructExtended;
    std::string _expectedSerializedStruct;
    std::string _expectedSerializedStructExtended;
    std::string _expectedSerializedNestedStruct;
    std::string _expectedSerializedNestedExtendedStruct;
    std::string _expectedSerializedNestedStructWithNullptr;
    std::vector<muesli::tests::testtypes::TStruct> _tStructVector;
    std::string _expectedSerializedStructVector;
};

TEST_F(JsonArchiveTest, serializeStruct)
{
    _jsonOutputArchive(_tStruct);
    EXPECT_EQ(_expectedSerializedStruct, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
    muesli::tests::testtypes::TStruct tStructDeserialized;
    serialize(jsonInputArchive, tStructDeserialized);
    EXPECT_EQ(_tStruct, tStructDeserialized);
}

TEST_F(JsonArchiveTest, serializeStructExtended)
{
    _jsonOutputArchive(_tStructExtended);
    EXPECT_EQ(_expectedSerializedStructExtended, _stream.str());

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
    muesli::tests::testtypes::TStructExtended tStructExtendedDeserialized;
    jsonInputArchive(tStructExtendedDeserialized);
    EXPECT_EQ(_tStructExtended, tStructExtendedDeserialized);
}

TEST_F(JsonArchiveTest, serializeNestedStruct)
{
    NestedStruct tNestedStruct = {_tStruct};
    _jsonOutputArchive(tNestedStruct);
    EXPECT_EQ(_expectedSerializedNestedStruct, _stream.str());

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
    NestedStruct tStructDeserialized;
    jsonInputArchive(tStructDeserialized);
    std::cout << _stream.str() << std::endl;
    EXPECT_EQ(tNestedStruct, tStructDeserialized);
}

TEST_F(JsonArchiveTest, serializeVectorOfStruct)
{
    _jsonOutputArchive(_tStructVector);
    EXPECT_EQ(_expectedSerializedStructVector, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
    std::vector<muesli::tests::testtypes::TStruct> tStructsDeserialized;
    jsonInputArchive(tStructsDeserialized);
    EXPECT_EQ(_tStructVector, tStructsDeserialized);
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

    _jsonOutputArchive(tEnums);
    EXPECT_EQ(expectedSerializedEnums, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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

    _jsonOutputArchive(orderedStringSet);
    EXPECT_EQ(expectedSerializedStringSet, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
    std::set<std::string> setOfStringDeserialized;
    jsonInputArchive(setOfStringDeserialized);
    EXPECT_EQ(orderedStringSet, setOfStringDeserialized);
}

TEST_F(JsonArchiveTest, serializeUnorderedSetOfStrings)
{
    const std::unordered_set<std::string> unorderedStringSet = {"string_a", "string_b"};

    _jsonOutputArchive(unorderedStringSet);
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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

    _jsonOutputArchive(expectedVector);
    EXPECT_EQ(expectedSerializedVector, _stream.str());

    EXPECT_NE(expectedVector, vectorDeserialized);
    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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

    _jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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

    _jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, _stream.str());

    EXPECT_NE(expectedMap, mapDeserialized);
    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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

    _jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, _stream.str());

    EXPECT_NE(expectedMap, mapDeserialized);
    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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

    _jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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

    _jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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

    _jsonOutputArchive(expectedMap);
    EXPECT_EQ(expectedSerializedMap, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
    TEnumValueMap mapDeserialized;
    jsonInputArchive(mapDeserialized);
    EXPECT_EQ(expectedMap, mapDeserialized);
}

TEST_F(JsonArchiveTest, polymorphismBase)
{
    NestedStructPolymorphic tNestedStructPolymorphic = {
            std::make_shared<muesli::tests::testtypes::TStruct>(_tStruct)};
    _jsonOutputArchive(tNestedStructPolymorphic);
    EXPECT_EQ(_expectedSerializedNestedStruct, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
    NestedStructPolymorphic tNestedStructPolymorphicDeserialized;
    jsonInputArchive(tNestedStructPolymorphicDeserialized);
    EXPECT_EQ(tNestedStructPolymorphic, tNestedStructPolymorphicDeserialized);
}

TEST_F(JsonArchiveTest, polymorphismDerived)
{
    NestedStructPolymorphic tNestedStructPolymorphic = {
            std::make_shared<muesli::tests::testtypes::TStructExtended>(_tStructExtended)};
    _jsonOutputArchive(tNestedStructPolymorphic);
    EXPECT_EQ(_expectedSerializedNestedExtendedStruct, _stream.str());

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
    NestedStructPolymorphic tNestedStructPolymorphicDeserialized;
    jsonInputArchive(tNestedStructPolymorphicDeserialized);
    EXPECT_EQ(tNestedStructPolymorphic, tNestedStructPolymorphicDeserialized);
}

TEST_F(JsonArchiveTest, nullptrSerializationPolymorphism)
{
    NestedStructPolymorphic tNestedStructPolymorphicWithNullptr = {nullptr};
    _jsonOutputArchive(tNestedStructPolymorphicWithNullptr);
    EXPECT_EQ(_expectedSerializedNestedStructWithNullptr, _stream.str());

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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
    expectedStateHistoryStruct.tstruct = _tStruct;
    expectedStateHistoryStruct.stringAfterStruct = "test string data after struct";

    std::string expectedSerializedStateHistoryStruct(
            R"({)"
            R"("_typeName":"StateHistoryTestStruct",)"
            R"("stringBeforStruct":"test string data before struct",)"
            R"("tstruct":)" +
            _expectedSerializedStruct +
            R"(,)"
            R"("stringAfterStruct":"test string data after struct")"
            R"(})");

    _jsonOutputArchive(expectedStateHistoryStruct);
    EXPECT_EQ(expectedSerializedStateHistoryStruct, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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
    _stream.str("}invalid-json{");
    EXPECT_THROW(JsonInputArchiveImpl{_inputStreamWrapper}, muesli::exceptions::ParseException);
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
    _stream.str(expectedSerializedStruct);
    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
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

    std::copy(_tStructVector.begin(),
              _tStructVector.end(),
              std::inserter(multiIndexContainer, multiIndexContainer.begin()));

    _jsonOutputArchive(multiIndexContainer);
    EXPECT_EQ(_expectedSerializedStructVector, _stream.str());
    std::cout << _stream.str() << std::endl;

    JsonInputArchiveImpl jsonInputArchive(_inputStreamWrapper);
    TStructMultiIndexContainer multiIndexContainerDeserialized;
    jsonInputArchive(multiIndexContainerDeserialized);
    EXPECT_EQ(multiIndexContainer, multiIndexContainerDeserialized);
}
