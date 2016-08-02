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
#include "muesli/Traits.h"
#include "muesli/detail/Expansion.h"
#include "muesli/detail/DispatchTraits.h"

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
    static T& discardConstQualifier(const T& value)
    {
        return const_cast<T&>(value);
    }

    template <typename T>
    std::enable_if_t<SkipIntroOutroTraits<std::decay_t<T>>::value> handle(T&& arg)
    {
        self.dispatch(std::forward<T>(arg));
    }

    template <typename T>
    std::enable_if_t<!SkipIntroOutroTraits<std::decay_t<T>>::value> handle(T&& arg)
    {
        intro(self, std::forward<T>(arg));
        self.dispatch(std::forward<T>(arg));
        outro(self, std::forward<T>(arg));
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

    template <typename T>
    void dispatch(T&& arg)
    {
        using DispatchTarget = detail::DispatchTo<std::decay_t<T>, Derived, ArchiveCategory>;
        static_assert(!std::is_void<DispatchTarget>::value, "no serialization function found");
        dispatch(std::forward<T>(arg), DispatchTarget{});
    }

    // 'arg' has 'serialize' member function
    template <typename T>
    void dispatch(T&& arg, const dispatch_targets::member::serialize&)
    {
        discardConstQualifier(arg).serialize(self);
    }

    // free function 'serialize' exists
    template <typename T>
    void dispatch(T&& arg, const dispatch_targets::free::serialize&)
    {
        serialize(self, discardConstQualifier(arg));
    }

    // 'arg' has 'load' member function
    template <typename T>
    void dispatch(T&& arg, const dispatch_targets::member::load&)
    {
        arg.load(self);
    }

    // 'arg' has 'save' member function
    template <typename T>
    void dispatch(T&& arg, const dispatch_targets::member::save&)
    {
        discardConstQualifier(arg).save(self);
    }

    // free function 'save' exists
    template <typename T>
    void dispatch(T&& arg, const dispatch_targets::free::save&)
    {
        save(self, arg);
    }

    // free function 'load' exists
    template <typename T>
    void dispatch(T&& arg, const dispatch_targets::free::load&)
    {
        load(self, arg);
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
