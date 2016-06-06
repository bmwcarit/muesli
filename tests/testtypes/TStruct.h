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
    static const std::uint32_t MAJOR_VERSION;
    /**
     * @brief MINOR_VERSION The minor version of this struct as specified in the
     * type collection or interface in the Franca model.
     */
    static const std::uint32_t MINOR_VERSION;

    // general methods

    // default constructor
    /** @brief Constructor */
    TStruct();

    // constructor setting all fields
    /**
     * @brief Parameterized constructor
     */
    explicit TStruct(const double& tDouble, const std::int64_t& tInt64, const std::string& tString);

    /** @brief Copy constructor */
    TStruct(const TStruct&) = default;

    /** @brief Move constructor */
    TStruct(TStruct&&) = default;

    /** @brief Destructor */
    virtual ~TStruct() = default;

    /**
     * @brief Stringifies the class
     * @return stringified class content
     */
    virtual std::string toString() const;

    /**
     * @brief Returns a hash code value for this object
     * @return a hash code value for this object.
     */
    virtual std::size_t hashCode() const;

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

    /**
     * @return a copy of this object
     */
    std::unique_ptr<TStruct> virtual clone() const;

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
    // printing TStruct with google-test and google-mock
    /**
     * @brief Print values of a TStruct object
     * @param tStruct The current object instance
     * @param os The output stream to send the output to
     */
    friend void PrintTo(const TStruct& tStruct, ::std::ostream* os);

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

private:
    // serialize TStruct with muesli
    template <typename Archive>
    friend void serialize(Archive& archive, TStruct& tStruct);

    // members
    double tDouble;
    std::int64_t tInt64;
    std::string tString;
};
// serialize TStruct with muesli
template <typename Archive>
void serialize(Archive& archive, TStruct& tStruct)
{
    archive(muesli::make_nvp("tDouble", tStruct.tDouble),
            muesli::make_nvp("tInt64", tStruct.tInt64),
            muesli::make_nvp("tString", tStruct.tString));
}

std::size_t hash_value(const TStruct& tStructValue);

} // namespace testtypes
} // namespace tests
} // namespace muesli

namespace std
{

/**
 * @brief Function object that implements a hash function for joynr::types::testtypes::TStruct.
 *
 * Used by the unordered associative containers std::unordered_set, std::unordered_multiset,
 * std::unordered_map, std::unordered_multimap as default hash function.
 */
template <>
struct hash<muesli::tests::testtypes::TStruct>
{

    /**
     * @brief method overriding default implementation of operator ()
     * @param tStructValue the operators argument
     * @return the ordinal number representing the enum value
     */
    std::size_t operator()(const muesli::tests::testtypes::TStruct& tStructValue) const
    {
        return muesli::tests::testtypes::hash_value(tStructValue);
    }
};
} // namespace std

#endif // MUESLI_TESTS_TESTTYPES_TSTRUCT_H_
