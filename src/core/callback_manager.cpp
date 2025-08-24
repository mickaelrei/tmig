#include <unordered_set>
#include <mutex>

#include "tmig/core/callback_manager.hpp"

namespace tmig::core {

std::unordered_set<WindowResizeListener*>& getListeners() {
    static std::unordered_set<WindowResizeListener*> listeners;
    return listeners;
}

std::mutex& getMutex() {
    static std::mutex mutex;
    return mutex;
}

void registerWindowResizeListener(WindowResizeListener* listener) {
    if (!listener) return;

    std::lock_guard<std::mutex> lock{getMutex()};
    getListeners().insert(listener);
}

void unregisterWindowResizeListener(WindowResizeListener* listener) {
    if (!listener) return;

    std::lock_guard<std::mutex> lock{getMutex()};
    getListeners().erase(listener);
}

void notifyWindowResize(int width, int height) {
    std::lock_guard<std::mutex> lock{getMutex()};

    for (auto& listener : getListeners()) {
        if (listener) {
            listener->onWindowResize(width, height);
        }
    }
}

} // namespace tmig::core
