#pragma once

namespace tmig {

namespace render {

namespace gl {

/// @brief Interface for OpenGL objects such as vertex arrays and texture
class GLObject {
public:
    /// @brief Destructor
    virtual ~GLObject() = default;

    /// @brief Get this object's id
    /// @return Object id
    unsigned int id() const { return _id; };

    /// @brief Destroy object
    virtual void destroy() = 0;

protected:
    /// @brief Deleter struct
    struct Deleter {
        void operator()(GLObject *glObject) {
            glObject->destroy();
            delete glObject;
        }
    };

    /// @brief OpenGL object identifier
    unsigned int _id = 0;
};

} // namespace gl

} // namespace render

} // namespace tmig