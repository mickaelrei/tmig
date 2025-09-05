#pragma once

namespace tmig::core {

/// @brief Base class that defines a non-copyable resource, as the copy constructor and copy assignment operator are marked as deleted.
///
/// @note This class is intended to be inherited using `protected` or `private` inheritance to prevent copying of derived classes.
/// Do not instantiate or use this class directly.
class NonCopyable {
protected:
    /// @brief Constructor
    constexpr NonCopyable() = default;

    /// @brief Destructor
    ~NonCopyable() = default;

    /// @brief Disabled copy constructor
    NonCopyable(const NonCopyable&) = delete;

    /// @brief Disabled copy assignment operator
    NonCopyable& operator=(const NonCopyable&) = delete;
};

} // namespace tmig::core