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
#include <gmock/gmock.h>

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

template <typename T, typename Enable = void>
struct TestParams
{
    static std::vector<std::pair<T, T>> getParams()
    {
        return {{std::numeric_limits<T>::min() - 1, std::numeric_limits<T>::max()},
                {std::numeric_limits<T>::min(), std::numeric_limits<T>::min()},
                {0, 0},
                {std::numeric_limits<T>::max(), std::numeric_limits<T>::max()},
                {std::numeric_limits<T>::max() + 1, std::numeric_limits<T>::min()},
                {std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN()}};
    }
};

template <typename T>
struct TestParams<T, std::enable_if_t<std::is_floating_point<T>::value>>
{
    static std::vector<std::pair<T, T>> getParams()
    {
        return {{std::numeric_limits<T>::min(), std::numeric_limits<T>::min()},
                {std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest()},
                {T(0.0), T(0.0)},
                {T(0.012345678901234567890123456789), T(0.012345678901234567890123456789)},
                {T(1.25e-9), T(1.25e-9)},
                {std::numeric_limits<T>::max(), std::numeric_limits<T>::max()},
                {std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN()}};
    }
};

template <>
struct TestParams<std::string>
{
    static std::vector<std::pair<std::string, std::string>> getParams()
    {
        return {{std::string(), std::string()},
                {std::string("Hello World!"), std::string("Hello World!")}};
    }
};

template <>
struct TestParams<bool>
{
    static std::vector<std::pair<bool, bool>> getParams()
    {
        return {{true, true}, {false, false}};
    }
};

template <>
struct TestParams<TEnum::Enum>
{
    static std::vector<std::pair<TEnum::Enum, TEnum::Enum>> getParams()
    {
        return {{TEnum::TLITERALA, TEnum::TLITERALA}, {TEnum::TLITERALB, TEnum::TLITERALB}};
    }
};

template <typename T>
void compareValues(const T& expected, const T& actual)
{
    EXPECT_EQ(expected, actual);
}

void compareValues(const double& expected, const double& actual)
{
    EXPECT_THAT(expected, ::testing::NanSensitiveDoubleEq(actual));
}

void compareValues(const float& expected, const float& actual)
{
    EXPECT_THAT(expected, ::testing::NanSensitiveFloatEq(actual));
}

template <typename T>
void compareValues(const std::vector<T>& expected, const std::vector<T>& actual)
{
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
        compareValues(expected[i], actual[i]);
    }
}

template <typename T>
using JsonTest = ::testing::Test;

using PrimitiveTypes = ::testing::Types<std::uint8_t,
                                        std::int8_t,
                                        std::uint32_t,
                                        std::int32_t,
                                        std::uint64_t,
                                        std::int64_t,
                                        double,
                                        float,
                                        bool,
                                        std::string,
                                        TEnum::Enum>;

TYPED_TEST_CASE(JsonTest, PrimitiveTypes);

template <typename T>
void serializeDeserialize(const T& input, const T& expected)
{
    std::stringstream stream;
    OutputStreamImpl outputStreamWrapper(stream);
    JsonOutputArchiveImpl jsonOutputArchive(outputStreamWrapper);
    jsonOutputArchive(input);
    std::cout << "JSON for value : " << stream.str() << std::endl;

    InputStreamImpl inputStreamWrapper(stream);
    JsonInputArchiveImpl jsonInputArchive(inputStreamWrapper);
    T deserializedValue;
    jsonInputArchive(deserializedValue);
    compareValues(expected, deserializedValue);
}

TYPED_TEST(JsonTest, serializeScalars)
{
    TestParams<TypeParam> testParams;
    for (const auto& param : testParams.getParams()) {
        serializeDeserialize(param.first, param.second);
    }
}

TYPED_TEST(JsonTest, serializeVectors)
{
    TestParams<TypeParam> testParams;
    std::vector<TypeParam> inputVectorParam;
    std::vector<TypeParam> expectedVectorParam;
    for (const auto& param : testParams.getParams()) {
        inputVectorParam.push_back(param.first);
        expectedVectorParam.push_back(param.second);
    }
    serializeDeserialize(inputVectorParam, expectedVectorParam);
}

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
