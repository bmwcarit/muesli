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
#include "testtypes/TStruct.h"

namespace muesli
{
namespace tests
{
namespace testtypes
{

const std::uint32_t TStruct::MAJOR_VERSION = 49;
const std::uint32_t TStruct::MINOR_VERSION = 13;

TStruct::TStruct() : tDouble(-1), tInt64(-1), tString("")
{
}

TStruct::TStruct(const double& tDouble, const std::int64_t& tInt64, const std::string& tString)
        : tDouble(tDouble), tInt64(tInt64), tString(tString)
{
}

std::size_t TStruct::hashCode() const
{
    std::size_t seed = 0;

    boost::hash_combine(seed, getTDouble());
    boost::hash_combine(seed, getTInt64());
    boost::hash_combine(seed, getTString());

    return seed;
}

std::string TStruct::toString() const
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

// printing TStruct with google-test and google-mock
void PrintTo(const TStruct& tStruct, ::std::ostream* os)
{
    *os << "TStruct::" << tStruct.toString();
}

std::size_t hash_value(const TStruct& tStructValue)
{
    return tStructValue.hashCode();
}

std::unique_ptr<TStruct> TStruct::clone() const
{
    return std::make_unique<TStruct>(const_cast<TStruct&>(*this));
}

} // namespace testtypes
} // namespace tests
} // namespace muesli
