#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "tmig/util/resources.hpp"

int main() {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        tmig::util::getResourcePath("models/cube.obj"),
        aiProcess_Triangulate | aiProcess_FlipUVs
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
        return -1;
    }

    std::cout << "Successfully loaded model with " << scene->mNumMeshes << " meshes\n";
    for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
        const auto mesh = scene->mMeshes[i];
        std::cout << "Mesh[" << i << "].numVertices = " << mesh->mNumVertices << "\n";
        for (uint32_t v = 0; v < mesh->mNumVertices; ++v) {
            const auto vertex = mesh->mVertices[v];
            printf("vert at (%.3f, %.3f, %.3f)\n", vertex.x, vertex.y, vertex.z);
        }
    }

    return 0;
}