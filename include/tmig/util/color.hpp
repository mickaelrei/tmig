#pragma once

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>

namespace tmig::util {

/// @brief Receives HSV (hue, saturation, value) and returns it in RGB (red, green, blue)
inline glm::vec3 HSVtoRGB(float h, float s, float v) {
    float r = 0, g = 0, b = 0;

    if (s == 0.0f) {
        r = g = b = v;
    } else {
        h = std::fmod(h, 1.0f) * 6.0f;
        int i = static_cast<int>(std::floor(h));
        float f = h - i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));
        switch (i) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            case 5: default: r = v; g = p; b = q; break;
        }
    }

    return glm::vec3{r, g, b};
}

/// @brief Receives RGB (red, green, blue) and returns it in  HSV (hue, saturation, value)
inline glm::vec3 RGBtoHSV(float r, float g, float b) {
    float maxc = std::max({r, g, b});
    float minc = std::min({r, g, b});
    float v = maxc;
    float s = (maxc == 0.0f) ? 0.0f : (maxc - minc) / maxc;
    float h = 0.0f;

    if (s != 0.0f) {
        float delta = maxc - minc;
        if (maxc == r) {
            h = (g - b) / delta;
        } else if (maxc == g) {
            h = 2.0f + (b - r) / delta;
        } else {
            h = 4.0f + (r - g) / delta;
        }

        h /= 6.0f;
        if (h < 0.0f) {
            h += 1.0f;
        }
    }

    return glm::vec3{h, s, v};
}

} // namespace tmig::util