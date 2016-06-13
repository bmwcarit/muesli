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
/*
 *
 * Copyright (C) 2011 - 2016 BMW Car IT GmbH
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

#ifndef MUESLI_TESTS_TESTTYPES_TSTRUCT_H_
#define MUESLI_TESTS_TESTTYPES_TSTRUCT_H_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include <typeinfo>

#include <muesli/TypeRegistry.h>
#include <muesli/NameValuePair.h>

// include complex Datatype headers.

namespace muesli
{
namespace tests
{
namespace testtypes
{

// forward declaration is necessary since 'serialize' must be declared prior to using it as a friend
// within the class
class TStruct;

// serialize TStruct with muesli
template <typename Archive>
void serialize(Archive& archive, TStruct& tStruct);

/**
 * @version 49.13
 */
class TStruct
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
    TStruct() : tDouble(-1), tInt64(-1), tString("")
    {
    }

    // constructor setting all fields
    /**
     * @brief Parameterized constructor
     */
    explicit TStruct(const double& tDouble, const std::int64_t& tInt64, const std::string& tString)
            : tDouble(tDouble), tInt64(tInt64), tString(tString)
    {
    }

    /** @brief Copy constructor */
    TStruct(const TStruct&) = default;

    /** @brief Move constructor */
    TStruct(TStruct&&) = default;

    /** @brief Destructor */
    virtual ~TStruct() = default;

    /**
     * @brief assigns an object
     * @return reference to the object assigned to
     */
    TStruct& operator=(const TStruct&) = default;

    /**
     * @brief move assigns an object
     * @return reference to the object assigned to
     */
    TStruct& operator=(TStruct&&) = default;

    /**
     * @brief "equal to" operator
     * @param other reference to the object to compare to
     * @return true if objects are equal, false otherwise
     */
    bool operator==(const TStruct& other) const
    {
        if (typeid(*this) != typeid(other)) {
            return false;
        }
        return this->equals(other);
    }

    /**
     * @brief "not equal to" operator
     * @param other reference to the object to compare to
     * @return true if objects are not equal, false otherwise
     */
    bool operator!=(const TStruct& other) const
    {
        return !(*this == other);
    }

    // getters
    /**
     * @brief Gets TDouble
     * @return
     */
    inline const double& getTDouble() const
    {
        return tDouble;
    }
    /**
     * @brief Gets TInt64
     * @return
     */
    inline const std::int64_t& getTInt64() const
    {
        return tInt64;
    }
    /**
     * @brief Gets TString
     * @return
     */
    inline const std::string& getTString() const
    {
        return tString;
    }

    // setters
    /**
     * @brief Sets TDouble
     */
    inline void setTDouble(const double& tDouble)
    {
        this->tDouble = tDouble;
    }
    /**
     * @brief Sets TInt64
     */
    inline void setTInt64(const std::int64_t& tInt64)
    {
        this->tInt64 = tInt64;
    }
    /**
     * @brief Sets TString
     */
    inline void setTString(const std::string& tString)
    {
        this->tString = tString;
    }

protected:
    /**
     * @brief equals method
     * @param other reference to the object to compare to
     * @return true if objects are equal, false otherwise
     */
    virtual bool equals(const TStruct& other) const
    {
        return this->tDouble == other.tDouble && this->tInt64 == other.tInt64 &&
               this->tString == other.tString;
    }

    // printing TStruct with google-test and google-mock
    /**
     * @brief Print values of a TStruct object
     * @param tStruct The current object instance
     * @param os The output stream to send the output to
     */
    friend void PrintTo(const TStruct& Print, ::std::ostream* os);

    std::string toString() const
    {
        std::ostringstream typeAsString;
        typeAsString << "TStruct{";
        typeAsString << "tDouble:" + std::to_string(getTDouble());
        typeAsString << ", ";
        typeAsString << "tInt64:" + std::to_string(getTInt64());
        typeAsString << ", ";
        typeAsString << "tString:" + getTString();
        typeAsString << "}";
        return typeAsString.str();
    }

private:
    // serialize TStruct with muesli
    template <typename Archive>
    friend void serialize(Archive& archive, TStruct& tStruct);

    // members
    double tDouble;
    std::int64_t tInt64;
    std::string tString;
};

// printing TStruct with google-test and google-mock
inline void PrintTo(const TStruct& tStruct, ::std::ostream* os)
{
    *os << "TStruct::" << tStruct.toString();
}

// serialize TStruct with muesli
template <typename Archive>
void serialize(Archive& archive, TStruct& tStruct)
{
    archive(muesli::make_nvp("tDouble", tStruct.tDouble),
            muesli::make_nvp("tInt64", tStruct.tInt64),
            muesli::make_nvp("tString", tStruct.tString));
}

} // namespace testtypes
} // namespace tests
} // namespace muesli

MUESLI_REGISTER_TYPE(muesli::tests::testtypes::TStruct, "muesli.tests.testtypes.TStruct")

#endif // MUESLI_TESTS_TESTTYPES_TSTRUCT_H_
