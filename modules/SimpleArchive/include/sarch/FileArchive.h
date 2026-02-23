#pragma once

#include <cassert>

#ifdef USING_SIMPLESTL
#include "sstl/Vector.h"
#else
#include <vector>
#endif

#include "Archive.h"

// An archive that can process files, uses standard c since it is faster
class CFileArchive : public CArchive {

public:

	// If archive goes out of scope, close the file
	virtual ~CFileArchive() override {
		close();
	}

	CFileArchive(const char* inFilePath, const char* inMode) {
		fopen_s(&mFile, inFilePath, inMode);
		if (mFile) mIsOpen = true;
	}

	CFileArchive(const std::string& inFilePath, const char* inMode)
		: CFileArchive(inFilePath.c_str(), inMode) {}

	[[nodiscard]] virtual bool isBinary() override {
		return true;
	}

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
	TVector<TType, TAlloc> readFile(const bool inRemoveBOM = false) {
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

	virtual void write(const void* inValue, const size_t inElementSize, const size_t inCount) override {
		assert(isOpen());
		fwrite(inValue, inElementSize, inCount, mFile);
	}

	virtual void read(void* inValue, size_t const inElementSize, const size_t inCount) override {
		assert(isOpen());
		fread(inValue, inElementSize, inCount, mFile);
	}

private:

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