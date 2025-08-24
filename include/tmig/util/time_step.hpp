#pragma once

#include <optional>

namespace tmig::util {

/// @brief A utility class to manage frame timing, delta time (dt), and smoothed FPS
///
/// This helper class should be updated once per frame. It calculates the time elapsed
/// since the last frame (dt) and provides a stable FPS reading at a configurable interval
class TimeStep {
public:
    /// @brief Constructor
    /// @param updateInterval The time in seconds between FPS calculations
    TimeStep(float updateInterval = 0.5f);

    /// @brief Updates the timer with the current time. Call this once at the start of your main loop
    /// @param currentTime The current runtime, generally obtained from `render::window::getRuntime()`
    /// @return Whether the update interval was reached during this call
    bool update(float currentTime);

    /// @brief Gets the delta time (time elapsed since the last frame) in seconds
    float dt() const { return _dt; }

    /// @brief Gets the latest calculated smoothed FPS value
    /// @note This value only changes when the update interval passes
    float fps() const { return _smoothedFps; }

private:
    float _updateInterval;
    float _timeAccumulator = 0.0f;
    int _frameCount = 0;
    float _lastTime = 0.0f;

    float _dt = 0.0f;
    float _smoothedFps = 0.0f;
};

} // namespace tmig::util