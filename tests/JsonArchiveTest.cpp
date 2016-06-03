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

#include <iostream>
#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "testtypes/TStruct.h"

#include "muesli/BaseArchive.h"
#include "muesli/JsonOutputArchive.h"

// using namespace ::testing;

class JsonArchiveTest : public ::testing::Test
{
};

TEST_F(JsonArchiveTest, serializeStruct)
{
    std::stringstream stream;
    muesli::JsonOutputArchive jsonOutputArchive(stream);
    muesli::tests::testtypes::TStruct tStruct(0.42, 42, "test string data");
    jsonOutputArchive(tStruct);
    std::cout << stream.str() << std::endl;
}
