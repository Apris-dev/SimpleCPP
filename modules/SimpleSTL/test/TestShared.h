#pragma once

#ifdef USING_SIMPLEARCHIVE
#include "sarch/Archive.h"
#endif


struct Abstract {
    Abstract() = default;
    Abstract(const size_t id): id(id) {}
    virtual ~Abstract() = default;
    virtual void print() const = 0;

    friend bool operator<(const Abstract& fst, const Abstract& snd) {
        return fst.id < snd.id;
    }

    friend bool operator==(const Abstract& fst, const Abstract& snd) {
        return fst.id == snd.id;
    }

#ifdef USING_SIMPLEARCHIVE
    friend COutputArchive& operator<<(COutputArchive& inArchive, const Abstract& obj) {
        inArchive << obj.id;
        return inArchive;
    }
#endif

    int id = 0;
};

struct Parent : Abstract{
    Parent() = default;
    Parent(const size_t id): Abstract(id) {}
    virtual ~Parent() override = default;
    virtual void print() const override {
        std::cout << "ID: " << id << std::endl;
    }

    friend bool operator<(const Parent& fst, const Parent& snd) {
        return fst.id < snd.id;
    }

    friend bool operator==(const Parent& fst, const Parent& snd) {
        return fst.id == snd.id;
    }

#ifdef USING_SIMPLEARCHIVE
    friend COutputArchive& operator<<(COutputArchive& inArchive, const Parent& obj) {
        inArchive << obj.id;
        return inArchive;
    }
#endif
};

struct SObject : Parent {

    SObject() = default;
    SObject(const size_t id, const std::string inName): Parent(id), name(inName) {}

    std::string name = "None";

    virtual void print() const override {
        std::cout << "ID: " << id << " Name: " << name << std::endl;
    }

    friend bool operator<(const SObject& fst, const SObject& snd) {
        return fst.id < snd.id;
    }

    friend bool operator==(const SObject& fst, const SObject& snd) {
        return fst.id == snd.id;
    }

#ifdef USING_SIMPLEARCHIVE
    friend COutputArchive& operator<<(COutputArchive& inArchive, const SObject& obj) {
        inArchive << obj.id;
        return inArchive;
    }
#endif
};