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
#ifndef JSON_OUTPUT_ARCHIVE_H
#define JSON_OUTPUT_ARCHIVE_H

#include <cstdint>
#include <ostream>
#include <string>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "BaseArchive.h"
#include "TypeRegistry.h"
#include "archives/json/detail/traits.h"

namespace muesli
{


class JsonOutputArchive : public muesli::BaseArchive<muesli::tag::OutputArchive, JsonOutputArchive>
{
public:
    JsonOutputArchive(std::ostream& oStream)
            : muesli::BaseArchive<muesli::tag::OutputArchive, JsonOutputArchive>(this),
              outStream(oStream),
              writer(outStream)
    {
    }

    ~JsonOutputArchive()
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

    void writeValue(const std::int64_t& int64Value)
    {
        writer.Int64(int64Value);
    }

    void writeValue(const std::int32_t& int32Value)
    {
        writer.Int(int32Value);
    }

    void writeValue(const std::uint32_t& uint32Value)
    {
        writer.Uint(uint32Value);
    }

    void writeValue(const std::string& stringValue)
    {
        writer.String(stringValue.c_str(), static_cast<rapidjson::SizeType>(stringValue.size()));
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
    rapidjson::OStreamWrapper outStream;
    rapidjson::Writer<rapidjson::OStreamWrapper> writer;
};

template <typename T>
void intro(JsonOutputArchive& archive, const NameValuePair<T>& nameValuePair)
{
    archive.writeKey(nameValuePair.name);
}

template <typename T>
void outro(JsonOutputArchive& archive, const NameValuePair<T>& nameValuePair)
{
}

template <typename T>
std::enable_if_t<json::detail::IsObject<T>::value>
intro(JsonOutputArchive& archive, const T& value)
{
    archive.startObject();
    archive.writeKey("_typeName");
    archive.writeValue(muesli::detail::RegisteredType<T>::name());
    std::ignore = value;
}

template <typename T>
std::enable_if_t<json::detail::IsObject<T>::value>
outro(JsonOutputArchive& archive, const T& value)
{
    archive.endObject();
    std::ignore = value;
}

template <typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value>
intro(JsonOutputArchive& archive, const T& value)
{
    std::ignore = value;
}

template <typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value>
outro(JsonOutputArchive& archive, const T& value)
{
    std::ignore = value;
}

template <typename T>
std::enable_if_t<json::detail::IsArray<T>::value>
intro(JsonOutputArchive& archive, const T& value)
{
    archive.startArray();
    std::ignore = value;
}

template <typename T>
std::enable_if_t<json::detail::IsArray<T>::value>
outro(JsonOutputArchive& archive, const T& value)
{
    archive.endArray();
    std::ignore = value;
}

template <typename T>
void serialize(JsonOutputArchive& archive, const NameValuePair<T>& nameValuePair)
{
    archive(nameValuePair.value);

}

template <typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value && !std::is_enum<T>::value>
serialize(JsonOutputArchive& archive, const T& value)
{
    archive.writeValue(value);
}

// generic serialization for generated Enum types
template <typename Enum, typename Wrapper = typename json::detail::EnumTraits<Enum>::Wrapper>
void save(muesli::JsonOutputArchive& archive, Enum value)
{
    archive.writeValue(Wrapper::getLiteral(value));
}

} // namespace muesli

#endif // JSON_OUTPUT_ARCHIVE_H
