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
#ifndef MUESLI_ARCHIVES_JSON_JSONOUTPUTARCHIVE_H_
#define MUESLI_ARCHIVES_JSON_JSONOUTPUTARCHIVE_H_

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/type_index.hpp>

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/writer.h>
#undef RAPIDJSON_HAS_STDSTRING
#else
#include <rapidjson/writer.h>
#endif // RAPIDJSON_HAS_STDSTRING

#include "muesli/ArchiveRegistry.h"
#include "muesli/BaseArchive.h"
#include "muesli/NameValuePair.h"
#include "muesli/Traits.h"
#include "muesli/TypeRegistryFwd.h"
#include "muesli/exceptions/UnknownTypeException.h"

#include "muesli/archives/json/Tag.h"
#include "muesli/archives/json/detail/RapidJsonOutputStreamAdapter.h"
#include "muesli/archives/json/detail/traits.h"

namespace muesli
{

template <typename OutputStream>
class JsonOutputArchive
        : public muesli::BaseArchive<muesli::tags::OutputArchive, JsonOutputArchive<OutputStream>>
{
    using Parent =
            muesli::BaseArchive<muesli::tags::OutputArchive, JsonOutputArchive<OutputStream>>;

public:
    explicit JsonOutputArchive(OutputStream& stream)
            : Parent(this), _outputStream(stream), _writer(_outputStream)
    {
    }

    void writeKey(const std::string& key)
    {
        _writer.Key(key.c_str(), static_cast<rapidjson::SizeType>(key.size()));
    }

    void writeValue(bool boolValue)
    {
        _writer.Bool(boolValue);
    }

    template <typename T>
    std::enable_if_t<std::is_arithmetic<T>::value> writeValue(const T& value)
    {
        if (!std::isnan(value)) {
            writeArithmeticValue(value);
        } else {
            _writer.Null();
        }
    }

    void writeArithmeticValue(const double& doubleValue)
    {
        _writer.Double(doubleValue);
    }

    void writeArithmeticValue(const float& floatValue)
    {
        _writer.Double(floatValue);
    }

    void writeArithmeticValue(const std::int64_t& int64Value)
    {
        _writer.Int64(int64Value);
    }

    void writeArithmeticValue(const std::uint64_t& uint64Value)
    {
        _writer.Uint64(uint64Value);
    }

    template <typename T>
    std::enable_if_t<json::detail::IsSignedIntegerUpTo32bit<T>::value> writeArithmeticValue(
            const T& value)
    {
        _writer.Int(value);
    }

    template <typename T>
    std::enable_if_t<json::detail::IsUnsignedIntegerUpTo32bit<T>::value> writeArithmeticValue(
            const T& value)
    {
        _writer.Uint(value);
    }

    void writeValue(const std::string& stringValue)
    {
        _writer.String(stringValue);
    }

    void writeValue(const char* value)
    {
        _writer.String(value);
    }

    void writeValue(const std::nullptr_t& value)
    {
        std::ignore = value;
        _writer.Null();
    }

    void startObject()
    {
        _writer.StartObject();
    }

    void endObject()
    {
        _writer.EndObject();
    }

    void startArray()
    {
        _writer.StartArray();
    }

    void endArray()
    {
        _writer.EndArray();
    }

private:
    using AdaptedStream = json::detail::RapidJsonOutputStreamAdapter<OutputStream>;
    AdaptedStream _outputStream;
    rapidjson::Writer<AdaptedStream> _writer;
};

template <typename OutputStream, typename... Ts>
void intro(JsonOutputArchive<OutputStream>& archive, const std::tuple<Ts...>& tuple)
{
    archive.startArray();
    std::ignore = tuple;
}

template <typename OutputStream, typename... Ts>
void outro(JsonOutputArchive<OutputStream>& archive, const std::tuple<Ts...>& tuple)
{
    archive.endArray();
    std::ignore = tuple;
}

template <typename OutputStream, typename TupleType, std::size_t... Indicies>
void saveTuple(JsonOutputArchive<OutputStream>& archive,
               const TupleType& tuple,
               std::index_sequence<Indicies...>)
{
    archive(std::get<Indicies>(tuple)...);
}

template <typename OutputStream, typename... Ts>
void save(JsonOutputArchive<OutputStream>& archive, const std::tuple<Ts...>& tuple)
{
    saveTuple(archive, tuple, std::index_sequence_for<Ts...>{});
}

template <typename OutputStream, typename T>
std::enable_if_t<!json::detail::IsNullable<std::decay_t<T>>::value> intro(
        JsonOutputArchive<OutputStream>& archive,
        const NameValuePair<T>& nameValuePair)
{
    archive.writeKey(nameValuePair._name);
}

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsNullable<std::decay_t<T>>::value> intro(
        JsonOutputArchive<OutputStream>& archive,
        const NameValuePair<T>& nameValuePair)
{
    if (nameValuePair._value) {
        archive.writeKey(nameValuePair._name);
    }
}

template <typename OutputStream, typename T>
void outro(JsonOutputArchive<OutputStream>& archive, const NameValuePair<T>& nameValuePair)
{
    std::ignore = archive;
    std::ignore = nameValuePair;
}

namespace detail
{
template <typename T, typename OutputStream>
std::enable_if_t<json::detail::HasRegisteredTypeName<T>::value> writeTypeName(
        JsonOutputArchive<OutputStream>& archive)
{
    archive(muesli::make_nvp("_typeName", muesli::RegisteredType<T>::name()));
}

template <typename T, typename OutputStream>
std::enable_if_t<!json::detail::HasRegisteredTypeName<T>::value> writeTypeName(
        JsonOutputArchive<OutputStream>& archive)
{
    // do nothing
    std::ignore = archive;
}
} // namespace detail

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsObject<T>::value> intro(JsonOutputArchive<OutputStream>& archive,
                                                         const T& value)
{
    archive.startObject();
    detail::writeTypeName<T>(archive);
    std::ignore = value;
}

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsObject<T>::value> outro(JsonOutputArchive<OutputStream>& archive,
                                                         const T& value)
{
    archive.endObject();
    std::ignore = value;
}

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value> intro(
        JsonOutputArchive<OutputStream>& archive,
        const T& value)
{
    std::ignore = archive;
    std::ignore = value;
}

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value> outro(
        JsonOutputArchive<OutputStream>& archive,
        const T& value)
{
    std::ignore = archive;
    std::ignore = value;
}

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsArray<T>::value> intro(JsonOutputArchive<OutputStream>& archive,
                                                        const T& value)
{
    archive.startArray();
    std::ignore = value;
}

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsArray<T>::value> outro(JsonOutputArchive<OutputStream>& archive,
                                                        const T& value)
{
    archive.endArray();
    std::ignore = value;
}

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsArray<T>::value> save(JsonOutputArchive<OutputStream>& archive,
                                                       const T& array)
{
    for (const typename T::value_type& element : array) {
        archive(element);
    }
}

template <typename T>
std::enable_if_t<std::is_same<std::string, T>::value, std::string> toString(const T& key)
{
    return key;
}

template <typename Enum, typename Wrapper = typename EnumTraits<Enum>::Wrapper>
std::enable_if_t<std::is_enum<Enum>::value, std::string> toString(const Enum& key)
{
    return Wrapper::getLiteral(key);
}

template <typename T>
std::enable_if_t<!std::is_same<std::string, T>::value && !std::is_enum<T>::value &&
                         json::detail::IsPrimitive<T>::value,
                 std::string>
toString(const T& key)
{
    return boost::lexical_cast<std::string>(key);
}

template <typename OutputStream, typename Map>
auto save(JsonOutputArchive<OutputStream>& archive, const Map& map)
        -> decltype(typename Map::mapped_type(), void())
{
    for (const auto& entry : map) {
        auto value(entry.second);
        archive(muesli::make_nvp(toString(entry.first), value));
    }
}

template <typename OutputStream, typename T>
void save(JsonOutputArchive<OutputStream>& archive, const NameValuePair<T>& nameValuePair)
{
    archive(nameValuePair._value);
}

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value && !std::is_enum<T>::value> save(
        JsonOutputArchive<OutputStream>& archive,
        const T& value)
{
    archive.writeValue(value);
}

// generic serialization for generated Enum types
template <typename OutputStream,
          typename Enum,
          typename Wrapper = typename EnumTraits<Enum>::Wrapper>
void save(JsonOutputArchive<OutputStream>& archive, Enum value)
{
    archive.writeValue(Wrapper::getLiteral(value));
}

namespace detail
{

template <typename OutputStream, typename T>
void savePointerDirectly(JsonOutputArchive<OutputStream>& archive, const T* ptr)
{
    archive(*ptr);
}

template <typename OutputStream, typename Base>
void savePolymorphicPointerThroughRegistry(JsonOutputArchive<OutputStream>& archive,
                                           const Base* ptr,
                                           const std::type_info& ptrInfo)
{
    // lookup in type registry
    using TypeRegistry =
            muesli::TypeSaveRegistry<std::decay_t<Base>, JsonOutputArchive<OutputStream>>;
    using SaveFunction = typename TypeRegistry::SaveFunction;
    boost::optional<SaveFunction> saveFunction = TypeRegistry::getSaveFunction(ptrInfo);
    if (saveFunction) {
        (*saveFunction)(archive, ptr);
    } else {
        throw exceptions::UnknownTypeException(
                std::string("could not find output serializer for " +
                            boost::typeindex::type_id_runtime(*ptr).pretty_name()));
    }
}

// generic serialization for non-polymorphic pointer types
template <typename OutputStream, typename T>
std::enable_if_t<!std::is_polymorphic<T>::value> savePointer(
        JsonOutputArchive<OutputStream>& archive,
        const T* ptr)
{
    if (ptr != nullptr) {
        savePointerDirectly(archive, ptr);
    }
}

// generic serialization for polymorphic, non-abstract pointer types
template <typename OutputStream, typename Base>
std::enable_if_t<std::is_polymorphic<Base>::value && !std::is_abstract<Base>::value> savePointer(
        JsonOutputArchive<OutputStream>& archive,
        const Base* ptr)
{
    if (ptr != nullptr) {
        const std::type_info& ptrInfo = typeid(*ptr);
        static const std::type_info& typeInfo = typeid(Base);

        if (ptrInfo == typeInfo) {
            savePointerDirectly(archive, ptr);
        } else {
            savePolymorphicPointerThroughRegistry(archive, ptr, ptrInfo);
        }
    }
}

// generic serialization for polymorphic, abstract pointer types
template <typename OutputStream, typename Base>
std::enable_if_t<std::is_polymorphic<Base>::value && std::is_abstract<Base>::value> savePointer(
        JsonOutputArchive<OutputStream>& archive,
        const Base* ptr)
{
    if (ptr != nullptr) {
        savePolymorphicPointerThroughRegistry(archive, ptr, typeid(*ptr));
    }
}
} // namespace detail

template <typename OutputStream, typename T>
void save(JsonOutputArchive<OutputStream>& archive, const std::shared_ptr<T>& ptr)
{
    detail::savePointer(archive, ptr.get());
}

template <typename OutputStream, typename T>
void save(JsonOutputArchive<OutputStream>& archive, const std::unique_ptr<T>& ptr)
{
    detail::savePointer(archive, ptr.get());
}

template <typename OutputStream, typename T>
void save(JsonOutputArchive<OutputStream>& archive, const boost::optional<T>& opt)
{
    if (opt) {
        archive(opt.get());
    }
}

} // namespace muesli

MUESLI_REGISTER_OUTPUT_ARCHIVE(muesli::JsonOutputArchive, muesli::tags::json)

#endif // MUESLI_ARCHIVES_JSON_JSONOUTPUTARCHIVE_H_
