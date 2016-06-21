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

#include <cstdint>
#include <map>
#include <ostream>
#include <string>
#include <boost/lexical_cast.hpp>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "muesli/BaseArchive.h"
#include "muesli/NameValuePair.h"
#include "muesli/Traits.h"
#include "muesli/TypeRegistryFwd.h"
#include "muesli/ArchiveRegistry.h"
#include "muesli/archives/json/detail/traits.h"
#include "muesli/concepts/OutputStream.h"

#include "detail/RapidJsonOutputStreamAdapter.h"

namespace muesli
{

template <typename OutputStream>
class JsonOutputArchive
        : public muesli::BaseArchive<muesli::tags::OutputArchive, JsonOutputArchive<OutputStream>>
{
    using Parent =
            muesli::BaseArchive<muesli::tags::OutputArchive, JsonOutputArchive<OutputStream>>;

public:
    JsonOutputArchive(OutputStream& stream)
            : Parent(this), outputStream(stream), writer(outputStream)
    {
    }

    void writeKey(const std::string& key)
    {
        writer.Key(key.c_str(), static_cast<rapidjson::SizeType>(key.size()));
    }

    void writeValue(const double& doubleValue)
    {
        writer.Double(doubleValue);
    }

    void writeValue(const float& floatValue)
    {
        writer.Double(floatValue);
    }

    void writeValue(const std::int64_t& int64Value)
    {
        writer.Int64(int64Value);
    }

    void writeValue(const std::uint64_t& uint64Value)
    {
        writer.Uint64(uint64Value);
    }

    template <typename T>
    std::enable_if_t<json::detail::IsSignedIntegerUpTo32bit<T>::value> writeValue(const T& value)
    {
        writer.Int(value);
    }

    template <typename T>
    std::enable_if_t<json::detail::IsUnSignedIntegerUpTo32bit<T>::value> writeValue(const T& value)
    {
        writer.Uint(value);
    }

    void writeValue(const std::string& stringValue)
    {
        writer.String(stringValue);
    }

    void startObject()
    {
        writer.StartObject();
    }

    void endObject()
    {
        writer.EndObject();
    }

    void startArray()
    {
        writer.StartArray();
    }

    void endArray()
    {
        writer.EndArray();
    }

private:
    using AdaptedStream = json::detail::RapidJsonOutputStreamAdapter<OutputStream>;
    AdaptedStream outputStream;
    rapidjson::Writer<AdaptedStream> writer;
};

template <typename OutputStream, typename T>
void intro(JsonOutputArchive<OutputStream>& archive, const NameValuePair<T>& nameValuePair)
{
    archive.writeKey(nameValuePair.name);
}

template <typename OutputStream, typename T>
void outro(JsonOutputArchive<OutputStream>& archive, const NameValuePair<T>& nameValuePair)
{
}

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsObject<T>::value> intro(JsonOutputArchive<OutputStream>& archive,
                                                         const T& value)
{
    archive.startObject();
    archive(muesli::make_nvp("_typeName", muesli::RegisteredType<T>::name()));
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
    std::ignore = value;
}

template <typename OutputStream, typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value> outro(
        JsonOutputArchive<OutputStream>& archive,
        const T& value)
{
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
std::enable_if_t<json::detail::IsArray<T>::value> save(JsonOutputArchive<OutputStream>& archive, const T& array)
{
    for (const auto& element : array) {
        archive(element);
    }
}

template <typename T>
std::enable_if_t<std::is_same<std::string, T>::value, std::string> toString(const T& key) {
    return key;
}

template <typename Enum,
          typename Wrapper = typename EnumTraits<Enum>::Wrapper>
std::enable_if_t<std::is_enum<Enum>::value, std::string> toString(const Enum& key) {
    return Wrapper::getLiteral(key);
}

template <typename T>
std::enable_if_t<!std::is_same<std::string, T>::value && !std::is_enum<T>::value && json::detail::IsPrimitive<T>::value, std::string> toString(const T& key) {
    return boost::lexical_cast<std::string>(key);
}

template <typename OutputStream, typename Map>
auto save(JsonOutputArchive<OutputStream>& archive, const Map& map) -> decltype(typename Map::mapped_type(), void())
{
    for (const auto& entry : map) {
        auto value(entry.second);
        archive(muesli::make_nvp(toString(entry.first), value));
    }
}

template <typename OutputStream, typename T>
void save(JsonOutputArchive<OutputStream>& archive, const NameValuePair<T>& nameValuePair)
{
    archive(nameValuePair.value);
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

} // namespace muesli

MUESLI_REGISTER_OUTPUT_ARCHIVE(muesli::JsonOutputArchive);

#endif // MUESLI_ARCHIVES_JSON_JSONOUTPUTARCHIVE_H_
