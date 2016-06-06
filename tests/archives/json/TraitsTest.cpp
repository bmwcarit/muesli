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

#include <string>
#include <vector>
#include <cstdint>

#include <gtest/gtest.h>

#include <muesli/archives/json/detail/traits.h>

class SomeClass
{
};

TEST(JsonTraitsTest, StaticAsserts)
{
    using namespace muesli::json::detail;
    static_assert(
            IsArray<std::vector<SomeClass>>::value, "std::vector<SomeClass> must be an array");
    static_assert(!IsArray<SomeClass>::value, "SomeClass must NOT be an array");
    static_assert(IsObject<SomeClass>::value, "SomeClass must be an object");
    static_assert(!IsPrimitive<SomeClass>::value, "SomeClass must NOT be primitive");

    static_assert(
            IsArray<std::vector<std::int8_t>>::value, "std::vector<std::int8_t> must be an array");
    static_assert(!IsArray<std::int8_t>::value, "std::int8_t must NOT be an array");
    static_assert(!IsObject<std::int8_t>::value, "std::int8_t must NOT be an object");
    static_assert(IsPrimitive<std::int8_t>::value, "std::int8_t must be primitive");

    static_assert(
            IsArray<std::vector<std::string>>::value, "std::vector<std::string> must be an array");
    static_assert(!IsArray<std::string>::value, "std::string must NOT be an array");
    static_assert(!IsObject<std::string>::value, "std::string must NOT be an object");
    static_assert(IsPrimitive<std::string>::value, "std::string must be primitive");
}
