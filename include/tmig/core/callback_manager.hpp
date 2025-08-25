#pragma once

namespace tmig::core {

/// @brief Window resize listener interface. Classes that might need to listen to window
//         resize events inherit from this and implement their own `onWindowResize` method
class WindowResizeListener {
public:
    virtual ~WindowResizeListener() = default;
    virtual void onWindowResize(int width, int height) = 0;
};

/// @brief Register a new window resize listener
void registerWindowResizeListener(WindowResizeListener* listener);

/// @brief Unregister a window resize listener
void unregisterWindowResizeListener(WindowResizeListener* listener);

/// @brief Notify all registered listeners
/// @note This is automatically called internally when the window gets resized. Should not be called manually
void onWindowResize(int width, int height);

} // namespace tmig::core
