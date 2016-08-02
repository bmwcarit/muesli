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

#include <cstdint>

#include <gtest/gtest.h>

#include "muesli/detail/IsTypeWithinList.h"
#include "muesli/detail/MetaSwitch.h"

TEST(TraitsTest, isTypeWithinList)
{
    using namespace muesli::detail;
    static_assert(IsTypeWithinList<std::int8_t, std::int16_t, std::int32_t, std::int8_t>::value,
                  "type must be found in list");
    static_assert(!IsTypeWithinList<std::int64_t, std::int16_t, std::int32_t, std::int8_t>::value,
                  "type must NOT be found in list");
}

TEST(TraitsTest, metaSwitchDefault)
{
    using namespace muesli::detail;
    using T = MetaSwitchT<MetaCase<false, int>, MetaCase<false, double>, MetaDefault<void>>;
    static_assert(std::is_void<T>::value, "type must be void");
}

TEST(TraitsTest, metaSwitchFirstCaseTrue)
{
    using namespace muesli::detail;
    using T = MetaSwitchT<MetaCase<true, int>, MetaCase<false, double>, MetaDefault<void>>;
    static_assert(std::is_same<T, int>::value, "type must be int");
}

TEST(TraitsTest, metaSwitchSecondCaseTrue)
{
    using namespace muesli::detail;
    using T = MetaSwitchT<MetaCase<false, int>, MetaCase<true, double>, MetaDefault<void>>;
    static_assert(std::is_same<T, double>::value, "type must be double");
}

TEST(TraitsTest, metaSwitchMultipleTrue)
{
    using namespace muesli::detail;
    using T = MetaSwitchT<MetaCase<true, int>, MetaCase<true, double>, MetaDefault<void>>;
    static_assert(std::is_same<T, int>::value, "type must be int");
}
