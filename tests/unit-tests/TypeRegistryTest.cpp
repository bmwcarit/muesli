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

namespace polymorphic
{

struct Base
{
    virtual ~Base() = default;
    template <typename Archive>
    void serialize(Archive&)
    {
        serializeCalledBase();
    }
    MOCK_METHOD0(serializeCalledBase, void());
};

struct DerivedOne : Base
{
    template <typename Archive>
    void serialize(Archive&)
    {
        serializeCalledDerivedOne();
    }
    MOCK_METHOD0(serializeCalledDerivedOne, void());
};

struct DerivedTwo : Base
{
    template <typename Archive>
    void serialize(Archive&)
    {
        serializeCalledDerivedTwo();
    }
    MOCK_METHOD0(serializeCalledDerivedTwo, void());
};

struct DerivedFromDerivedOne : DerivedOne
{
    template <typename Archive>
    void serialize(Archive&)
    {
        serializeCalledDerivedFromDerivedOne();
    }
    MOCK_METHOD0(serializeCalledDerivedFromDerivedOne, void());
};

} // namespace polymorphic

MUESLI_REGISTER_TYPE(polymorphic::Base, "polymorphic.Base")
MUESLI_REGISTER_POLYMORPHIC_TYPE(polymorphic::DerivedOne,
                                 polymorphic::Base,
                                 "polymorphic.DerivedOne")
MUESLI_REGISTER_POLYMORPHIC_TYPE(polymorphic::DerivedTwo,
                                 polymorphic::Base,
                                 "polymorphic.DerivedTwo")

MUESLI_REGISTER_POLYMORPHIC_TYPE(polymorphic::DerivedFromDerivedOne,
                                 polymorphic::DerivedOne,
                                 "polymorphic.DerivedFromDerivedOne")

using MockOutputArchiveImpl = MockOutputArchive<MockOutputStream>;
using MockInputArchiveImpl = MockInputArchive<MockInputStream>;

TEST(TypeRegistryTest, registeredNameIsCorrectForPolymorphicType)
{
    static_assert(
            muesli::test_util::stringEqual("polymorphic.DerivedOne",
                                           muesli::RegisteredType<polymorphic::DerivedOne>::name()),
            "typenames must match");
}

TEST(TypeRegistryTest, polymorphicTypeInputRegistry)
{
    using TypeRegistry = muesli::TypeRegistry<polymorphic::Base>;
    using LoadFunction = typename TypeRegistry::LoadFunction;

    MockInputArchiveImpl inputArchive;
    {
        boost::optional<LoadFunction> loadFunction =
                TypeRegistry::getLoadFunction("polymorphic.DerivedOne");
        ASSERT_TRUE(loadFunction.is_initialized());
        std::unique_ptr<polymorphic::Base> p = (*loadFunction)(inputArchive);
        EXPECT_EQ(typeid(polymorphic::DerivedOne), typeid(*p));
    }

    {
        boost::optional<LoadFunction> loadFunction =
                TypeRegistry::getLoadFunction("polymorphic.DerivedTwo");
        ASSERT_TRUE(loadFunction.is_initialized());
        std::unique_ptr<polymorphic::Base> p = (*loadFunction)(inputArchive);
        EXPECT_EQ(typeid(polymorphic::DerivedTwo), typeid(*p));
    }

    {
        boost::optional<LoadFunction> loadFunction =
                TypeRegistry::getLoadFunction("polymorphic.DerivedFromDerivedOne");
        ASSERT_TRUE(loadFunction.is_initialized());
        std::unique_ptr<polymorphic::Base> p = (*loadFunction)(inputArchive);
        EXPECT_EQ(typeid(polymorphic::DerivedFromDerivedOne), typeid(*p));
    }
}

TEST(TypeRegistryTest, polymorphicTypeOutputRegistry)
{
    using TypeRegistry = muesli::TypeRegistry<polymorphic::Base>;
    using SaveFunction = typename TypeRegistry::SaveFunction;

    MockOutputArchiveImpl outputArchive;
    std::unique_ptr<polymorphic::Base> derivedOne = std::make_unique<polymorphic::DerivedOne>();
    boost::optional<SaveFunction> saveFunction = TypeRegistry::getSaveFunction(typeid(*derivedOne));
    ASSERT_TRUE(saveFunction.is_initialized());
    EXPECT_CALL(dynamic_cast<polymorphic::DerivedOne&>(*derivedOne), serializeCalledDerivedOne());
    (*saveFunction)(outputArchive, derivedOne.get());
}

// TODO 2 additional test cases: compile static and shared library which contains the datatypes,
// then perform the actual (de-)serialization in the executable which links against one of the
// libraries
