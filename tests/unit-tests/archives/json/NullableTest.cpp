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
#include <memory>
#include <sstream>
#include <vector>

#include <boost/optional.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "muesli/exceptions/ParseException.h"
#include "muesli/exceptions/ValueNotFoundException.h"

#include "muesli/archives/json/JsonInputArchive.h"
#include "muesli/archives/json/JsonOutputArchive.h"

#include "muesli/streams/StdIStreamWrapper.h"
#include "muesli/streams/StdOStreamWrapper.h"

#include "testtypes/NestedStructs.h"
#include "testtypes/TStruct.h"

#include "muesli/TypeRegistry.h"

using OutputStreamImpl = muesli::StdOStreamWrapper<std::ostream>;
using InputStreamImpl = muesli::StdIStreamWrapper<std::istream>;

using JsonOutputArchiveImpl = muesli::JsonOutputArchive<OutputStreamImpl>;
using JsonInputArchiveImpl = muesli::JsonInputArchive<InputStreamImpl>;

using NestedBoostOptionalStruct = muesli::tests::testtypes::NestedBoostOptionalStruct;
using NestedSharedPtrStruct = muesli::tests::testtypes::NestedSharedPtrStruct;
using NestedUniquePtrStruct = muesli::tests::testtypes::NestedUniquePtrStruct;

template <typename T>
class NullablePtrHandling : public ::testing::Test
{
public:
    NullablePtrHandling()
            : Test(),
              expectedDeserializedStruct(),
              stream(),
              outputStreamWrapper(),
              jsonOutputArchive(),
              inputStreamWrapper(),
              jsonInputArchive(),
              expectedSerializedStruct(),
              expectedDeserialization()
    {
    }

    void InitStreams()
    {
        outputStreamWrapper = std::make_shared<OutputStreamImpl>(stream);
        jsonOutputArchive = std::make_shared<JsonOutputArchiveImpl>(*outputStreamWrapper);
        inputStreamWrapper = std::make_shared<InputStreamImpl>(stream);
        jsonInputArchive = std::make_shared<JsonInputArchiveImpl>(*inputStreamWrapper);
    }

protected:
    T expectedDeserializedStruct;
    std::stringstream stream;
    std::shared_ptr<OutputStreamImpl> outputStreamWrapper;
    std::shared_ptr<JsonOutputArchiveImpl> jsonOutputArchive;
    std::shared_ptr<InputStreamImpl> inputStreamWrapper;
    std::shared_ptr<JsonInputArchiveImpl> jsonInputArchive;
    std::string expectedSerializedStruct;
    std::string expectedDeserialization;
};

typedef ::testing::Types<NestedUniquePtrStruct, NestedSharedPtrStruct, NestedBoostOptionalStruct>
        NullableTypesToTest;
TYPED_TEST_CASE(NullablePtrHandling, NullableTypesToTest);

TYPED_TEST(NullablePtrHandling, AllowNullableToBeMissing)
{
    const std::string canBeDeserializedtStruct(
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.NestedPtrStruct",)"
            R"("tStruct":{)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(})"
            R"(})");
    this->stream << canBeDeserializedtStruct;
    this->InitStreams();
    TypeParam temp;
    this->jsonInputArchive->operator()(temp);
    EXPECT_EQ(temp, this->expectedDeserializedStruct);
}

TYPED_TEST(NullablePtrHandling, AllowNullableToBeNull)
{
    const std::string canBeDeserializedtStruct(
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.NestedPtrStruct",)"
            R"("tStruct":{)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(},)"
            R"("tStructOptional":null)"
            R"(})");
    this->stream << canBeDeserializedtStruct;
    this->InitStreams();
    TypeParam temp;
    this->jsonInputArchive->operator()(temp);
    EXPECT_EQ(temp, this->expectedDeserializedStruct);
}

TYPED_TEST(NullablePtrHandling, AllowNullableToHaveAValue)
{
    const std::string canBeDeserializedtStruct(
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.NestedPtrStruct",)"
            R"("tStruct":{)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(},)"
            R"("tStructOptional":{)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(})"
            R"(})");
    this->stream << canBeDeserializedtStruct;
    this->InitStreams();
    TypeParam temp;
    this->jsonInputArchive->operator()(temp);
    this->expectedDeserializedStruct.copyToOptional();
    EXPECT_EQ(temp, this->expectedDeserializedStruct);
}

TEST(NullableHandling, ExpectThrowOnNonNullableObjectIfMissing)
{
    const std::string canBeDeserializedtStruct(
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.NestedPtrStruct",)"
            R"("tStructOptional":null)"
            R"(})");
    std::stringstream ss;
    ss << canBeDeserializedtStruct;
    InputStreamImpl inputStreamWrapper(ss);
    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    NestedUniquePtrStruct temp;
    EXPECT_THROW(jsonInputArchive(temp), muesli::exceptions::ValueNotFoundException);
}

TEST(NullableHandling, serializeBoostOptional)
{
    const std::string expectedSerializedtStruct(
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.NestedBoostOptionalStruct",)"
            R"("tStruct":{)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(})"
            R"(})");
    const NestedBoostOptionalStruct tNestedBoostOptionalStruct;
    std::stringstream ss;
    OutputStreamImpl outputStreamWrapper(ss);
    JsonOutputArchiveImpl jsonOutputArchive(outputStreamWrapper);
    jsonOutputArchive(tNestedBoostOptionalStruct);

    EXPECT_EQ(expectedSerializedtStruct, ss.str());
}
