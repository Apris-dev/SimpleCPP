#pragma once

#include <cassert>
#include <charconv>
#include <string>

#include "sutil/InitializerList.h"
#include "sutil/Pair.h"

#ifdef USING_SIMPLEPTR
#include "sptr/Memory.h"
#endif

class CInputArchive {

protected:

	virtual size_t read(void* inValue, size_t inElementSize, size_t inCount) = 0;

	std::string readUntil(const char terminator) {
		std::string res;
		char c;
		while (true) {
			const size_t n = read(&c, sizeof(char), 1);
			if (n == 0 || c == terminator)
				break;
			res += c;
		}
		return res;
	}

	 [[nodiscard]] virtual bool isBinary() = 0;

public:

	virtual ~CInputArchive() = default;

	template <typename TType,
		std::enable_if_t<std::is_arithmetic_v<TType>, int> = 0
	>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TType& inValue) {
		// Either serialize numbers as binary or ASCII with null terminator
		if (inArchive.isBinary()) {
			inArchive.read(&inValue, sizeof(TType), 1);
		} else {
			const std::string res = inArchive.readUntil('\0');
			const std::from_chars_result numRes = std::from_chars(res.data(), res.data() + res.size(), inValue);
			assert(numRes.ec == std::errc() && numRes.ptr == (res.data() + res.size()));
		}
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
		// If binary, we serialize the string based on its size, if not, we check for newline
		if (inArchive.isBinary()) {
			size_t size;
			inArchive >> size;
			inValue.resize(size);

			inArchive.read(inValue.data(), 1, inValue.size());
		} else {
			inValue = inArchive.readUntil('\n');
		}

		return inArchive;
	}

#ifdef USING_SIMPLEPTR
	template <typename TType,
		std::enable_if_t<std::is_default_constructible_v<TType>, int> = 0
	>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TShared<TType>& inValue) {
		inArchive >> *inValue.get();
		return inArchive;
	}

	template <typename TType,
		std::enable_if_t<std::is_default_constructible_v<TType>, int> = 0
	>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TUnique<TType>& inValue) {
		inArchive >> *inValue.get();
		return inArchive;
	}
#endif

	template <typename TKeyType, typename TValueType>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TPair<TKeyType, TValueType>& pair) {
		inArchive >> pair.first;
		inArchive >> pair.second;
		return inArchive;
	}
};

class COutputArchive {

protected:

	virtual size_t write(const void* inValue, size_t inElementSize, size_t inCount) = 0;

	virtual bool isBinary() = 0;

public:

	virtual ~COutputArchive() = default;
	
	template <typename TType,
		std::enable_if_t<std::is_arithmetic_v<TType>, int> = 0
	>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TType& inValue) {
		// Either serialize numbers as binary or ASCII with null terminator
		if (inArchive.isBinary()) {
			inArchive.write(&inValue, sizeof(TType), 1);
		} else {
			const auto str = std::to_string(inValue);
			inArchive.write(str.data(), 1, str.size());
			constexpr char nullString = '\0';
			inArchive.write(&nullString, sizeof(nullString), 1);
		}
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
		// If binary, we serialize the string based on its size, if not, we save with newline
		if (inArchive.isBinary()) {
			inArchive << inValue.size();
			inArchive.write(inValue.data(), 1, inValue.size());
		} else {
			inArchive.write(inValue.data(), 1, inValue.size());
			constexpr char newLine = '\n';
			inArchive.write(&newLine, sizeof(newLine), 1);
		}
		return inArchive;
	}

#ifdef USING_SIMPLEPTR
	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TShared<TType>& inValue) {
		inArchive << *inValue.get();
		return inArchive;
	}

	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TUnique<TType>& inValue) {
		inArchive << *inValue.get();
		return inArchive;
	}
#endif

	template <typename TKeyType, typename TValueType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TPair<TKeyType, TValueType>& pair) {
		inArchive << pair.first;
		inArchive << pair.second;
		return inArchive;
	}

	// Initializer lists cannot be written to, but can be read from
	template <typename TType>
	friend COutputArchive& getHash(COutputArchive& inArchive, const TInitializerList<TType>& list) {
		inArchive << list.size();
		for (const auto& obj : list)
			inArchive << obj;
		return inArchive;
	}
};

class CArchive : public virtual CInputArchive, public virtual COutputArchive {};