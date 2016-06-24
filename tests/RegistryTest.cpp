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

#include <type_traits>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/equal.hpp>
#include <gtest/gtest.h>

#include "MockArchive.h"
#include "MockStream.h"

#include "muesli/Registry.h"

// in this test, the following Archives are included and therefore are expected to be registered:
// - MockInputArchive
// - MockOutputArchive

// since Archives are templates, they cannot be stored in a boost::mpl Sequence and have
// therefore to be wrapped into a TemplateHolder (see ArchiveRegistry.h)

static_assert(boost::mpl::equal<
                      muesli::RegisteredOutputArchives,
                      boost::mpl::vector<muesli::detail::TemplateHolder<MockOutputArchive>>>::value,
              "type vectors must match");
static_assert(boost::mpl::equal<
                      muesli::RegisteredInputArchives,
                      boost::mpl::vector<muesli::detail::TemplateHolder<MockInputArchive>>>::value,
              "type vectors must match");

// in this test, the following Streams are included and therefore are expected to be registered:
// - MockInputStream
// - MockOutputStream
static_assert(boost::mpl::equal<muesli::RegisteredInputStreams,
                                boost::mpl::vector<MockInputStream>>::value,
              "type vectors must match");
static_assert(boost::mpl::equal<muesli::RegisteredOutputStreams,
                                boost::mpl::vector<MockOutputStream>>::value,
              "type vectors must match");

// comparison of two variant types
// check if underlying type vectors are equal
template <typename V1, typename V2>
using variant_equal = boost::mpl::equal<typename V1::types, typename V2::types>;

// check that OutputArchiveVariant is as expected
static_assert(variant_equal<muesli::OutputArchiveVariant,
                            boost::variant<MockOutputArchive<MockOutputStream>&>>::value,
              "variants must match");

// check that InputArchiveVariant is as expected
static_assert(variant_equal<muesli::InputArchiveVariant,
                            boost::variant<MockInputArchive<MockInputStream>&>>::value,
              "variants must match");
