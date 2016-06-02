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

namespace muesli
{

namespace tag
{
struct InputArchive
{
};
struct OutputArchive
{
};
}

template <typename ArchiveCategory, typename Derived>
class BaseArchive
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
        static_assert(sizeof...(Ts) > 0, "at least one argument must be provided");
        using Expansion = int[];
        Expansion{0, (handle(std::forward<Ts>(args)), void(), 0)...};
    }

private:
    template <typename CheckCategory, typename C>
    using EnableIfXArchive = std::enable_if_t<std::is_same<CheckCategory, C>::value>;

    template <typename T>
    void handle(T&& arg)
    {
        intro(self, arg);
        dispatch(arg);
        outro(self, arg);
    }

    // 'arg' has 'serialize' member function
    template <typename T, typename D = Derived>
    auto dispatch(T&& arg) -> decltype(arg.serialize(static_cast<D*>(this) -> self), void())
    {
        arg.serialize(self);
    }

    // free function 'serialize' exists for 'arg'
    template <typename T, typename D = Derived>
    auto dispatch(T&& arg) -> decltype(serialize(static_cast<D*>(this) -> self, arg), void())
    {
        serialize(self, arg);
    }

    // 'Derived' is of category 'InputArchive', check if 'load' member function exists
    template <typename T, typename D = Derived, typename C = Category>
    auto dispatch(T&& arg) -> decltype(arg.load(static_cast<D*>(this) -> self),
                                       EnableIfXArchive<tag::InputArchive, C>())
    {
        arg.load(self);
    }

    // 'Derived' is of category 'InputArchive', check if free function 'load' exists
    template <typename T, typename D = Derived, typename C = Category>
    auto dispatch(T&& arg) -> decltype(load(static_cast<D*>(this) -> self, arg),
                                       EnableIfXArchive<tag::InputArchive, C>())
    {
        load(self, arg);
    }

    // 'Derived' is of category 'OutputArchive', check if 'load' member function exists
    template <typename T, typename D = Derived, typename C = Category>
    auto dispatch(T&& arg) -> decltype(arg.save(static_cast<D*>(this) -> self),
                                       EnableIfXArchive<tag::OutputArchive, C>())
    {
        arg.save(self);
    }

    // 'Derived' is of category 'OutputArchive', check if free function 'save' exists
    template <typename T, typename D = Derived, typename C = Category>
    auto dispatch(T&& arg) -> decltype(save(static_cast<D*>(this) -> self, arg),
                                       EnableIfXArchive<tag::OutputArchive, C>())
    {
        save(self, arg);
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
