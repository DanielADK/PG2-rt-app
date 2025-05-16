#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp> 
#include <glm/ext.hpp>

#include "Vertex.hpp"
#include "ShaderProgram.hpp"
#include <iostream>

class Mesh {
public:
    // mesh data
    glm::vec3 origin{};
    glm::vec3 orientation{};

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    
    GLuint texture_id{0}; // texture id=0  means no texture
    GLenum primitive_type = GL_POINT;//GL_TRIANGLES;
    ShaderProgram &shader;
    
    // mesh material
    glm::vec4 ambient_material{1.0f}; //white, non-transparent 
    glm::vec4 diffuse_material{1.0f}; //white, non-transparent 
    glm::vec4 specular_material{1.0f}; //white, non-transparent
    float reflectivity{1.0f}; 
    
    // indirect (indexed) draw 
	Mesh(GLenum primitive_type, ShaderProgram & shader, std::vector<Vertex> const & vertices, std::vector<GLuint> const & indices, glm::vec3 const & origin, glm::vec3 const & orientation, GLuint const texture_id = 0):
        primitive_type(primitive_type),
        shader(shader),
        vertices(vertices),
        indices(indices),
        origin(origin),
        orientation(orientation),
        texture_id(texture_id) {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Bind and fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // Bind and fill index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        // Define vertex attributes 
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_position));  // Position

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));  // Normal

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_tex_coords));  // Texture Coordinates

        glBindVertexArray(0); // Unbind VAO
        
    };

    
    void draw(glm::vec3 const & offset, glm::vec3 const & rotation) {
 		if (VAO == 0) {//VAO=VERTEXT ARRAY OBJECT
			std::cerr << "VAO not initialized!\n";
			return;
		}
 
        shader.activate();
        // Bind texture if available
        if (texture_id > 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            //shader.setUniform("tex0", 0);
            glUniform1i(glGetUniformLocation(shader.ID, "tex0"), 0); // Set texture unit in fragment shader
        }
        
        //TODO: draw mesh: bind vertex array object, draw all elements with selected primitive type 
        glBindVertexArray(VAO);        
        
        glDrawElements(primitive_type, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        //glDrawArrays(primitive_type, 0, vertices.size()); 
        
        //glBindVertexArray(0);
    }
    void draw(glm::mat4 const& model_matrix) {
        if (VAO == 0) {
            std::cerr << "VAO not initialized!\n";
            return;
        }

        shader.activate();

        // Set model matrix uniform in the shader
        shader.setUniform("uM_m", model_matrix);
        shader.setUniform("matAmbient", glm::vec3(0.1f, 0.1f, 0.1f));
        shader.setUniform("matSpecular", glm::vec3(0.8f, 0.8f, 0.8f));
        shader.setUniform("matShininess", 32.0f);

        if (texture_id > 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            //shader.setUniform("tex0", 0);
            glUniform1i(glGetUniformLocation(shader.ID, "tex0"), 0); // Set texture unit in fragment shader
        }

        glBindVertexArray(VAO);
        glDrawElements(primitive_type, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

	void clear(void) {
        if (texture_id) {   // or all textures in vector...
            glDeleteTextures(1, &texture_id);
        }
        texture_id = 0;
        primitive_type = GL_POINT;
        vertices.clear();
        indices.clear();

        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
        
        
    };

private:
    // OpenGL buffer IDs
    // ID = 0 is reserved (i.e. uninitalized)
     unsigned int VAO{0}, VBO{0}, EBO{0};
};
  


