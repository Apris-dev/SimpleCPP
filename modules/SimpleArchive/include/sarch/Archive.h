#pragma once

#include <cassert>
#include <charconv>
#include <sstream>
#include <string>

#include "sutil/InitializerList.h"
#include "sutil/Pair.h"
#include "sutil/PlatformDefinition.h"

class CStringArchive {

protected:

	virtual size_t write(const std::string& inValue) {
		str += inValue;
		return 0;
	}

	virtual size_t read(std::string& outValue) {
		const auto loc = str.find(LINE_ENDING);
		outValue = str.substr(0, loc);
		str = str.substr(1, loc);
		return 0;
	}

public:

	virtual ~CStringArchive() = default;

	[[nodiscard]] virtual const std::string& get() const { return str; }

	friend CStringArchive& operator>>(CStringArchive& inArchive, std::string& inValue) {
		inArchive.read(inValue);
		return inArchive;
	}

	friend CStringArchive& operator<<(CStringArchive& inArchive, const std::string& inValue) {
		inArchive.write(inValue);
		return inArchive;
	}

	// TODO: temporarily remove this until a better solution arises
	/*template <typename TType,
		std::enable_if_t<std::is_arithmetic_v<TType>, int> = 0
	>
	friend CStringArchive& operator>>(CStringArchive& inArchive, TType& inValue) {
		std::string string;
		inArchive >> string;
		inValue = std::stoull(string);
		return inArchive;
	}*/

	template <typename TType,
		std::enable_if_t<std::is_arithmetic_v<TType>, int> = 0
	>
	friend CStringArchive& operator<<(CStringArchive& inArchive, const TType& inValue) {
		inArchive << std::to_string(inValue);
		return inArchive;
	}

	/*template <typename TType,
		std::enable_if_t<std::is_enum_v<TType>, int> = 0
	>
	friend CStringArchive& operator>>(CStringArchive& inArchive, TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		EnumType value;
		inArchive >> value;
		inEnum = static_cast<TType>(value);
		return inArchive;
	}*/

	template <typename TType,
		std::enable_if_t<std::is_enum_v<TType>, int> = 0
	>
	friend CStringArchive& operator<<(CStringArchive& inArchive, const TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		inArchive << static_cast<EnumType>(inEnum);
		return inArchive;
	}

	template <typename TType>
	friend CStringArchive& operator>>(CStringArchive& inArchive, TType*& ptr) {
		size_t value;
		inArchive >> value;
		ptr = reinterpret_cast<TType*>(value);
		return inArchive;
	}

	template <typename TType>
	friend CStringArchive& operator<<(CStringArchive& inArchive, const TType*& ptr) {
		inArchive << reinterpret_cast<size_t>(ptr);
		return inArchive;
	}

	template <typename TKeyType, typename TValueType>
	friend CStringArchive& operator<<(CStringArchive& inArchive, const TPair<TKeyType, TValueType>& pair) {
		inArchive << pair.first;
		inArchive << pair.second;
		return inArchive;
	}

	// Initializer lists cannot be written to, but can be read from
	template <typename TType>
	friend CStringArchive& operator<<(CStringArchive& inArchive, const TInitializerList<TType>& list) {
		for (const auto& obj : list)
			inArchive << obj;
		return inArchive;
	}

protected:

	std::string str;
};

class CInputArchive {

protected:

	virtual size_t read(void* inValue, const size_t inElementSize) {
		return read(inValue, inElementSize, 1);
	}

	virtual size_t read(void* inValue, size_t inElementSize, size_t inCount) = 0;

	std::string readUntil(const char terminator) {
		std::string res;
		char c;
		while (true) {
			const size_t n = read(&c, sizeof(char));
			if (n == 0 || c == terminator)
				break;
			res += c;
		}
		return res;
	}

public:

	virtual ~CInputArchive() = default;

	template <typename TType,
		std::enable_if_t<std::is_arithmetic_v<TType>, int> = 0
	>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TType& inValue) {
		inArchive.read(&inValue, sizeof(TType));
		return inArchive;
	}

	template <typename TType,
		std::enable_if_t<std::is_enum_v<TType>, int> = 0
	>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		EnumType value;
		inArchive >> value;
		inEnum = static_cast<TType>(value);
		return inArchive;
	}

	template <typename TType>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TType*& ptr) {
		size_t value;
		inArchive >> value;
		ptr = reinterpret_cast<TType*>(value);
		return inArchive;
	}

	friend CInputArchive& operator>>(CInputArchive& inArchive, std::string& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size);
		inArchive.read(inValue.data(), sizeof(std::string::value_type), size);
		return inArchive;
	}

	template <typename TKeyType, typename TValueType>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TPair<TKeyType, TValueType>& pair) {
		inArchive >> pair.first;
		inArchive >> pair.second;
		return inArchive;
	}
};

class COutputArchive {

protected:

	virtual size_t write(const void* inValue, const size_t inElementSize) {
		return write(inValue, inElementSize, 1);
	}

	virtual size_t write(const void* inValue, size_t inElementSize, size_t inCount) = 0;

public:

	virtual ~COutputArchive() = default;
	
	template <typename TType,
		std::enable_if_t<std::is_arithmetic_v<TType>, int> = 0
	>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TType& inValue) {
		inArchive.write(&inValue, sizeof(TType));
		return inArchive;
	}

	template <typename TType,
		std::enable_if_t<std::is_enum_v<TType>, int> = 0
	>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		inArchive << static_cast<EnumType>(inEnum);
		return inArchive;
	}

	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TType*& ptr) {
		inArchive << reinterpret_cast<size_t>(ptr);
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const std::string& inValue) {
		inArchive << inValue.size();
		inArchive.write(inValue.data(), sizeof(std::string::value_type), inValue.size());
		return inArchive;
	}

	template <typename TKeyType, typename TValueType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TPair<TKeyType, TValueType>& pair) {
		inArchive << pair.first;
		inArchive << pair.second;
		return inArchive;
	}

	// Initializer lists cannot be written to, but can be read from
	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TInitializerList<TType>& list) {
		inArchive << list.size();
		for (const auto& obj : list)
			inArchive << obj;
		return inArchive;
	}
};

class CArchive : public CInputArchive, public COutputArchive {};