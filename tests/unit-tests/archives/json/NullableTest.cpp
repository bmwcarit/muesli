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

#include <memory>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "muesli/exceptions/ParseException.h"
#include "muesli/exceptions/ValueNotFoundException.h"

#include "muesli/archives/json/JsonInputArchive.h"
#include "muesli/archives/json/JsonOutputArchive.h"

#include "muesli/streams/StdIStreamWrapper.h"
#include "muesli/streams/StdOStreamWrapper.h"

#include "testtypes/NestedStructs.h"

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
              _expectedDeserializedStruct(),
              _stream(),
              _outputStreamWrapper(),
              _jsonOutputArchive(),
              _inputStreamWrapper(),
              _jsonInputArchive(),
              _expectedSerializedStruct(),
              _expectedDeserialization()
    {
    }

    void InitStreams()
    {
        _outputStreamWrapper = std::make_shared<OutputStreamImpl>(_stream);
        _jsonOutputArchive = std::make_shared<JsonOutputArchiveImpl>(*_outputStreamWrapper);
        _inputStreamWrapper = std::make_shared<InputStreamImpl>(_stream);
        _jsonInputArchive = std::make_shared<JsonInputArchiveImpl>(*_inputStreamWrapper);
    }

protected:
    T _expectedDeserializedStruct;
    std::stringstream _stream;
    std::shared_ptr<OutputStreamImpl> _outputStreamWrapper;
    std::shared_ptr<JsonOutputArchiveImpl> _jsonOutputArchive;
    std::shared_ptr<InputStreamImpl> _inputStreamWrapper;
    std::shared_ptr<JsonInputArchiveImpl> _jsonInputArchive;
    std::string _expectedSerializedStruct;
    std::string _expectedDeserialization;
};

using NullableTypesToTest =
        ::testing::Types<NestedUniquePtrStruct, NestedSharedPtrStruct, NestedBoostOptionalStruct>;

TYPED_TEST_CASE(NullablePtrHandling, NullableTypesToTest);

TYPED_TEST(NullablePtrHandling, AllowNullableToBeMissing)
{
    const std::string canBeDeserializedtStruct(
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.NestedPtrStruct",)"
            R"("_tStruct":{)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(})"
            R"(})");
    this->_stream << canBeDeserializedtStruct;
    this->InitStreams();
    TypeParam temp;
    this->_jsonInputArchive->operator()(temp);
    EXPECT_EQ(temp, this->_expectedDeserializedStruct);
}

TYPED_TEST(NullablePtrHandling, AllowNullableToBeNull)
{
    const std::string canBeDeserializedtStruct(
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.NestedPtrStruct",)"
            R"("_tStruct":{)"
            R"("_typeName":"muesli.tests.testtypes.TStruct",)"
            R"("tDouble":0.123456789,)"
            R"("tInt64":64,)"
            R"("tString":"test string data")"
            R"(},)"
            R"("tStructOptional":null)"
            R"(})");
    this->_stream << canBeDeserializedtStruct;
    this->InitStreams();
    TypeParam temp;
    this->_jsonInputArchive->operator()(temp);
    EXPECT_EQ(temp, this->_expectedDeserializedStruct);
}

TYPED_TEST(NullablePtrHandling, AllowNullableToHaveAValue)
{
    const std::string canBeDeserializedtStruct(
            R"({)"
            R"("_typeName":"muesli.tests.testtypes.NestedPtrStruct",)"
            R"("_tStruct":{)"
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
    this->_stream << canBeDeserializedtStruct;
    this->InitStreams();
    TypeParam temp;
    this->_jsonInputArchive->operator()(temp);
    this->_expectedDeserializedStruct.copyToOptional();
    EXPECT_EQ(temp, this->_expectedDeserializedStruct);
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
            R"("_tStruct":{)"
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
