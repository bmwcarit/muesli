/*
 * #%L
 * %%
 * Copyright 2016 Randolph Voorhies and Shane Grant
 * Copyright (C) 2016 BMW Car IT GmbH
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

#ifndef MUESLI_BASECLASS_H_
#define MUESLI_BASECLASS_H_

#include <type_traits>

namespace muesli
{

// wrapper which tells the archive to serialize the base class of a given class
template <typename Base>
struct BaseClass
{
    template <typename Derived>
    BaseClass(const Derived* derived)
            : wrapped(const_cast<Base*>(static_cast<const Base*>(derived)))
    {
        static_assert(
                std::is_base_of<Base, Derived>::value, "Base must be a base class of Derived");
    }

    Base* wrapped;
};

} // namespace muesli

#endif // MUESLI_BASECLASS_H_
