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

#include <gtest/gtest.h>
#include <boost/mpl/vector.hpp>

#include "MockArchive.h"
#include "MockStream.h"

#include "muesli/TypeRegistry.h"

#include "TestUtil.h"

using namespace testing;

struct NonPolymorphicType
{
};

MUESLI_REGISTER_TYPE(NonPolymorphicType, "NonPolymorphicType")

TEST(TypeRegistryTest, registeredNameIsCorrectForNonPolymorphicType)
{
    static_assert(muesli::test_util::stringEqual(
                          "NonPolymorphicType", muesli::RegisteredType<NonPolymorphicType>::name()),
                  "typenames must match");
}

struct PolymorphicBase
{
    virtual ~PolymorphicBase() = default;

    template <typename Archive>
    void serialize(Archive&)
    {
    }
};

struct PolymorphicDerived1 : PolymorphicBase
{
    std::string strValue = "TEST123";
    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(strValue);
    }
};

struct PolymorphicDerived2 : PolymorphicBase
{
    std::int64_t int64Value = 555;
    template <typename Archive>
    void serialize(Archive& archive)
    {
        archive(int64Value);
    }
};

MUESLI_REGISTER_TYPE(PolymorphicBase, "PolymorphicBase")
MUESLI_REGISTER_POLYMORPHIC_TYPE(PolymorphicDerived1, PolymorphicBase, "PolymorphicDerived1")
MUESLI_REGISTER_POLYMORPHIC_TYPE(PolymorphicDerived2, PolymorphicBase, "PolymorphicDerived2")

using MockOutputArchiveImpl = MockOutputArchive<MockOutputStream>;
using MockInputArchiveImpl = MockInputArchive<MockInputStream>;

TEST(TypeRegistryTest, registeredNameIsCorrectForPolymorphicType)
{
    static_assert(
            muesli::test_util::stringEqual(
                    "PolymorphicDerived1", muesli::RegisteredType<PolymorphicDerived1>::name()),
            "typenames must match");
}

TEST(TypeRegistryTest, polymorphicTypeInputRegistry)
{
    auto& inputRegistry = muesli::TypeRegistry<PolymorphicBase>::getInputRegistry();
    ASSERT_EQ(2, inputRegistry.size());

    MockInputArchiveImpl inputArchive;

    auto derived1Fun = inputRegistry.find("PolymorphicDerived1");
    ASSERT_NE(inputRegistry.cend(), derived1Fun);
    EXPECT_CALL(inputArchive, serializeString(Eq("TEST123")));
    auto p1 = derived1Fun->second(inputArchive);
    EXPECT_EQ(typeid(PolymorphicDerived1), typeid(*p1));

    auto derived2Fun = inputRegistry.find("PolymorphicDerived2");
    ASSERT_NE(inputRegistry.cend(), derived2Fun);
    EXPECT_CALL(inputArchive, serializeInt64(Eq(555)));
    auto p2 = derived2Fun->second(inputArchive);
    EXPECT_EQ(typeid(PolymorphicDerived2), typeid(*p2));
}

TEST(TypeRegistryTest, polymorphicTypeOutputRegistry)
{
    auto& outputRegistry = muesli::TypeRegistry<PolymorphicBase>::getOutputRegistry();
    ASSERT_EQ(2, outputRegistry.size());

    MockOutputArchiveImpl outputArchive;
    std::unique_ptr<PolymorphicBase> derived1 = std::make_unique<PolymorphicDerived1>();
    auto derived1Fun = outputRegistry.find(typeid(*derived1));
    ASSERT_NE(outputRegistry.cend(), derived1Fun);
    EXPECT_CALL(outputArchive, serializeString(_));
    derived1Fun->second(outputArchive, derived1.get());
}

// TODO 2 additional test cases: compile static and shared library which contains the datatypes,
// then perform the actual (de-)serialization in the executable which links against one of the
// libraries
