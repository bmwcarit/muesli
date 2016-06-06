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
#include <cstdint>

#include "muesli/TypeRegistry.h"

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
    static const std::uint32_t MAJOR_VERSION;
    /**
     * @brief MINOR_VERSION The minor version of this struct as specified in the
     * type collection or interface in the Franca model.
     */
    static const std::uint32_t MINOR_VERSION;

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
    static std::string getLiteral(const TEnum::Enum& tEnumValue);

    /**
     * @brief Get the matching enum for a string
     * @param tEnumString The string representing the enum value
     * @return The enum value representing the string
     */
    static TEnum::Enum getEnum(const std::string& tEnumString);

    /**
     * @brief Get the matching ordinal number for an enum
     * @param tEnumValue The enum
     * @return The ordinal number representing the enum
     */
    static std::uint32_t getOrdinal(TEnum::Enum tEnumValue);

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
void PrintTo(const muesli::tests::testtypes::TEnum::Enum& tEnumValue, ::std::ostream* os);

} // namespace testtypes
} // namespace tests
} // namespace muesli

MUESLI_REGISTER_TYPE(muesli::tests::testtypes::TEnum::Enum, "muesli.tests.testtypes.TEnum")

namespace std
{

/**
 * @brief Function object that implements a hash function for joynr::types::testtypes::TEnum.
 *
 * Used by the unordered associative containers std::unordered_set, std::unordered_multiset,
 * std::unordered_map, std::unordered_multimap as default hash function.
 */
template <>
struct hash<muesli::tests::testtypes::TEnum::Enum>
{

    /**
     * @brief method overriding default implementation of operator ()
     * @param tEnumValue the operators argument
     * @return the ordinal number representing the enum value
     */
    std::size_t operator()(const muesli::tests::testtypes::TEnum::Enum& tEnumValue) const
    {
        return muesli::tests::testtypes::TEnum::getOrdinal(tEnumValue);
    }
};
} // namespace std

#endif // MUESLI_TESTS_TESTTYPES_TENUM_H
