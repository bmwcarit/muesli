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
#ifndef MUESLI_JSONINPUTARCHIVE_H_
#define MUESLI_JSONINPUTARCHIVE_H_

#include <cstdint>
#include <istream>
#include <string>
#include <stack>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>

#include <muesli/BaseArchive.h>
#include <muesli/TypeRegistry.h>
#include <muesli/Traits.h>
#include <muesli/cereal/NameValuePair.h>
#include <muesli/archives/json/detail/traits.h>

namespace muesli
{

class JsonInputArchive : public muesli::BaseArchive<muesli::tag::InputArchive, JsonInputArchive>
{
public:
    JsonInputArchive(std::istream& iStream)
            : muesli::BaseArchive<muesli::tag::InputArchive, JsonInputArchive>(this),
              inStream(iStream),
              document(),
              nextKey()
    {
        rapidjson::IStreamWrapper streamWrapper(iStream);
        document.ParseStream<0>(streamWrapper);
        stack.push(&document);
    }

    void setNextKey(const std::string& nextKey)
    {
        this->nextKey = nextKey;
    }

    void readValue(double& doubleValue) const
    {
        doubleValue = getNextValue()->GetDouble();
    }

    void readValue(float& floatValue) const
    {
        floatValue = getNextValue()->GetFloat();
    }

    template <typename T>
    std::enable_if_t<json::detail::IsSignedIntegerUpTo32bit<T>::value> readValue(T& value) const
    {
        value = getNextValue()->GetInt();
    }

    template <typename T>
    std::enable_if_t<json::detail::IsUnSignedIntegerUpTo32bit<T>::value> readValue(T& value) const
    {
        value = getNextValue()->GetUint();
    }

    void readValue(std::int64_t& int64Value) const
    {
        int64Value = getNextValue()->GetInt64();
    }

    void readValue(std::uint64_t& uint64Value) const
    {
        uint64Value = getNextValue()->GetUint64();
    }

    void readValue(std::string& stringValue) const
    {
        stringValue = getNextValue()->GetString();
    }

    void pushNode() {
        if(!nextKey.empty())
        {
            stack.push(getNextValue());
        }
    }

    void popNode() {
        stack.pop();
    }

private:
    rapidjson::Value* getNextValue() const
    {
        return &(stack.top()->operator[](nextKey));
        //return document->operator[](nextKey);
    }

    std::stack<rapidjson::Value*> stack;

private:
    rapidjson::IStreamWrapper inStream;
    rapidjson::Document document;
    std::string nextKey;
};

template <typename T>
void intro(JsonInputArchive& archive, NameValuePair<T>& nameValuePair)
{
}

template <typename T>
void outro(JsonInputArchive& archive, NameValuePair<T>& nameValuePair)
{
}

template <typename T>
std::enable_if_t<json::detail::IsObject<T>::value> intro(JsonInputArchive& archive, T& value)
{
    std::ignore = value;
    archive.pushNode();
}

template <typename T>
std::enable_if_t<json::detail::IsObject<T>::value> outro(JsonInputArchive& archive, const T& value)
{
    std::ignore = value;
    archive.popNode();
}

template <typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value> intro(JsonInputArchive& archive, T& value)
{
    std::ignore = value;
}

template <typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value> outro(JsonInputArchive& archive, T& value)
{
    std::ignore = value;
}

template <typename T>
std::enable_if_t<json::detail::IsArray<T>::value> intro(JsonInputArchive& archive, T& value)
{
    std::ignore = value;
}

template <typename T>
std::enable_if_t<json::detail::IsArray<T>::value> outro(JsonInputArchive& archive, T& value)
{
    std::ignore = value;
}

template <typename T>
void serialize(JsonInputArchive& archive, NameValuePair<T>& nameValuePair)
{
    archive.setNextKey(nameValuePair.name);
    archive(nameValuePair.value);
}

template <typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value && !std::is_enum<T>::value> serialize(
        JsonInputArchive& archive,
        T& value)
{
    archive.readValue(value);
}

// generic de-serialization for enum types having a wrapper class
template <typename Enum, typename Wrapper = typename EnumTraits<Enum>::Wrapper>
void load(JsonInputArchive& archive, Enum& value)
{
    std::string enumStringValue;
    archive.readValue(enumStringValue);
    value = Wrapper::getEnum(enumStringValue);
}

} // namespace muesli

#endif // MUESLI_JSONINPUTARCHIVE_H_
