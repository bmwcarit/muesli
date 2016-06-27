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

#include "TestUtil.h"

TEST(TestUtilTest, stringEqual)
{
    using namespace muesli::test_util;
    static_assert(!stringEqual("hello", "world"), "strings must NOT match");
    static_assert(!stringEqual("1234", "123456"), "strings must NOT match");
    static_assert(stringEqual("TEST", "TEST"), "strings must match");
}
