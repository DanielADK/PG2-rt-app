#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include <GL/glew.h> 

class ShaderProgram {
public:
	// you can add more constructors for pipeline with GS, TS etc.
	ShaderProgram() = default; //does nothing
	ShaderProgram(const std::filesystem::path & VS_file, const std::filesystem::path & FS_file);
	void activate() { glUseProgram(ID); };    // activate shader
	void deactivate() { glUseProgram(0); };   // deactivate current shader program (i.e. activate shader no. 0)

	void clear(void) { 	//deallocate shader program - dont put in destructor
		deactivate();
		glDeleteProgram(ID);
		ID = 0;
	}
    
    // set uniform according to name 
    // https://docs.gl/gl4/glUniform
    void setUniform(const std::string & name, const float val);      
    void setUniform(const std::string & name, const double val);
	void setUniform(const std::string & name, const int val);
    void setUniform(const std::string & name, const glm::vec3 val);  
    void setUniform(const std::string & name, const glm::vec4 val);
    void setUniform(const std::string & name, const glm::mat3 val);   
    void setUniform(const std::string & name, const glm::mat4 val);
    
	GLuint ID{ 0 }; // default = 0, empty shader
private:
	
	std::string getShaderInfoLog(const GLuint obj);
	std::string getProgramInfoLog(const GLuint obj);

	GLuint compile_shader(const std::filesystem::path & source_file, const GLenum type);
	GLuint link_shader(const std::vector<GLuint> shader_ids);
    std::string textFileRead(const std::filesystem::path & filename);
};

