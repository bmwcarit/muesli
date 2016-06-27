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
#include <limits>
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

using TEnum = muesli::tests::testtypes::TEnum;

template <typename T>
struct TestStruct
{
    T value;

    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(muesli::make_nvp("value", value));
    }
};

template <typename T>
struct TestParams
{
    std::vector<std::pair<T, T>> params = {
            {std::numeric_limits<T>::min() - 1, std::numeric_limits<T>::max()},
            {std::numeric_limits<T>::min(), std::numeric_limits<T>::min()},
            {0, 0},
            {std::numeric_limits<T>::max(), std::numeric_limits<T>::max()},
            {std::numeric_limits<T>::max() + 1, std::numeric_limits<T>::min()}};
};

template <typename T>
void compareValues(const T& expected, const T& actual)
{
    EXPECT_EQ(expected, actual);
}

void compareValues(const double& expected, const double& actual)
{
    EXPECT_FLOAT_EQ(expected, actual);
}

template <typename T>
using JsonTest = ::testing::Test;

using PrimitiveTypes = ::testing::Types<std::int8_t,
                                        std::int32_t,
                                        double,
                                        std::string,
                                        std::vector<std::int32_t>,
                                        std::vector<std::string>,
                                        std::vector<TEnum::Enum>>;

TYPED_TEST_CASE(JsonTest, PrimitiveTypes);

TYPED_TEST(JsonTest, serialize)
{
    TestParams<TypeParam> testParams;
    for (std::pair<TypeParam, TypeParam> param : testParams.params) {
        TestStruct<TypeParam> testStruct, deserializedTestStruct;
        testStruct.value = param.first;

        std::stringstream stream;
        OutputStreamImpl outputStreamWrapper(stream);
        JsonOutputArchiveImpl jsonOutputArchive(outputStreamWrapper);
        jsonOutputArchive(testStruct);
        std::cout << "JSON for TestStruct : " << stream.str() << std::endl;
        InputStreamImpl inputStreamWrapper(stream);
        JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
        jsonInputArchive(deserializedTestStruct);
        compareValues(param.second, deserializedTestStruct.value);
    }
}

using Int8Struct = TestStruct<std::int8_t>;
MUESLI_REGISTER_TYPE(Int8Struct, "TestStruct")

using Int32Struct = TestStruct<std::int32_t>;
MUESLI_REGISTER_TYPE(Int32Struct, "TestStruct")

template <>
struct TestParams<double>
{
    std::vector<std::pair<double, double>> params = {
            {std::numeric_limits<double>::min(), std::numeric_limits<double>::min()},
            {std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest()},
            {0, 0},
            {0.012345678901234567890123456789, 0.012345678901234567890123456789},
            {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()}};
};
using DoubleStruct = TestStruct<double>;
MUESLI_REGISTER_TYPE(DoubleStruct, "TestStruct")

template <>
struct TestParams<std::string>
{
    std::vector<std::pair<std::string, std::string>> params = {
            {std::string(), std::string()},
            {std::string(""), std::string("")},
            {std::string("Hello World!"), std::string("Hello World!")}};
};
using StringStruct = TestStruct<std::string>;
MUESLI_REGISTER_TYPE(StringStruct, "TestStruct")

template <>
struct TestParams<std::vector<std::int32_t>>
{
    std::vector<std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>>> params = {
            {std::vector<std::int32_t>(), std::vector<std::int32_t>()},
            {{0}, {0}},
            {{0, 1, 2, 3, 4, 5}, {0, 1, 2, 3, 4, 5}}};
};
using ArrayOfInt32Struct = TestStruct<std::vector<std::int32_t>>;
MUESLI_REGISTER_TYPE(ArrayOfInt32Struct, "TestStruct")

template <>
struct TestParams<std::vector<std::string>>
{
    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> params = {
            {std::vector<std::string>(), std::vector<std::string>()},
            {{"Test String 1"}, {"Test String 1"}},
            {{"", "Test String 1", "Test String 2"}, {"", "Test String 1", "Test String 2"}}};
};
using ArrayOfStringStruct = TestStruct<std::vector<std::string>>;
MUESLI_REGISTER_TYPE(ArrayOfStringStruct, "TestStruct")

template <>
struct TestParams<std::vector<TEnum::Enum>>
{
    std::vector<std::pair<std::vector<TEnum::Enum>, std::vector<TEnum::Enum>>> params = {
            {std::vector<TEnum::Enum>(), std::vector<TEnum::Enum>()},
            {{TEnum::TLITERALA}, {TEnum::TLITERALA}},
            {{TEnum::TLITERALA, TEnum::TLITERALB}, {TEnum::TLITERALA, TEnum::TLITERALB}}};
};
using ArrayOfEnumStruct = TestStruct<std::vector<TEnum::Enum>>;
MUESLI_REGISTER_TYPE(ArrayOfEnumStruct, "TestStruct")
