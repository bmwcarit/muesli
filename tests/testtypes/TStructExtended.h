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

#ifndef MUESLI_TESTS_TESTTYPES_TSTRUCTEXTENDED_H_
#define MUESLI_TESTS_TESTTYPES_TSTRUCTEXTENDED_H_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include <typeinfo>

#include "muesli/BaseClass.h"
#include "muesli/TypeRegistry.h"

// include complex Datatype headers.
#include "testtypes/TStruct.h"
#include "testtypes/TEnum.h"

namespace muesli
{
namespace tests
{
namespace testtypes
{

/**
 * @version 49.13
 */
class TStructExtended : public TStruct
{

public:
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

    // general methods

    // default constructor
    /** @brief Constructor */
    TStructExtended() : TStruct(), tEnum(/* should have enum default value here */), tInt32(-1)
    {
    }

    // constructor setting all fields
    /**
     * @brief Parameterized constructor
     */
    explicit TStructExtended(const double& tDouble,
                             const std::int64_t& tInt64,
                             const std::string& tString,
                             const muesli::tests::testtypes::TEnum::Enum& tEnum,
                             const std::int32_t& tInt32)
            : TStruct(tDouble, tInt64, tString), tEnum(tEnum), tInt32(tInt32)
    {
    }

    /** @brief Copy constructor */
    TStructExtended(const TStructExtended&) = default;

    /** @brief Move constructor */
    TStructExtended(TStructExtended&&) = default;

    /** @brief Destructor */
    ~TStructExtended() override = default;

    /**
     * @brief Stringifies the class
     * @return stringified class content
     */
    std::string toString() const
    {
        std::ostringstream typeAsString;
        typeAsString << "TStructExtended{";
        typeAsString << muesli::tests::testtypes::TStruct::toString();
        typeAsString << ", ";
        typeAsString << "tEnum:" + getTEnumInternal();
        typeAsString << ", ";
        typeAsString << "tInt32:" + std::to_string(getTInt32());
        typeAsString << "}";
        return typeAsString.str();
    }

    /**
     * @brief assigns an object
     * @return reference to the object assigned to
     */
    TStructExtended& operator=(const TStructExtended&) = default;

    /**
     * @brief move assigns an object
     * @return reference to the object assigned to
     */
    TStructExtended& operator=(TStructExtended&&) = default;

    // getters
    /**
     * @brief Gets TEnum
     * @return
     */
    inline const muesli::tests::testtypes::TEnum::Enum& getTEnum() const
    {
        return tEnum;
    }
    /**
     * @brief Gets TInt32
     * @return
     */
    inline const std::int32_t& getTInt32() const
    {
        return tInt32;
    }

    // setters
    /**
     * @brief Sets TEnum
     */
    inline void setTEnum(const muesli::tests::testtypes::TEnum::Enum& tEnum)
    {
        this->tEnum = tEnum;
    }
    /**
     * @brief Sets TInt32
     */
    inline void setTInt32(const std::int32_t& tInt32)
    {
        this->tInt32 = tInt32;
    }

protected:
    // printing TStructExtended with google-test and google-mock
    /**
     * @brief Print values of a TStructExtended object
     * @param tStructExtended The current object instance
     * @param os The output stream to send the output to
     */
    friend void PrintTo(const TStructExtended& tStructExtended, ::std::ostream* os);

    /**
     * @brief equals method
     * @param other reference to the object to compare to
     * @return true if objects are equal, false otherwise
     */
    bool equals(const TStruct& other) const override
    {
        const TStructExtended& otherDerived = static_cast<const TStructExtended&>(other);
        return this->tEnum == otherDerived.tEnum && this->tInt32 == otherDerived.tInt32 &&
               TStruct::equals(other);
    }

private:
    // serialize TStructExtended with muesli
    template <typename Archive>
    friend void serialize(Archive& archive, TStructExtended& tStructExtended);

    // members
    muesli::tests::testtypes::TEnum::Enum tEnum;
    std::string getTEnumInternal() const
    {
        return muesli::tests::testtypes::TEnum::getLiteral(this->tEnum);
    }
    std::int32_t tInt32;
};

// printing TStructExtended with google-test and google-mock
inline void PrintTo(const TStructExtended& tStructExtended, ::std::ostream* os)
{
    *os << "TStructExtended::" << tStructExtended.toString();
}

// serialize TStructExtended with muesli
template <typename Archive>
void serialize(Archive& archive, TStructExtended& tStructExtended)
{
    archive(muesli::BaseClass<TStruct>(&tStructExtended),
            muesli::make_nvp("tEnum", tStructExtended.tEnum),
            muesli::make_nvp("tInt32", tStructExtended.tInt32));
}

std::size_t hash_value(const TStructExtended& tStructExtendedValue);

} // namespace testtypes
} // namespace tests
} // namespace muesli

MUESLI_REGISTER_TYPE(muesli::tests::testtypes::TStructExtended,
                     "muesli.tests.testtypes.TStructExtended")

#endif // MUESLI_TESTS_TESTTYPES_TSTRUCTEXTENDED_H_
