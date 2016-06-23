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
#ifndef MUESLI_ARCHIVES_JSON_JSONINPUTARCHIVE_H_
#define MUESLI_ARCHIVES_JSON_JSONINPUTARCHIVE_H_

#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <stack>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/type_index.hpp>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>

#include "muesli/BaseArchive.h"
#include "muesli/Traits.h"
#include "muesli/ArchiveRegistry.h"
#include "muesli/cereal/NameValuePair.h"
#include "muesli/archives/json/detail/traits.h"
#include "muesli/concepts/InputStream.h"
#include "muesli/TypeRegistryFwd.h"
#include "muesli/exceptions/UnknownTypeException.h"
#include "muesli/SkipIntroOutroWrapper.h"

#include "detail/RapidJsonInputStreamAdapter.h"

namespace muesli
{

template <typename InputStream>
class JsonInputArchive
        : public muesli::BaseArchive<muesli::tags::InputArchive, JsonInputArchive<InputStream>>
{
    using Parent = muesli::BaseArchive<muesli::tags::InputArchive, JsonInputArchive<InputStream>>;

public:
    JsonInputArchive(InputStream& stream)
            : Parent(this),
              document(),
              nextKey(),
              nextKeyValid(false),
              nextIndex(0),
              nextIndexValid(false),
              stack(),
              stateHistoryStack()
    {
        using AdaptedStream = json::detail::RapidJsonInputStreamAdapter<InputStream>;
        AdaptedStream adaptedStream(stream);
        document.ParseStream<0>(adaptedStream);
        stack.push(&document);
    }

    void setNextKey(const std::string& nextKey)
    {
        this->nextKey = nextKey;
        this->nextKeyValid = true;
    }

    void setNextKey(std::string&& nextKey)
    {
        this->nextKey = std::move(nextKey);
        this->nextKeyValid = true;
    }

    void setNextIndex(const std::size_t nextIndex)
    {
        this->nextIndex = nextIndex;
        this->nextIndexValid = true;
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

    std::size_t getArraySize() const
    {
        assert(stack.top()->IsArray());
        return stack.top()->Size();
    }

    rapidjson::Document::ConstMemberIterator getMemberBegin() const
    {
        return stack.top()->MemberBegin();
    }

    rapidjson::Document::ConstMemberIterator getMemberEnd() const
    {
        return stack.top()->MemberEnd();
    }

    void pushNode()
    {
        rapidjson::Document::GenericValue* nextValue = getNextValue();
        if (nextValue != nullptr) {
            stack.push(nextValue);
        }
    }

    void popNode()
    {
        stack.pop();
        nextKeyValid = false;
        nextIndexValid = false;
    }

    void pushState() {
        stateHistoryStack.push(stack);
    }

    void popState()
    {
        assert(!stateHistoryStack.empty());
        stack = stateHistoryStack.top();
        nextKeyValid = false;
        nextIndexValid = false;
        stateHistoryStack.pop();
    }

private:
    rapidjson::Document::GenericValue* getNextValue() const
    {
        if (stack.top()->IsArray() && nextIndexValid) {
            return &(stack.top()->operator[](nextIndex));
        } else if (stack.top()->IsObject() && nextKeyValid) {
            return &(stack.top()->operator[](nextKey));
        }
        return nullptr;
    }

private:
    rapidjson::Document document;
    std::string nextKey;
    bool nextKeyValid;
    std::size_t nextIndex;
    bool nextIndexValid;
    using ValueStack = std::stack<rapidjson::Document::GenericValue*>;
    ValueStack stack;
    std::stack<ValueStack> stateHistoryStack;
};

template <typename InputStream, typename T>
void intro(JsonInputArchive<InputStream>& archive, NameValuePair<T>& nameValuePair)
{
    std::ignore = archive;
    std::ignore = nameValuePair;
}

template <typename InputStream, typename T>
void outro(JsonInputArchive<InputStream>& archive, NameValuePair<T>& nameValuePair)
{
    std::ignore = archive;
    std::ignore = nameValuePair;
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsObject<T>::value || json::detail::IsArray<T>::value ||
                 json::detail::IsPointer<T>::value>
intro(JsonInputArchive<InputStream>& archive, T& value)
{
    std::ignore = value;
    archive.pushNode();
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsObject<T>::value || json::detail::IsArray<T>::value ||
                 json::detail::IsPointer<T>::value>
outro(JsonInputArchive<InputStream>& archive, const T& value)
{
    std::ignore = value;
    archive.popNode();
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value> intro(JsonInputArchive<InputStream>& archive,
                                                            T& value)
{
    std::ignore = archive;
    std::ignore = value;
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value> outro(JsonInputArchive<InputStream>& archive,
                                                            T& value)
{
    std::ignore = archive;
    std::ignore = value;
}

template <typename InputStream, typename T>
void load(JsonInputArchive<InputStream>& archive, std::vector<T>& array)
{
    std::size_t arraySize = archive.getArraySize();
    array.resize(arraySize);
    for (std::size_t i = 0; i < arraySize; i++) {
        archive.setNextIndex(i);
        archive(array[i]);
    }
}

namespace detail
{
template <typename T>
std::enable_if_t<!std::is_enum<T>::value && !std::is_same<T, std::string>::value> stringToType(
        const std::string& string,
        T& type)
{
    type = boost::lexical_cast<T>(string);
}

template <typename T>
std::enable_if_t<std::is_same<T, std::string>::value> stringToType(const std::string& string,
                                                                   T& type)
{
    type = string;
}

template <typename Enum, typename Wrapper = typename EnumTraits<Enum>::Wrapper>
std::enable_if_t<std::is_enum<Enum>::value> stringToType(const std::string& literal, Enum& enumValue)
{
    enumValue = Wrapper::getEnum(literal);
}
} // namespace detail

template <typename InputStream, typename Map>
std::enable_if_t<json::detail::IsMap<Map>::value> load(JsonInputArchive<InputStream>& archive,
                                                       Map& map)
{
    using T = typename Map::key_type;
    for (rapidjson::Document::ConstMemberIterator itr = archive.getMemberBegin();
         itr != archive.getMemberEnd();
         ++itr) {

        std::string keyString(itr->name.GetString());
        if (keyString == "_typeName") {
            continue;
        }

        T key;
        detail::stringToType(keyString, key);
        archive.setNextKey(std::move(keyString));
        archive(map[key]);
    }
}

template <typename InputStream, typename T>
void load(JsonInputArchive<InputStream>& archive, NameValuePair<T>& nameValuePair)
{
    archive.setNextKey(nameValuePair.name);
    archive(nameValuePair.value);
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value && !std::is_enum<T>::value> load(
        JsonInputArchive<InputStream>& archive,
        T& value)
{
    archive.readValue(value);
}

// generic de-serialization for enum types having a wrapper class
template <typename InputStream,
          typename Enum,
          typename Wrapper = typename EnumTraits<Enum>::Wrapper>
void load(JsonInputArchive<InputStream>& archive, Enum& value)
{
    std::string enumStringValue;
    archive.readValue(enumStringValue);
    value = Wrapper::getEnum(enumStringValue);
}

namespace detail
{
// generic de-serialization for non-polymorphic pointer types
template <typename T, typename InputStream>
std::enable_if_t<!std::is_polymorphic<T>::value, std::unique_ptr<T>> loadPointer(
        JsonInputArchive<InputStream>& archive)
{
    auto ptr = std::make_unique<T>();
    archive(SkipIntroOutroWrapper<T>(ptr.get()));
    return ptr;
}

// generic de-serialization for non-polymorphic pointer types
template <typename Base, typename InputStream>
std::enable_if_t<std::is_polymorphic<Base>::value, std::unique_ptr<Base>> loadPointer(
        JsonInputArchive<InputStream>& archive)
{
    std::unique_ptr<Base> ptr = nullptr;
    archive.setNextKey("_typeName");
    static const std::string baseTypeName = RegisteredType<Base>::name();
    std::string typeName;
    archive.readValue(typeName);
    if (baseTypeName == typeName) {
        ptr = std::make_unique<Base>();
        archive(SkipIntroOutroWrapper<Base>(ptr.get()));
    } else {
        // lookup in type registry
        auto& inputRegistry = TypeRegistry<Base>::getInputRegistry();
        auto inputFunction = inputRegistry.find(typeName);
        if (inputFunction != inputRegistry.cend()) {
            ptr = inputFunction->second(archive);
        } else {
            throw exceptions::UnknownTypeException(
                    std::string("could not find input serializer for " +
                                boost::typeindex::type_id_runtime(ptr).pretty_name()));
        }
    }
    return ptr;
}
} // namespace detail

template <typename InputStream, typename T>
void load(JsonInputArchive<InputStream>& archive, std::shared_ptr<T>& ptr)
{
    // forward to raw pointer implementation
    ptr = std::move(detail::loadPointer<T>(archive));
}

template <typename InputStream, typename T>
void load(JsonInputArchive<InputStream>& archive, std::unique_ptr<T>& ptr)
{
    // forward to raw pointer implementation
    ptr = std::move(detail::loadPointer<T>(archive));
}

} // namespace muesli

MUESLI_REGISTER_INPUT_ARCHIVE(muesli::JsonInputArchive)

#endif // MUESLI_ARCHIVES_JSON_JSONINPUTARCHIVE_H_
