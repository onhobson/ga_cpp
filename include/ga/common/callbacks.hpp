#ifndef GA_COMMON_CALLBACKS_HPP
#define GA_COMMON_CALLBACKS_HPP

#include <functional>
#include <optional>

namespace ga::callback
{

/// A no-operation callback that does nothing when invoked.
struct NoOpCallback
{
    NoOpCallback() = default;
    NoOpCallback(const NoOpCallback&) = delete;

    template <typename Context>
    void operator()(const Context&) const noexcept
    { }
};

inline NoOpCallback defaultCallback;

}

#endif
