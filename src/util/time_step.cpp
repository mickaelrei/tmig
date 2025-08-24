#include "tmig/util/time_step.hpp"

namespace tmig::util {

TimeStep::TimeStep(float updateInterval) : _updateInterval{updateInterval} {}

bool TimeStep::update(float currentTime) {
    // On the first frame, initialize lastTime and set dt to a reasonable default
    if (_lastTime == 0.0f) {
        _lastTime = currentTime;

        // Assume 60 FPS for the first frame
        _dt = 1.0f / 60.0f;
        _smoothedFps = 60.0f;
        return true;
    }

    _dt = currentTime - _lastTime;
    _lastTime = currentTime;

    _frameCount++;
    _timeAccumulator += _dt;

    if (_timeAccumulator >= _updateInterval) {
        _smoothedFps = static_cast<float>(_frameCount) / _timeAccumulator;

        // Reset for the next interval
        _frameCount = 0;
        _timeAccumulator = 0.0f;
        return true;
    }

    return false;
}

} // namespace tmig::util