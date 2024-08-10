#pragma once

#include "tmig/gmesh.hpp"

namespace tmig {

namespace utils {

/// @brief Get the default sphere gmesh
/// @return The sphere gmesh
GMesh sphereGMesh();

/// @brief Get the default cylinder gmesh
/// @return The cylinder gmesh
GMesh cylinderGMesh();

/// @brief Get the default cone gmesh
/// @return The cone gmesh
GMesh coneGMesh();

/// @brief Get the default torus gmesh
/// @return The torus gmesh
GMesh torusGMesh();

/// @brief Get the default box gmesh
/// @return The box gmesh
GMesh boxGMesh();

/// @brief Get the default wedge gmesh
/// @return The wedge gmesh
GMesh wedgeGMesh();

/// @brief Get the default screen quad mesh
/// @return The screen quad mesh
GMesh screenQuadGMesh();

} // namespace utils

} // namespace tmig