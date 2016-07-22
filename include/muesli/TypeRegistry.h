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

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include "muesli/detail/FlattenMplSequence.h"
#include "muesli/detail/IncrementalTypeList.h"
#include "muesli/Tags.h"

#include "muesli/TypeRegistryFwd.h"
#include "muesli/SkipIntroOutroWrapper.h"
#include "muesli/Registry.h"
#include "muesli/Traits.h"

namespace muesli
{

template <typename Base>
class TypeRegistry
{
public:
    // represents a callable which is passed an InputArchive and handles the polymorphic
    // deserialization of
    // it returns a std::unique_ptr<Base>
    using LoadFunction = std::function<std::unique_ptr<Base>(InputArchiveVariant)>;

    // represents a callable which is passed an OutputArchive and a pointer to the value which shall
    // be saved
    // it handles the polymorphic serialization of the passed value
    using SaveFunction = std::function<void(OutputArchiveVariant, const Base*)>;

    static boost::optional<LoadFunction> getLoadFunction(const std::string& typeName)
    {
        return getFunction(typeName, getLoadFunctionMap());
    }

    static boost::optional<SaveFunction> getSaveFunction(const std::type_index& typeId)
    {
        return getFunction(typeId, getSaveFunctionMap());
    }

private:
    using TypeNameToLoadFunctionMap = std::unordered_map<std::string, LoadFunction>;
    using TypeIdToSaveFunctionMap = std::unordered_map<std::type_index, SaveFunction>;

    static TypeNameToLoadFunctionMap& getLoadFunctionMap()
    {
        static TypeNameToLoadFunctionMap loadFunctionMap;
        return loadFunctionMap;
    }

    static TypeIdToSaveFunctionMap& getSaveFunctionMap()
    {
        static TypeIdToSaveFunctionMap saveFunctionMap;
        return saveFunctionMap;
    }

    template <typename Key, typename Map, typename T = typename Map::mapped_type>
    static boost::optional<T> getFunction(const Key& key, const Map& map)
    {
        boost::optional<T> function;
        typename Map::const_iterator it = map.find(key);
        if (it != map.cend()) {
            function = it->second;
        }
        return function;
    }

public:
    struct Inserter
    {
        Inserter(const std::string& typeName,
                 const std::type_index& typeId,
                 LoadFunction loadFunction,
                 SaveFunction saveFunction)
        {
            getLoadFunctionMap().insert({typeName, loadFunction});
            getSaveFunctionMap().insert({typeId, saveFunction});
        }
    };
};

namespace detail
{

template <typename Base, typename T>
struct RegisteredPolymorphicType;

template <typename T>
struct LinkPolymorphicTypeWithBase;

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

template <typename... Ts>
struct TypeList
{
};

template <typename T, typename List = TypeList<>, typename Enable = void>
struct GetRegisteredBaseHierarchy
{
    using type = List;
};

template <typename T, typename... Bases>
struct GetRegisteredBaseHierarchy<T,
                                  TypeList<Bases...>,
                                  VoidT<typename LinkPolymorphicTypeWithBase<T>::type>>
{
    using Base = typename LinkPolymorphicTypeWithBase<T>::type;
    using type = typename GetRegisteredBaseHierarchy<Base, TypeList<Bases..., Base>>::type;
};

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
    struct LinkPolymorphicTypeWithBase<T>                                                          \
    {                                                                                              \
        using type = Base;                                                                         \
    };                                                                                             \
    template <>                                                                                    \
    struct RegisteredPolymorphicType<Base, T>                                                      \
    {                                                                                              \
        static_assert(std::is_polymorphic<Base>::value, "Base must be polymorphic");               \
        static_assert(std::is_base_of<Base, T>::value, "Base must be a base class of T");          \
                                                                                                   \
        using BaseHierarchyTypeList = typename GetRegisteredBaseHierarchy<T>::type;                \
                                                                                                   \
        template <typename... Bases>                                                               \
        static auto dummyImpl(TypeList<Bases...>)                                                  \
        {                                                                                          \
            return std::make_tuple(                                                                \
                    std::ref(RegisteredPolymorphicTypeInstance<Bases, T>::instance)...);           \
        }                                                                                          \
                                                                                                   \
        /* this function will never be called, it just forces instantation of the static object */ \
        static auto dummy()                                                                        \
        {                                                                                          \
            assert(false);                                                                         \
            return dummyImpl(BaseHierarchyTypeList{});                                             \
        }                                                                                          \
    };                                                                                             \
    } /* namespace detail */                                                                       \
    } /* namespace muesli */

#endif // MUESLI_TYPEREGISTRY_H_
