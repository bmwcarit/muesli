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

#ifndef MUESLI_TESTS_TESTTYPES_TENUM_H
#define MUESLI_TESTS_TESTTYPES_TENUM_H

#include <cstdint>
#include <ostream>
#include <string>

#include <muesli/TypeRegistry.h>
#include <muesli/Traits.h>

namespace muesli
{
namespace tests
{
namespace testtypes
{

/**
 * @brief Enumeration wrapper class TEnum
 *
 * @version 49.13
 */
struct TEnum
{
    /**
     * @version 49.13
     */
    enum Enum : std::uint32_t {
        /**
         * @brief
         */
        TLITERALA = 0,
        /**
         * @brief
         */
        TLITERALB = 1
    };

    /**
     * @brief MAJOR_VERSION The major version of this struct as specified in the
     * type collection or interface in the Franca model.
     */
    static const std::uint32_t MAJOR_VERSION = 49;
    /**
     * @brief MINOR_VERSION The minor version of this struct as specified in the
     * type collection or interface in the Franca model.
     */
    static const std::uint32_t MINOR_VERSION = 13;

    /** @brief Constructor */
    TEnum() = delete;

    /**
     * @brief Copy constructor
     * @param o the object to copy from
     */
    TEnum(const TEnum& o) = delete;

    /**
     * @brief Get the matching enum name for an ordinal number
     * @param tEnumValue The ordinal number
     * @return The string representing the enum for the given ordinal number
     */
    static std::string getLiteral(const TEnum::Enum& tEnumValue)
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

    /**
     * @brief Get the matching enum for a string
     * @param tEnumString The string representing the enum value
     * @return The enum value representing the string
     */
    static TEnum::Enum getEnum(const std::string& tEnumString)
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

    /**
     * @brief Get the matching ordinal number for an enum
     * @param tEnumValue The enum
     * @return The ordinal number representing the enum
     */
    static std::uint32_t getOrdinal(TEnum::Enum tEnumValue)
    {
        return static_cast<std::uint32_t>(tEnumValue);
    }

    /**
     * @brief Get the typeName of the enumeration type
     * @return The typeName of the enumeration type
     */
    static std::string getTypeName();
};

// Printing TEnum with google-test and google-mock.
/**
 * @brief Print values of MessagingQos object
 * @param messagingQos The current object instance
 * @param os The output stream to send the output to
 */
inline void PrintTo(const muesli::tests::testtypes::TEnum::Enum& tEnumValue, ::std::ostream* os)
{
    *os << "TEnum::" << TEnum::getLiteral(tEnumValue) << " (" << TEnum::getOrdinal(tEnumValue)
        << ")";
}

} // namespace testtypes
} // namespace tests
} // namespace muesli

MUESLI_REGISTER_TYPE(muesli::tests::testtypes::TEnum::Enum, "muesli.tests.testtypes.TEnum")

namespace muesli
{
template <>
struct EnumTraits<muesli::tests::testtypes::TEnum::Enum>
{
    using Wrapper = muesli::tests::testtypes::TEnum;
};

} // namespace muesli

#endif // MUESLI_TESTS_TESTTYPES_TENUM_H
