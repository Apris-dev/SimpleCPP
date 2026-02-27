#pragma once

#include <cassert>
#include <cstring>
#include <map>

#ifdef USING_SIMPLESTL
#include "sstl/Vector.h"
#else
#include <vector>
#endif

#include "Archive.h"

enum class EOpenType : uint8_t {
	READ = 1,
	WRITE = 2,
	BINARY = 4,
	READWRITE = READ | WRITE,
	BINARY_READ = READ | BINARY,
	BINARY_WRITE = WRITE | BINARY,
	BINARY_READWRITE = READ | WRITE | BINARY
};

constexpr bool operator&(const EOpenType& fst, const EOpenType& snd) {
	return (static_cast<uint8_t>(fst) & static_cast<uint8_t>(snd)) != 0;
}

// An archive that can process files, uses standard c since it is faster
template <EOpenType TOpenType>
class CFileArchive : public CArchive {

public:

	// If archive goes out of scope, close the file
	virtual ~CFileArchive() override {
		close();
	}

	CFileArchive(const char* inFilePath) {
		std::string mode;
		if constexpr (TOpenType & EOpenType::READ) { mode += "r"; }
		if constexpr (TOpenType & EOpenType::WRITE) { mode += "w"; }
		if constexpr (TOpenType & EOpenType::BINARY) { mode += "b"; }

#ifdef USING_MSVC
		fopen_s(&mFile, inFilePath, mode.c_str());
#else
		mFile = fopen(inFilePath, mode.c_str());
#endif
		if (mFile) mIsOpen = true;
	}

	CFileArchive(const std::string& inFilePath)
		: CFileArchive(inFilePath.c_str()) {}

	[[nodiscard]] virtual bool isBinary() override { return TOpenType & EOpenType::BINARY; }

	[[nodiscard]] virtual bool isRead() { return TOpenType & EOpenType::READ; }

	[[nodiscard]] virtual bool isWrite() { return TOpenType & EOpenType::WRITE; }

	[[nodiscard]] virtual bool isReadWrite() { return isRead() && isWrite(); }

	[[nodiscard]] virtual bool isReadOnly() { return isRead() && !isWrite(); }

	[[nodiscard]] virtual bool isWriteOnly() { return isWrite() && !isRead(); }

	[[nodiscard]] bool isOpen() const { return mIsOpen; }

	[[nodiscard]] bool isEnd() const { return feof(mFile); }

	[[nodiscard]] std::string readLine(const bool inRemoveBOM = true) const {
		assert(isOpen());

		std::string line;
		char buffer[256];

		while (fgets(buffer, sizeof(buffer), mFile)) {
			line += buffer;

			// Stop if we read a full line
			if (line.back() == '\n')
				break;
		}

		if (inRemoveBOM) removeBOM(line.data(), line.size());

		return line;
	}

	// Char has size of 1
	void writeLine(const std::string& string) const {
		assert(isOpen());
		fwrite(string.data(), 1, string.size(), mFile);
		constexpr char newLine = '\n'; //TODO: platform newline
		fwrite(&newLine, 1, 1, mFile);
	}

	// Function to read from entire file with any type
	template <typename TType, class TAlloc = std::allocator<TType>>
#ifdef USING_SIMPLESTL
	TVector<TType> readFile(const bool inRemoveBOM = false) {
#else
	std::vector<TType, TAlloc> readFile(const bool inRemoveBOM = false) {
#endif
		assert(isOpen());

		fseek(mFile, 0L, SEEK_END);
		const auto fileSize = ftell(mFile);
		fseek(mFile, 0L, SEEK_SET);

		std::vector<TType, TAlloc> vector(fileSize / sizeof(TType));
		const size_t bytesRead = fread(vector.data(), sizeof(TType), vector.size(), mFile);

		if (inRemoveBOM) removeBOM(vector.data(), bytesRead);

		// Since every part of the file is read, we might as well close it
		close();

		return vector;
	}

	// Read into char vector, then reinterpret to a string
	std::string readFile(const bool inRemoveBOM = false) {
		std::vector<char> vector = readFile<char>(inRemoveBOM);
		return {vector.data(), vector.size()};
	}

#ifdef USING_SIMPLESTL
	// Function to write to entire file with any type
	template <typename TType>
	void writeFile(TVector<TType> vector) {
		assert(isOpen());
		fwrite(vector.data(), sizeof(TType), vector.getSize(), mFile);

		// Since every part of the file is read, we might as well close it
		close();
	}
#else
	// Function to write to entire file with any type
	template <typename TType, class TAlloc = std::allocator<TType>>
	void writeFile(std::vector<TType, TAlloc> vector) {
		assert(isOpen());
		fwrite(vector.data(), sizeof(TType), vector.size(), mFile);

		// Since every part of the file is read, we might as well close it
		close();
	}
#endif

	void close() {
		if (isOpen()) {
			fclose(mFile);
			mIsOpen = false;
		}
	}

protected:

	virtual size_t write(const void* inValue, const size_t inElementSize, const size_t inCount) override {
		assert(isOpen());
		return fwrite(inValue, inElementSize, inCount, mFile);
	}

	virtual size_t read(void* inValue, size_t const inElementSize, const size_t inCount) override {
		assert(isOpen());
		return fread(inValue, inElementSize, inCount, mFile);
	}

public:

	// The BOM might cause issues with certain interpreters
	static void removeBOM(void* inData, const size_t inSize) {
		constexpr static unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };
		if (inSize > 3)
			if (!memcmp(inData, BOM, 3))
				memset(inData, ' ', 3);
	}

	FILE* mFile = nullptr;
	bool mIsOpen = false;

};