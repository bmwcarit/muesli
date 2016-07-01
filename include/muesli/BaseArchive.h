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

#ifndef MUESLI_BASEARCHIVE_H_
#define MUESLI_BASEARCHIVE_H_

#include <memory>

#include "muesli/BaseClass.h"
#include "muesli/SkipIntroOutroWrapper.h"
#include "muesli/Tags.h"
#include "muesli/detail/Expansion.h"

namespace muesli
{

template <typename ArchiveCategory, typename Derived>
class BaseArchive : public std::enable_shared_from_this<Derived>
{
public:
    using Category = ArchiveCategory;

    BaseArchive(Derived* self) : self(*self)
    {
    }

    template <typename... Ts>
    void operator()(Ts&&... args)
    {
        // call handle() for each of the args
        detail::Expansion{0, (self.handle(std::forward<Ts>(args)), 0)...};
    }

private:
    template <typename CheckCategory, typename C>
    using EnableIfXArchive = std::enable_if_t<std::is_same<CheckCategory, C>::value>;

    template <typename T>
    T& discardConstQualifier(const T& value)
    {
        return const_cast<T&>(value);
    }

    template <typename T>
    void handle(T&& arg)
    {
        intro(self, arg);
        self.dispatch(arg);
        outro(self, arg);
    }

    template <typename T>
    void handle(BaseClass<T>&& arg)
    {
        // do no call intro/outro when serializing base classes
        self.dispatch(*arg.wrapped);
    }

    template <typename T>
    void handle(SkipIntroOutroWrapper<T>&& arg)
    {
        // do no call intro/outro for the wrapped element
        self.dispatch(*arg.wrapped);
    }

    // 'arg' has 'serialize' member function
    template <typename T, typename D = Derived>
    auto dispatch(T&& arg)
            -> decltype(discardConstQualifier(arg).serialize(static_cast<D*>(this) -> self), void())
    {
        discardConstQualifier(arg).serialize(self);
    }

    // free function 'serialize' exists for 'arg'
    template <typename T, typename D = Derived>
    auto dispatch(T&& arg)
            -> decltype(serialize(static_cast<D*>(this) -> self, discardConstQualifier(arg)),
                        void())
    {
        serialize(self, discardConstQualifier(arg));
    }

    // 'Derived' is of category 'InputArchive', check if 'load' member function exists
    template <typename T, typename D = Derived, typename C = Category>
    auto dispatch(T&& arg) -> decltype(arg.load(static_cast<D*>(this) -> self),
                                       EnableIfXArchive<tags::InputArchive, C>())
    {
        arg.load(self);
    }

    // 'Derived' is of category 'InputArchive', check if free function 'load' exists
    template <typename T, typename D = Derived, typename C = Category>
    auto dispatch(T&& arg) -> decltype(load(static_cast<D*>(this) -> self, std::forward<T>(arg)),
                                       EnableIfXArchive<tags::InputArchive, C>())
    {
        load(self, std::forward<T>(arg));
    }

    // 'Derived' is of category 'OutputArchive', check if 'load' member function exists
    template <typename T, typename D = Derived, typename C = Category>
    auto dispatch(T&& arg) -> decltype(arg.save(static_cast<D*>(this) -> self),
                                       EnableIfXArchive<tags::OutputArchive, C>())
    {
        arg.save(self);
    }

    // 'Derived' is of category 'OutputArchive', check if free function 'save' exists
    template <typename T, typename D = Derived, typename C = Category>
    auto dispatch(T&& arg) -> decltype(save(static_cast<D*>(this) -> self, std::forward<T>(arg)),
                                       EnableIfXArchive<tags::OutputArchive, C>())
    {
        save(self, std::forward<T>(arg));
    }

    Derived& self;
};

template <typename ArchiveType, typename T>
void intro(ArchiveType&, const T&)
{
    // empty default implementation
}

template <typename ArchiveType, typename T>
void outro(ArchiveType&, const T&)
{
    // empty default implementation
}

} // namespace muesli

#endif // MUESLI_BASEARCHIVE_H_
