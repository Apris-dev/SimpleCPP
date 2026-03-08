#pragma once
#include "Archive.h"

class CPathArchive : public CStringArchive {
protected:

    virtual size_t read(std::string& outValue) override {
        return CStringArchive::read(outValue);
    }

    virtual size_t write(const std::string& inValue) override {
        if (inValue.find('.') != std::string::npos) {
            return CStringArchive::write(inValue);
        }
        return CStringArchive::write(inValue + PATH_SEPARATOR);
    }

};