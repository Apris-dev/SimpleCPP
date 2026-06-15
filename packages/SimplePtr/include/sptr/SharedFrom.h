#pragma once

#include "sptr/Shared.h"

template <typename TType>
struct TSharedFrom {
    using _Esft_type = TSharedFrom;

    [[nodiscard]] TShared<TType> getShared() {
        return TShared<TType>(_Wptr);
    }

    [[nodiscard]] TShared<const TType> getShared() const {
        return TShared<const TType>(_Wptr);
    }

    [[nodiscard]] TWeak<TType> getWeak() noexcept {
        return _Wptr;
    }

    [[nodiscard]] TWeak<const TType> getWeak() const noexcept {
        return _Wptr;
    }

protected:
    constexpr TSharedFrom() noexcept : _Wptr() {}

    TSharedFrom(const TSharedFrom&) noexcept : _Wptr() {}

    TSharedFrom& operator=(const TSharedFrom&) noexcept {
        return *this;
    }

    ~TSharedFrom() = default;

private:
    template <typename>
    friend struct TShared;

    template <typename>
    friend struct std::shared_ptr;

    // Due to how this works internally, it has to be a std::weak_ptr, and called _Wptr
    mutable std::weak_ptr<TType> _Wptr;
};