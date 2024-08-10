#include "tmig/utils/primitives_mesh.hpp"
#include "tmig/utils/primitives_gmesh.hpp"

namespace tmig {

namespace utils {

GMesh sphereGMesh() {
    static GMesh gmesh{sphereMesh()};
    return gmesh;
}

GMesh cylinderGMesh() {
    static GMesh gmesh{cylinderMesh()};
    return gmesh;
}

GMesh coneGMesh() {
    static GMesh gmesh{coneMesh()};
    return gmesh;
}

GMesh torusGMesh() {
    static GMesh gmesh{torusMesh()};
    return gmesh;
}

GMesh boxGMesh() {
    static GMesh gmesh{boxMesh};
    return gmesh;
}

GMesh wedgeGMesh() {
    static GMesh gmesh{wedgeMesh};
    return gmesh;
}

GMesh screenQuadGMesh() {
    static GMesh gmesh{screenQuadMesh};
    return gmesh;
}

} // namespace utils

} // namespace tmig