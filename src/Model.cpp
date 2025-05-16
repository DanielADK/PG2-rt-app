//
// Created by Daniel Adámek - daniel@nullptr.cz on 4.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include "Model.hpp"

Model::Model(const std::filesystem::path &filename, ShaderProgram &shader) {
    std::string outfilename_str = filename.string();
    const char* outfilename_ptr = outfilename_str.c_str();

    std::vector < glm::vec3 > out_vertices;
    std::vector < glm::vec2 > out_uvs;
    std::vector < glm::vec3 > out_normals;
    std::vector < GLuint > out_indices;
    loadOBJ(outfilename_ptr, out_vertices, out_uvs, out_normals, out_indices);

    std::vector<Vertex> meshVertices;

    for (size_t i = 0; i < out_vertices.size(); i++) {
        Vertex vertex{};
        vertex.m_position = out_vertices[i];
        vertex.m_normal = out_normals[i];
        vertex.m_tex_coords = out_uvs[i];
        meshVertices.push_back(vertex);
    }

    meshes.emplace_back(std::make_shared<Mesh>(
        GL_TRIANGLES,
        shader,
        meshVertices,
        out_indices,
        glm::vec3(0.0f),
        glm::vec3(0.0f)
    ));
}

Model::Model(const std::filesystem::path& filename, ShaderProgram& shader, const std::filesystem::path& texture_file_path) {//WITH TEXTURE
    std::string outfilename_str = filename.string();
    const char* outfilename_ptr = outfilename_str.c_str();

    std::vector < glm::vec3 > out_vertices;
    std::vector < glm::vec2 > out_uvs;
    std::vector < glm::vec3 > out_normals;
    std::vector < GLuint > out_indices;
    loadOBJ(outfilename_ptr, out_vertices, out_uvs, out_normals, out_indices);

    std::vector<Vertex> meshVertices;

    for (size_t i = 0; i < out_vertices.size(); i++) {
        Vertex vertex{};
        vertex.m_position = out_vertices[i];
        vertex.m_normal = out_normals[i];
        vertex.m_tex_coords = out_uvs[i];
        meshVertices.push_back(vertex);
    }

    GLuint texture_id = textureInit(texture_file_path.string().c_str());

    meshes.emplace_back(std::make_shared<Mesh>(
        GL_TRIANGLES,
        shader,
        meshVertices,
        out_indices,
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        texture_id
    ));
}

void Model::draw(glm::vec3 const &offset, glm::vec3 const &rotation, glm::vec3 const &scale_change) {
    // compute complete transformation
    glm::mat4 t = glm::translate(glm::mat4(1.0f), m_origin);
    glm::mat4 rx = glm::rotate(glm::mat4(1.0f), orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 ry = glm::rotate(glm::mat4(1.0f), orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rz = glm::rotate(glm::mat4(1.0f), orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

    glm::mat4 m_off = glm::translate(glm::mat4(1.0f), offset);
    glm::mat4 m_rx = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 m_ry = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 m_rz = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 m_s = glm::scale(glm::mat4(1.0f), scale_change);

    glm::mat4 model_matrix = local_model_matrix * s * rz * ry * rx * t * m_s * m_rz * m_ry * m_rx * m_off;

    // draw all meshes
    for (auto mesh : meshes) {
        mesh->draw(model_matrix);
    }
}

void Model::draw(glm::mat4 const &model_matrix) {
    for (const auto mesh : meshes) {
        mesh->draw(local_model_matrix * model_matrix);
    }
}
