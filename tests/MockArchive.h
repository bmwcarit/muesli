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

#ifndef MUESLI_TESTS_MOCKARCHIVE_H_
#define MUESLI_TESTS_MOCKARCHIVE_H_

#include <gmock/gmock.h>

#include <muesli/BaseArchive.h>

class MockInputArchive : public muesli::BaseArchive<muesli::tag::InputArchive, MockInputArchive>
{
public:
    using Parent = muesli::BaseArchive<muesli::tag::InputArchive, MockInputArchive>;
    MockInputArchive() : Parent(this)
    {
    }
    MOCK_METHOD1(serializeString, void(std::string&));
    MOCK_METHOD1(serializeInt64, void(std::int64_t&));
};

class MockOutputArchive : public muesli::BaseArchive<muesli::tag::OutputArchive, MockOutputArchive>
{
public:
    using Parent = muesli::BaseArchive<muesli::tag::OutputArchive, MockOutputArchive>;
    MockOutputArchive() : Parent(this)
    {
    }
    MOCK_METHOD1(serializeString, void(std::string&));
    MOCK_METHOD1(serializeInt64, void(std::int64_t&));
};

template <typename Archive>
void serialize(Archive& ar, std::string& val)
{
    ar.serializeString(val);
}

template <typename Archive>
void serialize(Archive& ar, std::int64_t& val)
{
    ar.serializeInt64(val);
}

#endif // MUESLI_TESTS_MOCKARCHIVE_H_
