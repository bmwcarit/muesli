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

#ifndef MUESLI_TESTS_UNIT_TESTS_TESTUTIL_H_
#define MUESLI_TESTS_UNIT_TESTS_TESTUTIL_H_

namespace muesli
{
namespace test_util
{
// this function can be evaluated at compile time
// and its result can be used in static_assert
constexpr bool stringEqual(const char* lhs, const char* rhs)
{
    return *lhs == *rhs && (*lhs == '\0' || stringEqual(lhs + 1, rhs + 1));
}
} // namespace test_util
} // namespace muesli
#endif // MUESLI_TESTS_UNIT_TESTS_TESTUTIL_H_
