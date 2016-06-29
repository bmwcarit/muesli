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

#ifndef TESTTYPES_NESTEDSTRUCTS_H_
#define TESTTYPES_NESTEDSTRUCTS_H_

#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

#include <boost/optional.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
    muesli::tests::testtypes::TStruct tStruct;

    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(muesli::make_nvp("tStruct", tStruct));
    }

    bool operator==(const NestedStruct& other) const
    {
        return tStruct == other.tStruct;
    }
};

struct NestedStructPolymorphic
{
    std::shared_ptr<muesli::tests::testtypes::TStruct> tStruct;

    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(muesli::make_nvp("tStruct", tStruct));
    }

    bool operator==(const NestedStructPolymorphic& other) const
    {
        // value comparison
        if (tStruct != nullptr && other.tStruct != nullptr) {
            return *tStruct == *(other.tStruct);
        }
        return tStruct == nullptr && other.tStruct == nullptr;
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
    TypeInContainer tStructOptional; // Can also be missing
    muesli::tests::testtypes::TStruct tStruct;

    NestedNullableContainerStruct()
            : tStructOptional(), tStruct(0.123456789, 64, "test string data")
    {
    }

    void copyToOptional()
    {
        copy(tStructOptional, tStruct);
    }

    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(muesli::make_nvp("tStructOptional", tStructOptional),
                muesli::make_nvp("tStruct", tStruct));
    }

    bool operator==(const NestedNullableContainerStruct<TypeInContainer>& other) const
    {
        // value comparison
        if (tStruct == other.tStruct) {
            if (tStructOptional && other.tStructOptional) {
                return *tStructOptional == *(other.tStructOptional);
            } else {
                return (!tStructOptional && !other.tStructOptional);
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

#endif // TESTTYPES_NESTEDSTRUCTS_H_
