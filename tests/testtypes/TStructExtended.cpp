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

#include <sstream>
#include <string>

#include <boost/functional/hash.hpp>
#include "TStructExtended.h"

namespace muesli
{
namespace tests
{
namespace testtypes
{

const std::uint32_t TStructExtended::MAJOR_VERSION = 49;
const std::uint32_t TStructExtended::MINOR_VERSION = 13;

TStructExtended::TStructExtended()
        : TStruct(), tEnum(/* should have enum default value here */), tInt32(-1)
{
}

TStructExtended::TStructExtended(const double& tDouble,
                                 const std::int64_t& tInt64,
                                 const std::string& tString,
                                 const muesli::tests::testtypes::TEnum::Enum& tEnum,
                                 const std::int32_t& tInt32)
        : TStruct(tDouble, tInt64, tString), tEnum(tEnum), tInt32(tInt32)
{
}

std::size_t TStructExtended::hashCode() const
{
    std::size_t seed = 0;

    boost::hash_combine(seed, getTEnum());
    boost::hash_combine(seed, getTInt32());

    boost::hash_combine(seed, TStruct::hashCode());
    return seed;
}

std::string TStructExtended::getTEnumInternal() const
{
    return muesli::tests::testtypes::TEnum::getLiteral(this->tEnum);
}

std::string TStructExtended::toString() const
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

// printing TStructExtended with google-test and google-mock
void PrintTo(const TStructExtended& tStructExtended, ::std::ostream* os)
{
    *os << "TStructExtended::" << tStructExtended.toString();
}

std::size_t hash_value(const TStructExtended& tStructExtendedValue)
{
    return tStructExtendedValue.hashCode();
}

std::unique_ptr<TStruct> TStructExtended::clone() const
{
    return std::make_unique<TStructExtended>(const_cast<TStructExtended&>(*this));
}

} // namespace TestTypes
} // namespace types
} // namespace joynr
