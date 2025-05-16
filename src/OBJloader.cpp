#include <string>
#include <GL/glew.h> 
#include <glm/glm.hpp>
#include <unordered_map>
#include <tuple>
#include "OBJloader.hpp"
#include <cstring>
#include <functional>

#define MAX_LINE_SIZE 255

// Custom hash function for a tuple of three unsigned ints
struct TupleHash {
    size_t operator()(const std::tuple<unsigned int, unsigned int, unsigned int>& t) const {
        auto [a, b, c] = t;
        return std::hash<unsigned int>()(a) ^
               (std::hash<unsigned int>()(b) << 1) ^
               (std::hash<unsigned int>()(c) << 2);
    }
};

bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals, std::vector<GLuint>& out_indices) {
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    // (position index, uv index, normal index)
    std::unordered_map<std::tuple<unsigned int, unsigned int, unsigned int>, GLuint, TupleHash> uniqueVertexMap;

    out_vertices.clear();
    out_uvs.clear();
    out_normals.clear();
    out_indices.clear();

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }

    while (1) {
        char lineHeader[MAX_LINE_SIZE];
        int res = fscanf(file, "%s", lineHeader, MAX_LINE_SIZE);
        if (res == EOF) {
            break;
        }

        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.y, &uv.x);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

            if (matches != 9) {
                printf("File can't be read by simple parser :( Try exporting with other options\n");
                fclose(file);
                return false;
            }

            for (int i = 0; i < 3; i++) {
                // Adjust indices (OBJ is 1-based, our arrays are 0-based)
                unsigned int vI = vertexIndex[i] - 1;
                unsigned int uvI = uvIndex[i] - 1;
                unsigned int nI = normalIndex[i] - 1;

                // Create a tuple of position, uv, and normal indices
                auto vertexKey = std::make_tuple(vI, uvI, nI);

                // Check if we've seen this vertex combination before
                auto it = uniqueVertexMap.find(vertexKey);
                if (it == uniqueVertexMap.end()) {
                    // Add this unique vertex to our output arrays
                    GLuint newIndex = static_cast<GLuint>(out_vertices.size());
                    uniqueVertexMap[vertexKey] = newIndex;

                    // Add the vertex data
                    out_vertices.push_back(temp_vertices[vI]);
                    out_uvs.push_back(temp_uvs[uvI]);
                    out_normals.push_back(temp_normals[nI]);

                    // Add the index
                    out_indices.push_back(newIndex);
                } else {
                    // Reuse the existing vertex
                    out_indices.push_back(it->second);
                }
            }
        }
    }

    fclose(file);
    return true;
}