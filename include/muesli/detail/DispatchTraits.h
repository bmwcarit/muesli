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

#ifndef MUESLI_DETAIL_DISPATCHTRAITS_H_
#define MUESLI_DETAIL_DISPATCHTRAITS_H_

#include <functional>

#include "muesli/detail/MetaSwitch.h"
#include "muesli/detail/MetaLazyEval.h"

namespace muesli
{

namespace dispatch_targets
{
namespace member
{
struct serialize
{
};
struct load
{
};
struct save
{
};
} // namespace member
namespace free
{
struct serialize
{
};
struct load
{
};
struct save
{
};
} // namespace free
} // namespace dispatch_targets

template <typename Archive, typename T>
struct ExplicitlyDispatchTo;

namespace detail
{
template <typename Archive, typename T, typename Enable = void>
struct HasExplicitlyDispatchTo : std::false_type
{
};

template <typename Archive, typename T>
struct HasExplicitlyDispatchTo<Archive, T, decltype(void(sizeof(ExplicitlyDispatchTo<Archive, T>)))>
        : std::true_type
{
};

namespace function_tags
{
struct serialize;
struct load;
struct save;

template <typename T>
struct IsConst : std::false_type
{
};

template <>
struct IsConst<save> : std::true_type
{
};
} // namespace function_tags

template <typename Archive, typename T>
std::false_type serialize(Archive&, T&);
template <typename Archive, typename T>
std::false_type load(Archive&, T&);
template <typename Archive, typename T>
std::false_type save(Archive&, const T&);

template <typename Tag, typename Archive, typename T>
struct HasMemberFunction;

template <typename Tag, typename Archive, typename T>
struct HasNonInheritedTemplateMemberFunction;

template <typename Tag, typename Archive, typename T>
struct HasNonInheritedMemberFunction;

template <typename Tag, typename Archive, typename T>
struct HasTemplateMemberFunction;

template <typename Tag, typename Archive, typename T>
struct HasMemberFunction;

#define MUESLI_DEFINE_TRAIT_HAS_MEMBER_FUNCTION(FunctionName)                                      \
    template <typename Archive, typename T>                                                        \
    struct HasMemberFunction<function_tags::FunctionName, Archive, T>                              \
    {                                                                                              \
        template <typename X>                                                                      \
        static auto check(int)                                                                     \
                -> decltype(std::result_of_t<decltype(&X::FunctionName)(X, Archive&)>(),           \
                            std::true_type());                                                     \
        template <typename X>                                                                      \
        static auto check(char) -> std::false_type;                                                \
        static constexpr bool value = decltype(check<T>(0))::value;                                \
    };

#define MUESLI_DEFINE_TRAIT_HAS_TEMPLATE_MEMBER_FUNCTION(FunctionName)                             \
    template <typename Archive, typename T>                                                        \
    struct HasTemplateMemberFunction<function_tags::FunctionName, Archive, T>                      \
    {                                                                                              \
        template <typename X>                                                                      \
        static auto check(int) -> decltype(                                                        \
                std::declval<X>().template FunctionName<Archive&>(std::declval<Archive&>()),       \
                std::true_type());                                                                 \
                                                                                                   \
        template <typename X>                                                                      \
        static auto check(...) -> std::false_type;                                                 \
        static constexpr bool value = decltype(check<T>(0))::value;                                \
    };

#define MUESLI_DEFINE_TRAIT_HAS_NONINHERITED_TEMPLATE_MEMBER_FUNCTION(FunctionName)                \
    template <typename Archive, typename T>                                                        \
    struct HasNonInheritedTemplateMemberFunction<function_tags::FunctionName, Archive, T>          \
    {                                                                                              \
        static constexpr bool value = std::is_same<decltype(&T::template FunctionName<Archive&>),  \
                                                   void (T::*)(Archive&)>::value;                  \
    };

#define MUESLI_DEFINE_TRAIT_HAS_NONINHERITED_MEMBER_FUNCTION(FunctionName)                         \
    template <typename Archive, typename T>                                                        \
    struct HasNonInheritedMemberFunction<function_tags::FunctionName, Archive, T>                  \
    {                                                                                              \
        static constexpr bool value =                                                              \
                std::is_same<decltype(&T::FunctionName), void (T::*)(Archive&)>::value;            \
    };

template <typename Tag, typename Archive, typename T>
struct HasFreeFunction;

template <typename Tag, typename Archive, typename T>
struct HasExactFreeFunction;

#define MUESLI_DEFINE_TRAIT_HAS_FREE_FUNCTION(FunctionName)                                        \
    template <typename Archive, typename T>                                                        \
    struct HasFreeFunction<function_tags::FunctionName, Archive, T>                                \
    {                                                                                              \
        template <typename... Args>                                                                \
        static auto check(int) -> decltype(                                                        \
                std::enable_if_t<                                                                  \
                        std::is_void<decltype(FunctionName(std::declval<Args>()...))>::value>(),   \
                std::true_type());                                                                 \
        template <typename... Args>                                                                \
        static auto check(...) -> std::false_type;                                                 \
        using CheckT =                                                                             \
                std::conditional_t<function_tags::IsConst<function_tags::FunctionName>::value,     \
                                   const T,                                                        \
                                   T>;                                                             \
        static constexpr bool value = decltype(check<Archive&, CheckT&>(0))::value;                \
    };

#define MUESLI_DEFINE_TRAIT_HAS_EXACT_FREE_FUNCTION(FunctionName)                                  \
    template <typename Archive, typename T>                                                        \
    struct HasExactFreeFunction<function_tags::FunctionName, Archive, T>                           \
    {                                                                                              \
        template <typename... Args>                                                                \
        static auto check(int)                                                                     \
                -> decltype(std::bind<void (*)(Args...)>(FunctionName, std::declval<Args>()...),   \
                            std::true_type());                                                     \
                                                                                                   \
        template <typename... Args>                                                                \
        static auto check(...) -> std::false_type;                                                 \
        using CheckT =                                                                             \
                std::conditional_t<function_tags::IsConst<function_tags::FunctionName>::value,     \
                                   const T,                                                        \
                                   T>;                                                             \
        static constexpr bool value = decltype(check<Archive&, CheckT&>(0))::value;                \
    };

#define MUESLI_DEFINE_DISPATCH_TRAITS(FunctionName)                                                \
    MUESLI_DEFINE_TRAIT_HAS_FREE_FUNCTION(FunctionName)                                            \
    MUESLI_DEFINE_TRAIT_HAS_EXACT_FREE_FUNCTION(FunctionName)                                      \
    MUESLI_DEFINE_TRAIT_HAS_MEMBER_FUNCTION(FunctionName)                                          \
    MUESLI_DEFINE_TRAIT_HAS_NONINHERITED_MEMBER_FUNCTION(FunctionName)                             \
    MUESLI_DEFINE_TRAIT_HAS_TEMPLATE_MEMBER_FUNCTION(FunctionName)                                 \
    MUESLI_DEFINE_TRAIT_HAS_NONINHERITED_TEMPLATE_MEMBER_FUNCTION(FunctionName)

MUESLI_DEFINE_DISPATCH_TRAITS(serialize)
MUESLI_DEFINE_DISPATCH_TRAITS(load)
MUESLI_DEFINE_DISPATCH_TRAITS(save)

template <typename T, typename Archive, typename ArchiveCategory>
struct DispatchToImpl
{
    using IsInputArchive = std::is_same<ArchiveCategory, tags::InputArchive>;
    using IsOutputArchive = std::is_same<ArchiveCategory, tags::OutputArchive>;

    static_assert(IsInputArchive::value != IsOutputArchive::value,
                  "Archive must either be Input or OutputArchive");

    template <typename Function>
    using MemberFunction = HasMemberFunction<Function, Archive, T>;

    template <typename Function>
    using TemplateMemberFunction = HasTemplateMemberFunction<Function, Archive, T>;

    template <typename Function>
    using NonInheritedTemplateMemberFunction =
            MetaLazyEval<TemplateMemberFunction<Function>,
                         HasNonInheritedTemplateMemberFunction<Function, Archive, T>>;

    template <typename Function>
    using NonInheritedMemberFunction =
            MetaLazyEval<MemberFunction<Function>,
                         HasNonInheritedMemberFunction<Function, Archive, T>>;
    template <typename Function>
    using AnyNonInheritedMemberFunction =
            std::integral_constant<bool,
                                   NonInheritedTemplateMemberFunction<Function>::value ||
                                           NonInheritedMemberFunction<Function>::value>;

    template <typename Function>
    using ExactFreeFunction = HasExactFreeFunction<Function, Archive, T>;

    template <typename Function>
    using FreeFunction = HasFreeFunction<Function, Archive, T>;

    using type = MetaSwitchT<
            MetaCase<HasExplicitlyDispatchTo<Archive, T>::value, ExplicitlyDispatchTo<Archive, T>>,
            MetaCase<AnyNonInheritedMemberFunction<function_tags::serialize>::value,
                     dispatch_targets::member::serialize>,
            MetaCase<MetaLazyEval<IsInputArchive,
                                  NonInheritedTemplateMemberFunction<function_tags::load>>::value,
                     dispatch_targets::member::load>,
            MetaCase<MetaLazyEval<IsInputArchive,
                                  NonInheritedMemberFunction<function_tags::load>>::value,
                     dispatch_targets::member::load>,
            MetaCase<MetaLazyEval<IsOutputArchive,
                                  NonInheritedTemplateMemberFunction<function_tags::save>>::value,
                     dispatch_targets::member::save>,
            MetaCase<MetaLazyEval<IsOutputArchive,
                                  NonInheritedMemberFunction<function_tags::save>>::value,
                     dispatch_targets::member::save>,
            MetaCase<ExactFreeFunction<function_tags::serialize>::value,
                     dispatch_targets::free::serialize>,
            MetaCase<MetaLazyEval<IsInputArchive, ExactFreeFunction<function_tags::load>>::value,
                     dispatch_targets::free::load>,
            MetaCase<MetaLazyEval<IsOutputArchive, ExactFreeFunction<function_tags::save>>::value,
                     dispatch_targets::free::save>,
            MetaCase<TemplateMemberFunction<function_tags::serialize>::value,
                     dispatch_targets::member::serialize>,
            MetaCase<MemberFunction<function_tags::serialize>::value,
                     dispatch_targets::member::serialize>,
            MetaCase<MetaLazyEval<IsInputArchive, MemberFunction<function_tags::load>>::value,
                     dispatch_targets::member::load>,
            MetaCase<MetaLazyEval<IsInputArchive,
                                  TemplateMemberFunction<function_tags::load>>::value,
                     dispatch_targets::member::load>,
            MetaCase<MetaLazyEval<IsOutputArchive, MemberFunction<function_tags::save>>::value,
                     dispatch_targets::member::save>,
            MetaCase<MetaLazyEval<IsOutputArchive,
                                  TemplateMemberFunction<function_tags::save>>::value,
                     dispatch_targets::member::save>,
            MetaCase<FreeFunction<function_tags::serialize>::value,
                     dispatch_targets::free::serialize>,
            MetaCase<MetaLazyEval<IsInputArchive, FreeFunction<function_tags::load>>::value,
                     dispatch_targets::free::load>,
            MetaCase<MetaLazyEval<IsOutputArchive, FreeFunction<function_tags::save>>::value,
                     dispatch_targets::free::save>,
            MetaDefault<void>>;
};

template <typename T, typename Archive, typename ArchiveCategory>
using DispatchTo = typename DispatchToImpl<T, Archive, ArchiveCategory>::type;

} // namespace detail
} // namespace muesli

#endif // MUESLI_DETAIL_DISPATCHTRAITS_H_
