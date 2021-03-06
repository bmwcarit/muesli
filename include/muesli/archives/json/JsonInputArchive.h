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
#include <iterator>
#include <memory>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/type_index.hpp>
#include <boost/optional.hpp>

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#undef RAPIDJSON_HAS_STDSTRING
#else
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#endif // RAPIDJSON_HAS_STDSTRING

#include "muesli/ArchiveRegistry.h"
#include "muesli/BaseArchive.h"
#include "muesli/NameValuePair.h"
#include "muesli/SkipIntroOutroWrapper.h"
#include "muesli/Traits.h"
#include "muesli/TypeRegistryFwd.h"
#include "muesli/detail/Expansion.h"
#include "muesli/exceptions/ValueNotFoundException.h"
#include "muesli/exceptions/UnknownTypeException.h"
#include "muesli/exceptions/ParseException.h"

#include "muesli/archives/json/detail/traits.h"
#include "muesli/archives/json/detail/RapidJsonInputStreamAdapter.h"
#include "muesli/archives/json/Tag.h"

namespace muesli
{

template <typename InputStream>
class JsonInputArchive
        : public muesli::BaseArchive<muesli::tags::InputArchive, JsonInputArchive<InputStream>>
{
    using Parent = muesli::BaseArchive<muesli::tags::InputArchive, JsonInputArchive<InputStream>>;
    using Value = rapidjson::Document::GenericValue;

public:
    explicit JsonInputArchive(InputStream& stream)
            : Parent(this),
              _document(),
              _nextKey(),
              _nextKeyValid(false),
              _nextIndex(0),
              _nextIndexValid(false),
              _stack(),
              _stateHistoryStack(),
              _isRoot(true)
    {
        using AdaptedStream = json::detail::RapidJsonInputStreamAdapter<InputStream>;
        AdaptedStream adaptedStream(stream);
        _document.ParseStream<0>(adaptedStream);
        if (_document.HasParseError()) {
            throw exceptions::ParseException(
                    std::string("could not parse JSON: ") +
                    rapidjson::GetParseError_En(_document.GetParseError()));
        }
        _stack.push(&_document);
    }

    void setNextKey(const std::string& nextKey)
    {
        this->_nextKey = nextKey;
        this->_nextKeyValid = true;
    }

    void setNextKey(std::string&& nextKey)
    {
        this->_nextKey = std::move(nextKey);
        this->_nextKeyValid = true;
    }

    void setNextIndex(const std::size_t nextIndex)
    {
        this->_nextIndex = nextIndex;
        this->_nextIndexValid = true;
    }

    void readValue(bool& boolValue)
    {
        const Value* nextValue = getNextValue(true);
        if (nextValue->IsBool()) {
            boolValue = nextValue->GetBool();
        } else {
            throw std::invalid_argument("Cannot read a Bool.");
        }
    }

    void readValue(std::vector<bool>::reference& boolValue)
    {
        bool value;
        readValue(value);
        boolValue = value;
    }

    template <typename T>
    std::enable_if_t<std::is_arithmetic<T>::value> readValue(T& value)
    {
        const Value* nextValue = getNextValue(true);
        if (!nextValue->IsNull()) {
            readArithmeticValue(value, nextValue);
        } else {
            value = std::numeric_limits<T>::signaling_NaN();
        }
    }

    void readArithmeticValue(double& doubleValue, const Value* value) const
    {
        if (value->IsDouble() || value->IsInt()) {
            doubleValue = value->GetDouble();
        } else {
            throw std::invalid_argument("Cannot read a Double.");
        }
    }

    void readArithmeticValue(float& floatValue, const Value* value) const
    {
        double doubleValue;
        readArithmeticValue(doubleValue, value);
        floatValue = static_cast<float>(doubleValue);
    }

    template <typename T>
    std::enable_if_t<json::detail::IsSignedIntegerUpTo32bit<T>::value> readArithmeticValue(
            T& intValue,
            const Value* value) const
    {
        if (value->IsInt()) {
            intValue = value->GetInt();
        } else {
            throw std::invalid_argument("Cannot read an Int.");
        }
    }

    template <typename T>
    std::enable_if_t<json::detail::IsUnsignedIntegerUpTo32bit<T>::value> readArithmeticValue(
            T& intValue,
            const Value* value) const
    {
        if (value->IsUint()) {
            intValue = value->GetUint();
        } else {
            throw std::invalid_argument("Cannot read an Uint.");
        }
    }

    void readArithmeticValue(std::int64_t& int64Value, const Value* value) const
    {
        if (value->IsInt64()) {
            int64Value = value->GetInt64();
        } else {
            throw std::invalid_argument("Cannot read an Int64.");
        }
    }

    void readArithmeticValue(std::uint64_t& uint64Value, const Value* value) const
    {
        if (value->IsUint64()) {
            uint64Value = value->GetUint64();
        } else {
            throw std::invalid_argument("Cannot read an UInt64.");
        }
    }

    void readValue(std::string& stringValue) const
    {
        const Value* nextValue = getNextValue(true);
        if (nextValue->IsString()) {
            stringValue = nextValue->GetString();
        } else {
            throw std::invalid_argument("Cannot read a String.");
        }
    }

    std::size_t getArraySize() const
    {
        assert(currentValueIsArray());
        return _stack.top()->Size();
    }

    inline bool currentValueIsArray() const
    {
        return _stack.top()->IsArray();
    }

    rapidjson::Document::ConstMemberIterator getMemberBegin() const
    {
        return _stack.top()->MemberBegin();
    }

    rapidjson::Document::ConstMemberIterator getMemberEnd() const
    {
        return _stack.top()->MemberEnd();
    }

    void pushNode()
    {
        // TODO: benchmark following if condition "if(isRoot)"
        // It is required to skip the root object
        // but can cause slow-downs
        if (_isRoot) {
            _isRoot = false;
            return;
        }
        const Value* nextValue = getNextValue(true);
        if (!nextValue->IsNull()) {
            _stack.push(nextValue);
        } else {
            throw exceptions::ValueNotFoundException(
                    "Could not find a value for a not nullable object.");
        }
    }

    void pushNullableNode()
    {
        _stack.push(getNextValue());
    }

    void popNode()
    {
        _stack.pop();
        _nextKeyValid = false;
        _nextIndexValid = false;
    }

    void pushState()
    {
        _stateHistoryStack.push(_stack);
    }

    void popState()
    {
        assert(!_stateHistoryStack.empty());
        _stack = _stateHistoryStack.top();
        _nextKeyValid = false;
        _nextIndexValid = false;
        _stateHistoryStack.pop();
    }

    bool currentValueIsNull() const
    {
        return _stack.top() == nullptr || _stack.top()->IsNull();
    }

private:
    const Value* getNextValue(bool throwOnNotFound = false) const
    {
        if (_stack.top()->IsArray() && _nextIndexValid) {
            return &(_stack.top()->operator[](_nextIndex));
        } else if (_stack.top()->IsObject() && _nextKeyValid) {
            Value::ConstMemberIterator it = _stack.top()->FindMember(_nextKey);
            if (it != _stack.top()->MemberEnd()) {
                return &(it->value);
            }
            if (throwOnNotFound) {
                throw exceptions::ValueNotFoundException("Could not find value for key \"" +
                                                         _nextKey + "\".");
            }
            return nullptr;
        }
        return _stack.top();
    }

private:
    rapidjson::Document _document;
    std::string _nextKey;
    bool _nextKeyValid;
    std::size_t _nextIndex;
    bool _nextIndexValid;
    using ValueStack = std::stack<const Value*>;
    ValueStack _stack;
    std::stack<ValueStack> _stateHistoryStack;
    bool _isRoot;
};

namespace detail
{
template <std::size_t Index, typename InputStream, typename TupleType>
void loadTupleElement(JsonInputArchive<InputStream>& archive, TupleType& tuple)
{
    archive.setNextIndex(Index);
    archive(std::get<Index>(tuple));
}

template <typename InputStream, typename TupleType, std::size_t... Indicies>
void loadTuple(JsonInputArchive<InputStream>& archive,
               TupleType& tuple,
               std::index_sequence<Indicies...>)
{
    detail::Expansion{0, (loadTupleElement<Indicies>(archive, tuple), 0)...};
}

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
std::enable_if_t<std::is_enum<Enum>::value> stringToType(const std::string& literal,
                                                         Enum& enumValue)
{
    enumValue = Wrapper::getEnum(literal);
}

template <typename T>
std::enable_if_t<json::detail::IsArray<T>::value> reserveArray(T& array, std::size_t size)
{
    array.reserve(size);
}

template <typename T>
void reserveArray(std::set<T>& array, std::size_t size)
{
    std::ignore = array;
    std::ignore = size;
}

template <typename... Ts>
void reserveArray(boost::multi_index_container<Ts...>& array, std::size_t size)
{
    std::ignore = array;
    std::ignore = size;
}

} // namespace detail

template <typename InputStream, typename T>
void intro(JsonInputArchive<InputStream>& archive, const NameValuePair<T>& nameValuePair)
{
    std::ignore = archive;
    std::ignore = nameValuePair;
}

template <typename InputStream, typename T>
void outro(JsonInputArchive<InputStream>& archive, const NameValuePair<T>& nameValuePair)
{
    std::ignore = archive;
    std::ignore = nameValuePair;
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsObject<T>::value || json::detail::IsArray<T>::value> intro(
        JsonInputArchive<InputStream>& archive,
        const T& value)
{
    std::ignore = value;
    archive.pushNode();
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsNullable<T>::value> intro(JsonInputArchive<InputStream>& archive,
                                                           const T& value)
{
    std::ignore = value;
    archive.pushNullableNode();
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsObject<T>::value || json::detail::IsArray<T>::value ||
                 json::detail::IsNullable<T>::value>
outro(JsonInputArchive<InputStream>& archive, const T& value)
{
    std::ignore = value;
    archive.popNode();
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value> intro(JsonInputArchive<InputStream>& archive,
                                                            const T& value)
{
    std::ignore = archive;
    std::ignore = value;
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsPrimitive<T>::value> outro(JsonInputArchive<InputStream>& archive,
                                                            const T& value)
{
    std::ignore = archive;
    std::ignore = value;
}

template <typename InputStream, typename T>
std::enable_if_t<json::detail::IsArray<T>::value> load(JsonInputArchive<InputStream>& archive,
                                                       T& array)
{
    using ValueType = typename T::value_type;
    if (archive.currentValueIsArray()) {
        std::size_t arraySize = archive.getArraySize();
        array.clear();
        detail::reserveArray(array, arraySize);
        auto inserter = std::inserter(array, array.begin());
        for (std::size_t i = 0; i < arraySize; i++) {
            archive.setNextIndex(i);
            ValueType entry;
            archive(entry);
            inserter = std::move(entry);
        }
    } else {
        throw std::invalid_argument("Cannot read an array.");
    }
}

template <typename InputStream, typename Map>
std::enable_if_t<json::detail::IsMap<Map>::value> load(JsonInputArchive<InputStream>& archive,
                                                       Map& map)
{
    using T = typename Map::key_type;
    using V = typename Map::mapped_type;
    map.clear();
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
        V value;
        archive(value);
        map.insert({std::move(key), std::move(value)});
    }
}

template <typename InputStream, typename T>
void load(JsonInputArchive<InputStream>& archive, NameValuePair<T>& nameValuePair)
{
    archive.setNextKey(nameValuePair._name);
    archive(nameValuePair._value);
}

template <typename InputStream, typename... Ts>
void load(JsonInputArchive<InputStream>& archive, std::tuple<Ts...>& tuple)
{
    if (archive.currentValueIsArray()) {
        const std::size_t arraySize = archive.getArraySize();

        if (arraySize != sizeof...(Ts)) {
            throw exceptions::ParseException(
                    "Failed to load tuple. Persisted tuple size is " + std::to_string(arraySize) +
                    ". Expected tuple size is " + std::to_string(sizeof...(Ts)));
        }

        detail::loadTuple(archive, tuple, std::index_sequence_for<Ts...>{});
    } else {
        throw std::invalid_argument("Cannot read a Tuple.");
    }
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

template <typename T, typename InputStream>
std::unique_ptr<T> loadPointerDirectly(JsonInputArchive<InputStream>& archive)
{
    auto ptr = std::make_unique<T>();
    archive(SkipIntroOutroWrapper<T>(ptr.get()));
    return ptr;
}

template <typename Base, typename InputStream>
std::unique_ptr<Base> loadPolymorphicPointerThroughRegistry(JsonInputArchive<InputStream>& archive,
                                                            const std::string& typeName)
{
    using DecayedBase = std::decay_t<Base>;
    // lookup in type registry
    using TypeRegistry = muesli::TypeLoadRegistry<DecayedBase, JsonInputArchive<InputStream>>;
    using LoadFunction = typename TypeRegistry::LoadFunction;
    boost::optional<LoadFunction> loadFunction = TypeRegistry::getLoadFunction(typeName);
    if (loadFunction) {
        return (*loadFunction)(archive);
    } else {
        throw exceptions::UnknownTypeException(
                std::string("could not find input serializer for " +
                            boost::typeindex::type_id<DecayedBase>().pretty_name()));
    }
}

// generic de-serialization for non-polymorphic pointer types
template <typename T, typename InputStream>
std::enable_if_t<!std::is_polymorphic<T>::value, std::unique_ptr<T>> loadPointer(
        JsonInputArchive<InputStream>& archive)
{
    if (archive.currentValueIsNull()) {
        return nullptr;
    }
    return loadPointerDirectly<T>(archive);
}

template <typename InputStream>
std::string getTypeNameForPointer(JsonInputArchive<InputStream>& archive)
{
    archive.setNextKey("_typeName");
    std::string typeName;
    archive.readValue(typeName);
    return typeName;
}

// generic de-serialization for polymorphic, non-abstract pointer types
template <typename Base, typename InputStream>
std::enable_if_t<std::is_polymorphic<Base>::value && !std::is_abstract<Base>::value,
                 std::unique_ptr<Base>>
loadPointer(JsonInputArchive<InputStream>& archive)
{
    if (archive.currentValueIsNull()) {
        return nullptr;
    }
    static const std::string baseTypeName = RegisteredType<std::decay_t<Base>>::name();
    std::string typeName = getTypeNameForPointer(archive);
    if (baseTypeName == typeName) {
        return loadPointerDirectly<Base>(archive);
    } else {
        return loadPolymorphicPointerThroughRegistry<Base>(archive, typeName);
    }
}

// generic de-serialization for polymorphic abstract pointer types
template <typename Base, typename InputStream>
std::enable_if_t<std::is_polymorphic<Base>::value && std::is_abstract<Base>::value,
                 std::unique_ptr<Base>>
loadPointer(JsonInputArchive<InputStream>& archive)
{
    if (archive.currentValueIsNull()) {
        return nullptr;
    }
    return loadPolymorphicPointerThroughRegistry<Base>(archive, getTypeNameForPointer(archive));
}

} // namespace detail

template <typename InputStream, typename T>
void load(JsonInputArchive<InputStream>& archive, std::shared_ptr<T>& ptr)
{
    // forward to raw pointer implementation
    ptr = detail::loadPointer<T>(archive);
}

template <typename InputStream, typename T>
void load(JsonInputArchive<InputStream>& archive, std::unique_ptr<T>& ptr)
{
    // forward to raw pointer implementation
    ptr = detail::loadPointer<T>(archive);
}

template <typename InputStream, typename T>
void load(JsonInputArchive<InputStream>& archive, boost::optional<T>& opt)
{
    if (archive.currentValueIsNull()) {
        opt = boost::none;
    } else {
        T wrapped;
        archive(SkipIntroOutroWrapper<T>(&wrapped));
        opt = std::move(wrapped);
    }
}

} // namespace muesli

MUESLI_REGISTER_INPUT_ARCHIVE(muesli::JsonInputArchive, muesli::tags::json)

#endif // MUESLI_ARCHIVES_JSON_JSONINPUTARCHIVE_H_
