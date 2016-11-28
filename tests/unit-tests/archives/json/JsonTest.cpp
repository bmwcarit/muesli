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
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include <utility>

#include <gtest/gtest.h>

#include "muesli/archives/json/JsonInputArchive.h"
#include "muesli/archives/json/JsonOutputArchive.h"

#include "muesli/streams/StdIStreamWrapper.h"
#include "muesli/streams/StdOStreamWrapper.h"

#include "testtypes/TEnum.h"

using OutputStreamImpl = muesli::StdOStreamWrapper<std::ostream>;
using InputStreamImpl = muesli::StdIStreamWrapper<std::istream>;

using JsonOutputArchiveImpl = muesli::JsonOutputArchive<OutputStreamImpl>;
using JsonInputArchiveImpl = muesli::JsonInputArchive<InputStreamImpl>;

using TEnum = muesli::tests::testtypes::TEnum;

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
                                        float,
                                        bool,
                                        std::string,
                                        std::vector<std::int32_t>,
                                        std::vector<std::string>,
                                        std::vector<TEnum::Enum>,
                                        std::vector<bool>>;

TYPED_TEST_CASE(JsonTest, PrimitiveTypes);

TYPED_TEST(JsonTest, serialize)
{
    TestParams<TypeParam> testParams;
    for (std::pair<TypeParam, TypeParam> param : testParams.params) {
        TypeParam value;
        value = param.first;

        std::stringstream stream;
        OutputStreamImpl outputStreamWrapper(stream);
        JsonOutputArchiveImpl jsonOutputArchive(outputStreamWrapper);
        jsonOutputArchive(value);
        std::cout << "JSON for value : " << stream.str() << std::endl;

        InputStreamImpl inputStreamWrapper(stream);
        JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
        TypeParam deserializedValue;
        jsonInputArchive(deserializedValue);
        compareValues(param.second, deserializedValue);
    }
}

template <>
struct TestParams<double>
{
    std::vector<std::pair<double, double>> params = {
            {std::numeric_limits<double>::min(), std::numeric_limits<double>::min()},
            {std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest()},
            {0, 0},
            {0.012345678901234567890123456789, 0.012345678901234567890123456789},
            {1.25e-9, 1.25e-9},
            {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()}};
};

template <>
struct TestParams<float>
{
    std::vector<std::pair<float, float>> params = {
            {std::numeric_limits<float>::min(), std::numeric_limits<float>::min()},
            {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()},
            {0.0f, 0.0f},
            {0.012345678901234567890123456789f, 0.012345678901234567890123456789f},
            {1.25e-9f, 1.25e-9f},
            {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()}};
};

template <>
struct TestParams<std::string>
{
    std::vector<std::pair<std::string, std::string>> params = {
            {std::string(), std::string()},
            {std::string(""), std::string("")},
            {std::string("Hello World!"), std::string("Hello World!")}};
};

template <>
struct TestParams<std::vector<std::int32_t>>
{
    std::vector<std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>>> params = {
            {std::vector<std::int32_t>(), std::vector<std::int32_t>()},
            {{0}, {0}},
            {{0, 1, 2, 3, 4, 5}, {0, 1, 2, 3, 4, 5}}};
};

template <>
struct TestParams<std::vector<std::string>>
{
    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> params = {
            {std::vector<std::string>(), std::vector<std::string>()},
            {{"Test String 1"}, {"Test String 1"}},
            {{"", "Test String 1", "Test String 2"}, {"", "Test String 1", "Test String 2"}}};
};

template <>
struct TestParams<std::vector<TEnum::Enum>>
{
    std::vector<std::pair<std::vector<TEnum::Enum>, std::vector<TEnum::Enum>>> params = {
            {std::vector<TEnum::Enum>(), std::vector<TEnum::Enum>()},
            {{TEnum::TLITERALA}, {TEnum::TLITERALA}},
            {{TEnum::TLITERALA, TEnum::TLITERALB}, {TEnum::TLITERALA, TEnum::TLITERALB}}};
};

template <>
struct TestParams<bool>
{
    std::vector<std::pair<bool, bool>> params = {{true, true}, {false, false}};
};

template <>
struct TestParams<std::vector<bool>>
{
    std::vector<std::pair<std::vector<bool>, std::vector<bool>>> params = {
            {{true, true}, {true, true}},
            {{false, true}, {false, true}},
            {{true, false}, {true, false}},
            {{false, false}, {false, false}}};
};

TEST(JsonTest, deserializeDoubleFromInteger)
{
    using StringIntegerPair = std::pair<std::string, int>;
    std::vector<StringIntegerPair> inputValues = {{"0", 0}, {"100", 100}, {"-999", -999}};

    for (const StringIntegerPair& input : inputValues) {
        std::stringstream stream(input.first);
        InputStreamImpl inputStreamWrapper(stream);
        JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
        double deserializedDouble = -1;
        jsonInputArchive(deserializedDouble);
        EXPECT_FLOAT_EQ(input.second, deserializedDouble);
    }
}
