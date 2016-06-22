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

#ifndef MUESLI_TYPEREGISTRY_H_
#define MUESLI_TYPEREGISTRY_H_

#include <unordered_map>
#include <string>
#include <functional>
#include <typeindex>
#include <type_traits>

#include <boost/variant.hpp>
#include <boost/mpl/transform.hpp>

#include "muesli/detail/FlattenMplSequence.h"
#include "muesli/detail/IncrementalTypeList.h"
#include "muesli/Tags.h"

#include "muesli/TypeRegistryFwd.h"
#include "muesli/SkipIntroOutroWrapper.h"

namespace muesli
{
namespace detail
{

template <typename Stream, typename RegisteredArchive>
struct ApplyStream
{
    // get actual Archive template out of TemplateHolder
    // instantiate the combination of Archive template and concrete Stream implementation
    using type = typename RegisteredArchive::template type<Stream>;
};

template <typename RegisteredArchive, typename RegisteredStreams>
struct CartesianStreamProduct
{
    // iterate over registered streams
    using type =
            typename boost::mpl::transform<RegisteredStreams,
                                           ApplyStream<boost::mpl::_1, RegisteredArchive>>::type;
};

// iterate over registered archives
template <typename RegisteredArchives, typename RegisteredStreams>
using CartesianStreamArchiveProduct = typename boost::mpl::transform<
        RegisteredArchives,
        CartesianStreamProduct<boost::mpl::_1, RegisteredStreams>>::type;

// flatten nested type lists
template <typename RegisteredArchives, typename RegisteredStreams>
using FlatCartesianStreamArchiveProduct = typename FlattenMplSequence<
        typename CartesianStreamArchiveProduct<RegisteredArchives, RegisteredStreams>::type>::type;

template <typename RegisteredArchives, typename RegisteredStreams>
using MakeArchiveVariant = typename boost::make_variant_over<typename boost::mpl::transform<
        FlatCartesianStreamArchiveProduct<RegisteredArchives, RegisteredStreams>,
        std::add_lvalue_reference<boost::mpl::_1>>::type>::type;

using RegisteredOutputStreams = MUESLI_GET_INCREMENTAL_TYPELIST(muesli::tags::OutputStream);
using RegisteredInputStreams = MUESLI_GET_INCREMENTAL_TYPELIST(muesli::tags::InputStream);

using RegisteredOutputArchives = MUESLI_GET_INCREMENTAL_TYPELIST(muesli::tags::OutputArchive);
using RegisteredInputArchives = MUESLI_GET_INCREMENTAL_TYPELIST(muesli::tags::InputArchive);

using OutputArchiveVariant = MakeArchiveVariant<RegisteredOutputArchives, RegisteredOutputStreams>;
using InputArchiveVariant = MakeArchiveVariant<RegisteredInputArchives, RegisteredInputStreams>;
} // namespace detail

template <typename Base>
class TypeRegistry
{
public:
    using DeserializerFunction = std::function<std::unique_ptr<Base>(detail::InputArchiveVariant)>;
    using SerializerFunction = std::function<void(detail::OutputArchiveVariant, const Base*)>;

    using InputMap = std::unordered_map<std::string, DeserializerFunction>;
    using OutputMap = std::unordered_map<std::type_index, SerializerFunction>;

    static InputMap& getInputRegistry()
    {
        static InputMap inputRegistry;
        return inputRegistry;
    }

    static OutputMap& getOutputRegistry()
    {
        static OutputMap outputRegistry;
        return outputRegistry;
    }

    struct Inserter
    {
        Inserter(const std::string& name,
                 const std::type_index& typeIndex,
                 DeserializerFunction deserializerFun,
                 SerializerFunction serializerFun)
        {
            getInputRegistry().insert({name, deserializerFun});
            getOutputRegistry().insert({typeIndex, serializerFun});
        }
    };
};

namespace detail
{

template <typename Base, typename T>
struct RegisteredPolymorphicType;

template <typename Base, typename T>
struct RegisteredPolymorphicTypeInstance
{
    static const typename ::muesli::TypeRegistry<Base>::Inserter instance;
};

template <typename Base, typename T>
const typename ::muesli::TypeRegistry<Base>::Inserter RegisteredPolymorphicTypeInstance<
        Base,
        T>::instance(muesli::RegisteredType<T>::name(),
                     typeid(T),
                     [](InputArchiveVariant archive) {
                         auto value = std::make_unique<T>();
                         boost::apply_visitor(
                                 [&value](auto& ar) { ar(SkipIntroOutroWrapper<T>(value.get())); },
                                 archive);
                         return value;
                     },
                     [](OutputArchiveVariant archive, const Base* ptr) {
                         boost::apply_visitor(
                                 [ptr](auto& ar) { ar(*(static_cast<const T*>(ptr))); }, archive);
                     });

} // namespace detail
} // namespace muesli

#define MUESLI_REGISTER_TYPE(T, Name)                                                              \
    namespace muesli                                                                               \
    {                                                                                              \
    template <>                                                                                    \
    struct RegisteredType<T>                                                                       \
    {                                                                                              \
        static constexpr char const* name()                                                        \
        {                                                                                          \
            return Name;                                                                           \
        }                                                                                          \
    };                                                                                             \
    } /* namespace muesli */

#define MUESLI_REGISTER_POLYMORPHIC_TYPE(T, Base, Name)                                            \
    MUESLI_REGISTER_TYPE(T, Name) /* register T as a "normal" type as well */                      \
    namespace muesli                                                                               \
    {                                                                                              \
    namespace detail                                                                               \
    {                                                                                              \
    template <>                                                                                    \
    struct RegisteredPolymorphicType<Base, T>                                                      \
    {                                                                                              \
        static_assert(std::is_polymorphic<Base>::value, "Base must be polymorphic");               \
        static_assert(std::is_base_of<Base, T>::value, "Base must be a base class of T");          \
                                                                                                   \
        /* this function will never be called, it just forces instantation of the static object */ \
        static const auto& dummy()                                                                 \
        {                                                                                          \
            assert(false);                                                                         \
            return RegisteredPolymorphicTypeInstance<Base, T>::instance;                           \
        }                                                                                          \
    };                                                                                             \
    } /* namespace detail */                                                                       \
    } /* namespace muesli */

#endif // MUESLI_TYPEREGISTRY_H_
