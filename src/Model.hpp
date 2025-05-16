//
// Created by Daniel Adámek - daniel@nullptr.cz on 2.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#ifndef MODEL_HPP
#define MODEL_HPP

#include <filesystem>
#include <string>
#include <vector> 
#include <glm/glm.hpp> 


#include "Vertex.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "OBJloader.hpp"
#include "Texture.hpp"

class Model {
public:
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::string name;

    // original position
    glm::vec3 m_origin{0.0};
    glm::vec3 orientation{0.0}; //rotation by x,y,z axis, in radians 
    glm::vec3 scale{1.0};
    glm::mat4 local_model_matrix{1.0};

    GLuint tex_ID = 0;  // Texture ID for model

    bool transparent = false ;

	Model() = default;

    //NO TEXTURE
    Model(const std::filesystem::path& filename, ShaderProgram& shader);

    //WITH TEXTURE
    Model(const std::filesystem::path& filename, ShaderProgram& shader, const std::filesystem::path& texture_file_path);

    void draw(glm::vec3 const& offset = glm::vec3(0.0), 
              glm::vec3 const& rotation = glm::vec3(0.0f),
              glm::vec3 const& scale_change = glm::vec3(1.0f));

    void draw(glm::mat4 const& model_matrix);


    


private:
    void loadOBJFile(const std::filesystem::path& filename, ShaderProgram& shader);
};

#endif //MODEL_HPP