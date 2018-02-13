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
#include <functional>
#include <string>
#include <typeindex>
#include <type_traits>

#include <boost/mpl/joint_view.hpp>
#include <boost/optional.hpp>

#include "muesli/detail/TypeList.h"
#include "muesli/detail/FlattenMplSequence.h"
#include "muesli/detail/IncrementalTypeList.h"
#include "muesli/detail/VoidT.h"

#include "muesli/Registry.h"
#include "muesli/SkipIntroOutroWrapper.h"
#include "muesli/Tags.h"
#include "muesli/TypeRegistryFwd.h"
#include "muesli/Traits.h"

namespace muesli
{

namespace detail
{

class TypeRegistryBase
{
protected:
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
};
} // namespace detail

template <typename Base, typename InputArchive>
class TypeLoadRegistry : detail::TypeRegistryBase
{
public:
    // represents a callable which is passed an InputArchive and handles the polymorphic
    // deserialization
    // it returns a std::unique_ptr<Base>
    using LoadFunction = std::add_pointer_t<std::unique_ptr<Base>(InputArchive&)>;

    static boost::optional<LoadFunction> getLoadFunction(const std::string& typeName)
    {
        return getFunction(typeName, getLoadFunctionMap());
    }

private:
    using TypeNameToLoadFunctionMap = std::unordered_map<std::string, LoadFunction>;

    static TypeNameToLoadFunctionMap& getLoadFunctionMap()
    {
        static TypeNameToLoadFunctionMap loadFunctionMap;
        return loadFunctionMap;
    }

public:
    struct Inserter
    {
        Inserter(const std::string& typeName, LoadFunction loadFunction)
        {
            getLoadFunctionMap().insert({typeName, loadFunction});
        }
    };
};

template <typename Base, typename OutputArchive>
class TypeSaveRegistry : detail::TypeRegistryBase
{
public:
    // represents a callable which is passed an OutputArchive and a pointer to the value which shall
    // be saved
    // it handles the polymorphic serialization of the passed value
    using SaveFunction = std::add_pointer_t<void(OutputArchive&, const Base*)>;

    static boost::optional<SaveFunction> getSaveFunction(const std::type_index& typeId)
    {
        return getFunction(typeId, getSaveFunctionMap());
    }

private:
    using TypeIdToSaveFunctionMap = std::unordered_map<std::type_index, SaveFunction>;

    static TypeIdToSaveFunctionMap& getSaveFunctionMap()
    {
        static TypeIdToSaveFunctionMap saveFunctionMap;
        return saveFunctionMap;
    }

public:
    struct Inserter
    {
        Inserter(const std::type_index& typeId, SaveFunction saveFunction)
        {
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

template <typename T, typename Base, typename InputArchive>
struct RegisteredPolymorphicTypeLoadInstance
{
    static const typename ::muesli::TypeLoadRegistry<Base, InputArchive>::Inserter instance;
};

template <typename T, typename Base, typename OutputArchive>
struct RegisteredPolymorphicTypeSaveInstance
{
    static const typename ::muesli::TypeSaveRegistry<Base, OutputArchive>::Inserter instance;
};

template <typename T, typename Base, typename InputArchive>
const typename ::muesli::TypeLoadRegistry<Base, InputArchive>::Inserter
RegisteredPolymorphicTypeLoadInstance<T, Base, InputArchive>::instance(
        muesli::RegisteredType<T>::name(),
        [](InputArchive& archive) -> std::unique_ptr<Base> {
            auto value = std::make_unique<T>();
            archive(SkipIntroOutroWrapper<T>(value.get()));
            return std::move(value);
        });

template <typename T, typename Base, typename OutputArchive>
const typename ::muesli::TypeSaveRegistry<Base, OutputArchive>::Inserter
RegisteredPolymorphicTypeSaveInstance<T, Base, OutputArchive>::instance(
        typeid(T),
        [](OutputArchive& archive, const Base* ptr) { archive(*(static_cast<const T*>(ptr))); });

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

template <typename T>
struct CombineBaseAndOutputArchiveForT
{
    template <typename Base, typename OutputArchive>
    struct apply
    {
        using type = RegisteredPolymorphicTypeSaveInstance<T, Base, OutputArchive>;
    };
};

template <typename T>
struct CombineBaseAndInputArchiveForT
{
    template <typename Base, typename InputArchive>
    struct apply
    {
        using type = RegisteredPolymorphicTypeLoadInstance<T, Base, InputArchive>;
    };
};

template <typename T,
          typename BasesTypeVector,
          typename ArchiveTypeVector,
          template <typename> class CombineOperation>
using GetPolymorphicInstancesTypeVector =
        typename FlatCartesianTypeProduct<BasesTypeVector,
                                          ArchiveTypeVector,
                                          CombineOperation<T>>::type;

template <typename T, typename BasesTypeVector>
struct GetFullPolymorphicInstancesTypeList
{
    using BaseByInputArchiveCartesianProduct =
            GetPolymorphicInstancesTypeVector<T,
                                              BasesTypeVector,
                                              InputArchiveTypeVector,
                                              CombineBaseAndInputArchiveForT>;
    using BaseByOutputArchiveCartesianProduct =
            GetPolymorphicInstancesTypeVector<T,
                                              BasesTypeVector,
                                              OutputArchiveTypeVector,
                                              CombineBaseAndOutputArchiveForT>;
    using BaseByInputOutputCartesianProduct =
            typename boost::mpl::joint_view<BaseByInputArchiveCartesianProduct,
                                            BaseByOutputArchiveCartesianProduct>::type;
    using type = typename MplSequenceToTypeList<BaseByInputOutputCartesianProduct>::type;
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
        template <typename... Instances>                                                           \
        static auto dummyImpl(TypeList<Instances...>)                                              \
        {                                                                                          \
            return std::make_tuple(std::ref(Instances::instance)...);                              \
        }                                                                                          \
                                                                                                   \
        /* this function will never be called, it just forces instantation of the static object */ \
        static auto dummy()                                                                        \
        {                                                                                          \
            assert(false); /* NOLINT */                                                            \
            using BasesVector = typename TypeListToMplVector<BaseHierarchyTypeList>::type;         \
            using InstanceTypeList =                                                               \
                    typename GetFullPolymorphicInstancesTypeList<T, BasesVector>::type;            \
            return dummyImpl(InstanceTypeList{});                                                  \
        }                                                                                          \
    };                                                                                             \
    } /* namespace detail */                                                                       \
    } /* namespace muesli */

#endif // MUESLI_TYPEREGISTRY_H_
