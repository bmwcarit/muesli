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

#ifndef MUESLI_TESTS_UNIT_TESTS_TESTTYPES_NESTEDSTRUCTS_H_
#define MUESLI_TESTS_UNIT_TESTS_TESTTYPES_NESTEDSTRUCTS_H_

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include <boost/optional.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "muesli/exceptions/ParseException.h"
#include "muesli/exceptions/ValueNotFoundException.h"

#include "muesli/archives/json/JsonInputArchive.h"
#include "muesli/archives/json/JsonOutputArchive.h"

#include "muesli/streams/StdIStreamWrapper.h"
#include "muesli/streams/StdOStreamWrapper.h"

#include "testtypes/TStruct.h"

#include "muesli/TypeRegistry.h"

namespace muesli
{
namespace tests
{
namespace testtypes
{

struct NestedStruct
{
    muesli::tests::testtypes::TStruct _tStruct;

    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(muesli::make_nvp("_tStruct", _tStruct));
    }

    bool operator==(const NestedStruct& other) const
    {
        return _tStruct == other._tStruct;
    }
};

struct NestedStructPolymorphic
{
    std::shared_ptr<muesli::tests::testtypes::TStruct> _tStruct;

    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(muesli::make_nvp("_tStruct", _tStruct));
    }

    bool operator==(const NestedStructPolymorphic& other) const
    {
        // value comparison
        if (_tStruct != nullptr && other._tStruct != nullptr) {
            return *_tStruct == *(other._tStruct);
        }
        return _tStruct == nullptr && other._tStruct == nullptr;
    }
};

template <typename T>
void copy(std::shared_ptr<T>& ptr, const T& value)
{
    ptr = std::make_shared<T>(value);
}
template <typename T>
void copy(std::unique_ptr<T>& ptr, const T& value)
{
    ptr = std::make_unique<T>(value);
}
template <typename T>
void copy(boost::optional<T>& ptr, const T& value)
{
    ptr = value;
}

//
// TypeInContainer must implement
//    - operator!()
//    - operator*()
//
template <typename TypeInContainer>
struct NestedNullableContainerStruct
{
    TypeInContainer _tStructOptional; // Can also be missing
    muesli::tests::testtypes::TStruct _tStruct;

    NestedNullableContainerStruct()
            : _tStructOptional(), _tStruct(0.123456789, 64, "test string data")
    {
    }

    void copyToOptional()
    {
        copy(_tStructOptional, _tStruct);
    }

    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(muesli::make_nvp("tStructOptional", _tStructOptional),
                muesli::make_nvp("_tStruct", _tStruct));
    }

    bool operator==(const NestedNullableContainerStruct<TypeInContainer>& other) const
    {
        // value comparison
        if (_tStruct == other._tStruct) {
            if (_tStructOptional && other._tStructOptional) {
                return *_tStructOptional == *(other._tStructOptional);
            } else {
                return (!_tStructOptional && !other._tStructOptional);
            }
        }
        return false;
    }
};

// typedefs nullable containers to be used in tests
using NestedBoostOptionalStruct =
        NestedNullableContainerStruct<boost::optional<muesli::tests::testtypes::TStruct>>;
using NestedSharedPtrStruct =
        NestedNullableContainerStruct<std::shared_ptr<muesli::tests::testtypes::TStruct>>;
using NestedUniquePtrStruct =
        NestedNullableContainerStruct<std::unique_ptr<muesli::tests::testtypes::TStruct>>;

} // namespace testtypes
} // namespace tests
} // namespace muesli

// register base and derived with same name so that we can use the same string comparison of
// expected JSON
MUESLI_REGISTER_TYPE(muesli::tests::testtypes::NestedStruct, "muesli.tests.testtypes.NestedStruct")
MUESLI_REGISTER_TYPE(muesli::tests::testtypes::NestedStructPolymorphic,
                     "muesli.tests.testtypes.NestedStruct")

// register nullable struct types
MUESLI_REGISTER_TYPE(muesli::tests::testtypes::NestedUniquePtrStruct,
                     "muesli.tests.testtypes.NestedPtrStruct")
MUESLI_REGISTER_TYPE(muesli::tests::testtypes::NestedSharedPtrStruct,
                     "muesli.tests.testtypes.NestedPtrStruct")

MUESLI_REGISTER_TYPE(muesli::tests::testtypes::NestedBoostOptionalStruct,
                     "muesli.tests.testtypes.NestedBoostOptionalStruct")

#endif // MUESLI_TESTS_UNIT_TESTS_TESTTYPES_NESTEDSTRUCTS_H_
