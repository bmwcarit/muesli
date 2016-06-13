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

#include <boost/mpl/equal.hpp>

#include "muesli/detail/IncrementalTypeList.h"

// tags
struct Tag1;
struct Tag2;

// types
struct Foo1
{
};
struct Foo2
{
};

struct Bar1
{
};
struct Bar2
{
};

using namespace muesli::detail;
static_assert(boost::mpl::equal<MUESLI_GET_INCREMENTAL_TYPELIST(Tag1), boost::mpl::vector<>>::value,
              "list must be empty");
static_assert(boost::mpl::equal<MUESLI_GET_INCREMENTAL_TYPELIST(Tag2), boost::mpl::vector<>>::value,
              "list must be empty");

MUESLI_ADD_TO_INCREMENTAL_TYPELIST(Tag1, Foo1);
MUESLI_ADD_TO_INCREMENTAL_TYPELIST(Tag2, Bar1);

static_assert(
        boost::mpl::equal<MUESLI_GET_INCREMENTAL_TYPELIST(Tag1), boost::mpl::vector<Foo1>>::value,
        "lists must match");
static_assert(
        boost::mpl::equal<MUESLI_GET_INCREMENTAL_TYPELIST(Tag2), boost::mpl::vector<Bar1>>::value,
        "lists must match");

MUESLI_ADD_TO_INCREMENTAL_TYPELIST(Tag1, Foo2);
MUESLI_ADD_TO_INCREMENTAL_TYPELIST(Tag2, Bar2);

static_assert(boost::mpl::equal<MUESLI_GET_INCREMENTAL_TYPELIST(Tag1),
                                boost::mpl::vector<Foo1, Foo2>>::value,
              "lists must match");
static_assert(boost::mpl::equal<MUESLI_GET_INCREMENTAL_TYPELIST(Tag2),
                                boost::mpl::vector<Bar1, Bar2>>::value,
              "lists must match");
