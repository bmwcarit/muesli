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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "testtypes/TStruct.h"
#include "testtypes/TStructExtended.h"
#include "testtypes/TEnum.h"

#include <muesli/BaseArchive.h>
#include <muesli/archives/json/JsonInputArchive.h>
#include <muesli/archives/json/JsonOutputArchive.h>

TEST(JsonArchiveTest, serializeStruct)
{
    muesli::tests::testtypes::TStruct tStruct(0.123456789, 64, "test string data");
    std::string expectedSerializedStruct(
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(})");

    std::stringstream stream;
    muesli::JsonOutputArchive jsonOutputArchive(stream);
    jsonOutputArchive(tStruct);
    EXPECT_EQ(expectedSerializedStruct, stream.str());

    muesli::JsonInputArchive jsonInputArchive(stream);
    muesli::tests::testtypes::TStruct tStructDeserialized;
    jsonInputArchive(tStructDeserialized);
    EXPECT_EQ(tStruct, tStructDeserialized);
}

TEST(JsonArchiveTest, serializeStructExtended)
{
    muesli::tests::testtypes::TStructExtended tStructExtended(
            0.123456789, 64, "test string data", muesli::tests::testtypes::TEnum::TLITERALB, 32);
    std::string expectedSerializedStructExtended(
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.TStructExtended",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data",)"
            R"("tEnum":"TLITERALB",)"
            R"("tInt32":32)"
            R"(})");

    std::stringstream stream;
    muesli::JsonOutputArchive jsonOutputArchive(stream);
    jsonOutputArchive(tStructExtended);
    EXPECT_EQ(expectedSerializedStructExtended, stream.str());

    muesli::JsonInputArchive jsonInputArchive(stream);
    muesli::tests::testtypes::TStructExtended tStructExtendedDeserialized;
    jsonInputArchive(tStructExtendedDeserialized);
    EXPECT_EQ(tStructExtended, tStructExtendedDeserialized);
}

struct TestStruct
{
    muesli::tests::testtypes::TStruct tstruct;

    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(muesli::make_nvp("tstruct", tstruct));
    }

    bool operator==(const TestStruct& other) const
    {
        return tstruct == other.tstruct;
    }
};

MUESLI_REGISTER_TYPE(TestStruct, "TestStruct")

TEST(JsonArchiveTest, serializeNestedStruct)
{
    muesli::tests::testtypes::TStruct tStruct(0.123456789, 64, "test string data");
    TestStruct testStruct;
    testStruct.tstruct = tStruct;
    std::string expectedSerializedStruct(
            R"({)"
            R"("_typeName":"TestStruct",)"
            R"("tstruct":)"
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(})"
            R"(})");

    std::stringstream stream;
    muesli::JsonOutputArchive jsonOutputArchive(stream);
    jsonOutputArchive(testStruct);
    EXPECT_EQ(expectedSerializedStruct, stream.str());

    muesli::JsonInputArchive jsonInputArchive(stream);
    TestStruct tStructDeserialized;
    jsonInputArchive(tStructDeserialized);
    std::cout << stream.str() << std::endl;
    EXPECT_EQ(testStruct, tStructDeserialized);
}
