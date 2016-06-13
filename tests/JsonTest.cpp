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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "testtypes/TStruct.h"
#include "testtypes/TStructExtended.h"
#include "testtypes/TEnum.h"

#include "muesli/BaseArchive.h"
#include "muesli/archives/json/JsonInputArchive.h"
#include "muesli/archives/json/JsonOutputArchive.h"
#include "muesli/archives/json/detail/traits.h"

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

using PrimitiveTypes = ::testing::Types<std::int8_t, std::int32_t, double, std::string>;

TYPED_TEST_CASE(JsonTest, PrimitiveTypes);

TYPED_TEST(JsonTest, serialize)
{
    TestStruct<TypeParam> testStruct, deserializedTestStruct;

    TestParams<TypeParam> testParams;
    for (std::pair<TypeParam, TypeParam> param : testParams.params) {
        testStruct.value = param.first;

        std::stringstream stream;
        muesli::JsonOutputArchive jsonOutputArchive(stream);
        jsonOutputArchive(testStruct);
        std::cout << "JSON for TestStruct : " << stream.str() << std::endl;
        muesli::JsonInputArchive jsonInputArchive(stream);
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
