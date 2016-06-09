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

#include <muesli/BaseArchive.h>

#include "MockArchive.h"

using namespace ::testing;

template <typename Archive>
class MockClassWithSerializeMemberFunction
{
public:
    MOCK_METHOD1_T(serialize, void(Archive&));
};

template <typename Archive>
class MockClassWithLoadSaveMemberFunction
{
public:
    MOCK_METHOD1_T(load, void(Archive&));
    MOCK_METHOD1_T(save, void(Archive&));
};

// forward declaration is necessary since 'serialize' must be declared prior to using it as a friend
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

    FRIEND_TEST(BaseArchiveTest, SaveClassWithFriendSerializeFunction);
    FRIEND_TEST(BaseArchiveTest, LoadClassWithFriendSerializeFunction);
    FRIEND_TEST(BaseArchiveTest, SerializingBaseClassCallsSerializeOfBaseClass);

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
void save(Archive& archive, MockClassWithFriendLoadSaveFunction<Archive>& data)
{
    // call private member function to check if friend access works
    data.saveCalled(archive);
}

template <typename Archive>
class MockClassWithFriendLoadSaveFunction
{
public:
    friend void load<>(Archive&, MockClassWithFriendLoadSaveFunction&);
    friend void save<>(Archive&, MockClassWithFriendLoadSaveFunction&);

    FRIEND_TEST(BaseArchiveTest, SaveClassWithFriendLoadSaveFunction);
    FRIEND_TEST(BaseArchiveTest, LoadClassWithFriendLoadSaveFunction);

private:
    MOCK_METHOD1_T(loadCalled, void(Archive&));
    MOCK_METHOD1_T(saveCalled, void(Archive&));
};

TEST(BaseArchiveTest, SaveClassWithSerializeMemberFunction)
{
    MockOutputArchive outputArchive;
    MockClassWithSerializeMemberFunction<MockOutputArchive> data;
    EXPECT_CALL(data, serialize(Ref(outputArchive))).Times(1);
    outputArchive(data);
}

TEST(BaseArchiveTest, LoadClassWithSerializeMemberFunction)
{
    MockInputArchive inputArchive;
    MockClassWithSerializeMemberFunction<MockInputArchive> data;
    EXPECT_CALL(data, serialize(Ref(inputArchive))).Times(1);
    inputArchive(data);
}

TEST(BaseArchiveTest, SaveClassWithWithLoadSaveMemberFunction)
{
    MockOutputArchive outputArchive;
    MockClassWithLoadSaveMemberFunction<MockOutputArchive> data;
    EXPECT_CALL(data, save(Ref(outputArchive))).Times(1);
    EXPECT_CALL(data, load(Ref(outputArchive))).Times(0);
    outputArchive(data);
}

TEST(BaseArchiveTest, LoadClassWithWithLoadSaveMemberFunction)
{
    MockInputArchive inputArchive;
    MockClassWithLoadSaveMemberFunction<MockInputArchive> data;
    EXPECT_CALL(data, load(Ref(inputArchive))).Times(1);
    EXPECT_CALL(data, save(Ref(inputArchive))).Times(0);
    inputArchive(data);
}

TEST(BaseArchiveTest, SaveClassWithFriendSerializeFunction)
{
    MockOutputArchive outputArchive;
    MockClassWithFriendSerializeFunction<MockOutputArchive> data;
    EXPECT_CALL(data, serializeCalled(Ref(outputArchive))).Times(1);
    outputArchive(data);
}

TEST(BaseArchiveTest, LoadClassWithFriendSerializeFunction)
{
    MockInputArchive inputArchive;
    MockClassWithFriendSerializeFunction<MockInputArchive> data;
    EXPECT_CALL(data, serializeCalled(Ref(inputArchive))).Times(1);
    inputArchive(data);
}

TEST(BaseArchiveTest, SaveClassWithFriendLoadSaveFunction)
{
    MockOutputArchive outputArchive;
    MockClassWithFriendLoadSaveFunction<MockOutputArchive> data;
    EXPECT_CALL(data, saveCalled(Ref(outputArchive))).Times(1);
    EXPECT_CALL(data, loadCalled(Ref(outputArchive))).Times(0);
    outputArchive(data);
}

TEST(BaseArchiveTest, LoadClassWithFriendLoadSaveFunction)
{
    MockInputArchive inputArchive;
    MockClassWithFriendLoadSaveFunction<MockInputArchive> data;
    EXPECT_CALL(data, loadCalled(Ref(inputArchive))).Times(1);
    EXPECT_CALL(data, saveCalled(Ref(inputArchive))).Times(0);
    inputArchive(data);
}

TEST(BaseArchiveTest, SerializingBaseClassCallsSerializeOfBaseClass)
{
    MockInputArchive inputArchive;
    DerivedMockClassWithFriendSerializeFunction<MockInputArchive> derived;
    EXPECT_CALL(derived, serializeCalled(Ref(inputArchive))).Times(1);
    inputArchive(derived);
}
