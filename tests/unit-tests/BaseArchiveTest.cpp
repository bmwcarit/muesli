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
#include <gmock/gmock.h>

#include "muesli/BaseArchive.h"
#include "muesli/detail/TemplateHolder.h"

#include "MockArchive.h"
#include "MockStream.h"

using namespace ::testing;

#define TEMPLATE_HOLDER(Type)                                                                      \
    namespace muesli                                                                               \
    {                                                                                              \
    namespace detail                                                                               \
    {                                                                                              \
    template <>                                                                                    \
    struct TemplateHolder<Type>                                                                    \
    {                                                                                              \
        template <typename... Ts>                                                                  \
        using type = Type<Ts...>;                                                                  \
    };                                                                                             \
    } /*namespace detail*/                                                                         \
    } /*namespace muesli*/

template <typename Archive>
class MockClassWithSerializeMemberFunction
{
public:
    template <typename T>
    void serialize(T& archive)
    {
        serializeCalled(archive);
    }

    MOCK_METHOD1_T(serializeCalled, void(Archive&));
};
TEMPLATE_HOLDER(MockClassWithSerializeMemberFunction)

template <typename Archive>
class MockClassWithLoadSaveMemberFunction
{
public:
    template <typename T>
    void load(T& archive)
    {
        loadCalled(archive);
    }
    MOCK_METHOD1_T(loadCalled, void(Archive&));

    template <typename T>
    void save(T& archive)
    {
        saveCalled(archive);
    }
    MOCK_METHOD1_T(saveCalled, void(Archive&));
};
TEMPLATE_HOLDER(MockClassWithLoadSaveMemberFunction)

// forward declaration is necessary since 'serialize' must be declared prior to
// using it as a friend
// within the class
template <typename Archive>
class MockClassWithFriendSerializeFunction;

template <typename Archive>
void serialize(Archive& archive, MockClassWithFriendSerializeFunction<Archive>& data)
{
    // call private member function to check if friend access works
    data.serializeCalled(archive);
}

template <typename Archive>
class MockClassWithFriendSerializeFunction
{
public:
    friend void serialize<>(Archive&, MockClassWithFriendSerializeFunction&);

    FRIEND_TEST(BaseArchiveTest, saveClassWithFriendSerializeFunction);
    FRIEND_TEST(BaseArchiveTest, loadClassWithFriendSerializeFunction);
    FRIEND_TEST(BaseArchiveTest, serializingBaseClassCallsSerializeOfBaseClass);

private:
    MOCK_METHOD1_T(serializeCalled, void(Archive&));
};

template <typename Archive>
struct DerivedMockClassWithFriendSerializeFunction
        : public MockClassWithFriendSerializeFunction<Archive>
{
};

template <typename Archive>
void serialize(Archive& archive, DerivedMockClassWithFriendSerializeFunction<Archive>& data)
{
    using Base = MockClassWithFriendSerializeFunction<Archive>;
    archive(muesli::BaseClass<Base>(&data));
}

template <typename Archive>
class MockClassWithFriendLoadSaveFunction;

template <typename Archive>
void load(Archive& archive, MockClassWithFriendLoadSaveFunction<Archive>& data)
{
    // call private member function to check if friend access works
    data.loadCalled(archive);
}

template <typename Archive>
void save(Archive& archive, const MockClassWithFriendLoadSaveFunction<Archive>& data)
{
    // call private member function to check if friend access works
    data.saveCalled(archive);
}

template <typename Archive>
class MockClassWithFriendLoadSaveFunction
{
public:
    friend void load<>(Archive&, MockClassWithFriendLoadSaveFunction&);
    friend void save<>(Archive&, const MockClassWithFriendLoadSaveFunction&);

    FRIEND_TEST(BaseArchiveTest, saveClassWithFriendLoadSaveFunction);
    FRIEND_TEST(BaseArchiveTest, loadClassWithFriendLoadSaveFunction);

private:
    MOCK_METHOD1_T(loadCalled, void(Archive&));
    MOCK_CONST_METHOD1_T(saveCalled, void(Archive&));
};

// ----- derived classes -----

template <typename Archive>
class MockClassWithSerializeMemberFunctionInheritsFromMockClassWithSerializeMemberFunction
        : public MockClassWithSerializeMemberFunction<Archive>
{
public:
    template <typename T>
    void serialize(T& archive)
    {
        archive(muesli::BaseClass<MockClassWithSerializeMemberFunction<Archive>>(this));
        serializeCalled(archive);
    }
    MOCK_METHOD1_T(serializeCalled, void(Archive&));
};
TEMPLATE_HOLDER(
        MockClassWithSerializeMemberFunctionInheritsFromMockClassWithSerializeMemberFunction)

template <typename Archive>
class MockClassWithLoadSaveMemberFunctionInheritsFromMockClassWithSerializeMemberFunction
        : public MockClassWithSerializeMemberFunction<Archive>
{
public:
    template <typename T>
    void load(T& archive)
    {
        archive(muesli::BaseClass<MockClassWithSerializeMemberFunction<Archive>>(this));
        loadCalled(archive);
    }
    MOCK_METHOD1_T(loadCalled, void(Archive&));

    template <typename T>
    void save(T& archive)
    {
        saveCalled(archive);
    }
    MOCK_METHOD1_T(saveCalled, void(Archive&));
};
TEMPLATE_HOLDER(MockClassWithLoadSaveMemberFunctionInheritsFromMockClassWithSerializeMemberFunction)

template <typename Archive>
class MockClassWithSerializeMemberFunctionInheritsFromMockClassWithLoadSaveMemberFunction
        : public MockClassWithLoadSaveMemberFunction<Archive>
{
public:
    template <typename T>
    void serialize(T& archive)
    {
        archive(muesli::BaseClass<MockClassWithLoadSaveMemberFunction<Archive>>(this));
        serializeCalled(archive);
    }
    MOCK_METHOD1_T(serializeCalled, void(Archive&));
};
TEMPLATE_HOLDER(MockClassWithSerializeMemberFunctionInheritsFromMockClassWithLoadSaveMemberFunction)

template <typename Archive>
class MockClassWithSerializeMemberFunctionInheritsFromMockClassWithLoadSaveMemberFunctionAndMockClassWithSerializeMemberFunction
        : public MockClassWithLoadSaveMemberFunction<Archive>,
          public MockClassWithSerializeMemberFunction<Archive>
{
public:
    template <typename T>
    void serialize(T& archive)
    {
        serializeCalled(archive);
    }
    MOCK_METHOD1_T(serializeCalled, void(Archive&));
};
TEMPLATE_HOLDER(
        MockClassWithSerializeMemberFunctionInheritsFromMockClassWithLoadSaveMemberFunctionAndMockClassWithSerializeMemberFunction)

using MockOutputArchiveImpl = MockOutputArchive<MockOutputStream>;
using MockInputArchiveImpl = MockInputArchive<MockInputStream>;

template <typename T>
struct ExpectSerializeMemberFunctionToBeCalledTest : public ::testing::Test
{
};

using namespace muesli::detail;
using ExpectSerializeMemberFunctionToBeCalledTypes = ::testing::Types<
        TemplateHolder<MockClassWithSerializeMemberFunction>,
        TemplateHolder<
                MockClassWithSerializeMemberFunctionInheritsFromMockClassWithLoadSaveMemberFunction>,
        TemplateHolder<
                MockClassWithSerializeMemberFunctionInheritsFromMockClassWithSerializeMemberFunction>,
        TemplateHolder<
                MockClassWithSerializeMemberFunctionInheritsFromMockClassWithLoadSaveMemberFunctionAndMockClassWithSerializeMemberFunction>>;

TYPED_TEST_CASE(ExpectSerializeMemberFunctionToBeCalledTest,
                ExpectSerializeMemberFunctionToBeCalledTypes);

TYPED_TEST(ExpectSerializeMemberFunctionToBeCalledTest, saveClassWithSerializeMemberFunction)
{
    MockOutputArchiveImpl outputArchive;
    typename TypeParam::template type<MockOutputArchiveImpl> data;
    EXPECT_CALL(data, serializeCalled(Ref(outputArchive))).Times(1);
    outputArchive(data);
}

TYPED_TEST(ExpectSerializeMemberFunctionToBeCalledTest, loadClassWithSerializeMemberFunction)
{
    MockInputArchiveImpl inputArchive;
    typename TypeParam::template type<MockInputArchiveImpl> data;
    EXPECT_CALL(data, serializeCalled(Ref(inputArchive))).Times(1);
    inputArchive(data);
}

using ExpectLoadSaveMemberFunctionToBeCalledTypes = ::testing::Types<
        TemplateHolder<MockClassWithLoadSaveMemberFunction>,
        TemplateHolder<
                MockClassWithLoadSaveMemberFunctionInheritsFromMockClassWithSerializeMemberFunction>>;

template <typename T>
struct ExpectLoadMemberFunctionToBeCalledTest : public ::testing::Test
{
};

template <typename T>
struct ExpectSaveMemberFunctionToBeCalledTest : public ::testing::Test
{
};

TYPED_TEST_CASE(ExpectLoadMemberFunctionToBeCalledTest,
                ExpectLoadSaveMemberFunctionToBeCalledTypes);

TYPED_TEST_CASE(ExpectSaveMemberFunctionToBeCalledTest,
                ExpectLoadSaveMemberFunctionToBeCalledTypes);

TYPED_TEST(ExpectSaveMemberFunctionToBeCalledTest, saveClassWithWithLoadSaveMemberFunction)
{
    MockOutputArchiveImpl outputArchive;
    typename TypeParam::template type<MockOutputArchiveImpl> data;
    EXPECT_CALL(data, saveCalled(Ref(outputArchive))).Times(1);
    EXPECT_CALL(data, loadCalled(Ref(outputArchive))).Times(0);
    outputArchive(data);
}

TYPED_TEST(ExpectLoadMemberFunctionToBeCalledTest, loadClassWithWithLoadSaveMemberFunction)
{
    MockInputArchiveImpl inputArchive;
    typename TypeParam::template type<MockInputArchiveImpl> data;
    EXPECT_CALL(data, loadCalled(Ref(inputArchive))).Times(1);
    EXPECT_CALL(data, saveCalled(Ref(inputArchive))).Times(0);
    inputArchive(data);
}

TEST(BaseArchiveTest, saveClassWithFriendSerializeFunction)
{
    MockOutputArchiveImpl outputArchive;
    MockClassWithFriendSerializeFunction<MockOutputArchiveImpl> data;
    EXPECT_CALL(data, serializeCalled(Ref(outputArchive))).Times(1);
    outputArchive(data);
}

TEST(BaseArchiveTest, loadClassWithFriendSerializeFunction)
{
    MockInputArchiveImpl inputArchive;
    MockClassWithFriendSerializeFunction<MockInputArchiveImpl> data;
    EXPECT_CALL(data, serializeCalled(Ref(inputArchive))).Times(1);
    inputArchive(data);
}

TEST(BaseArchiveTest, saveClassWithFriendLoadSaveFunction)
{
    MockOutputArchiveImpl outputArchive;
    MockClassWithFriendLoadSaveFunction<MockOutputArchiveImpl> data;
    EXPECT_CALL(data, saveCalled(Ref(outputArchive))).Times(1);
    EXPECT_CALL(data, loadCalled(Ref(outputArchive))).Times(0);
    outputArchive(data);
}

TEST(BaseArchiveTest, loadClassWithFriendLoadSaveFunction)
{
    MockInputArchiveImpl inputArchive;
    MockClassWithFriendLoadSaveFunction<MockInputArchiveImpl> data;
    EXPECT_CALL(data, loadCalled(Ref(inputArchive))).Times(1);
    EXPECT_CALL(data, saveCalled(Ref(inputArchive))).Times(0);
    inputArchive(data);
}

TEST(BaseArchiveTest, serializingBaseClassCallsSerializeOfBaseClass)
{
    MockInputArchiveImpl inputArchive;
    DerivedMockClassWithFriendSerializeFunction<MockInputArchiveImpl> derived;
    EXPECT_CALL(derived, serializeCalled(Ref(inputArchive))).Times(1);
    inputArchive(derived);
}
