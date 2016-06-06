/*
 *
 * Copyright (C) 2011 - 2015 BMW Car IT GmbH
 *
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
 */

#include "TEnum.h"
#include <sstream>

namespace muesli
{
namespace tests
{
namespace testtypes
{

const std::uint32_t TEnum::MAJOR_VERSION = 49;
const std::uint32_t TEnum::MINOR_VERSION = 13;

std::string TEnum::getLiteral(const TEnum::Enum& tEnumValue)
{
    std::string literal;
    switch (tEnumValue) {
    case TLITERALA:
        literal = std::string("TLITERALA");
        break;
    case TLITERALB:
        literal = std::string("TLITERALB");
        break;
    }
    if (literal.empty()) {
        throw std::invalid_argument("TEnum: No literal found for value \"" +
                                    std::to_string(tEnumValue) + "\"");
    }
    return literal;
}

TEnum::Enum TEnum::getEnum(const std::string& tEnumString)
{
    if (tEnumString == std::string("TLITERALA")) {
        return TLITERALA;
    }
    if (tEnumString == std::string("TLITERALB")) {
        return TLITERALB;
    }
    std::stringstream errorMessage(tEnumString);
    errorMessage << " is unknown literal for TEnum";
    throw std::invalid_argument(errorMessage.str());
}

std::string TEnum::getTypeName()
{
    return "joynr.types.TestTypes.TEnum";
}

std::uint32_t TEnum::getOrdinal(TEnum::Enum tEnumValue)
{
    return static_cast<std::uint32_t>(tEnumValue);
}

// Printing TEnum with google-test and google-mock.
void PrintTo(const TEnum::Enum& tEnumValue, ::std::ostream* os)
{
    *os << "TEnum::" << TEnum::getLiteral(tEnumValue) << " (" << TEnum::getOrdinal(tEnumValue)
        << ")";
}

} // namespace testtypes
} // namespace tests
} // namespace muesli
