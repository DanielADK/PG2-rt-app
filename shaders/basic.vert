#version 460 core

// input vertex attributes

layout(location = 0) in vec3 aPos;   // position: MUST exist
//in vec3 aColor; // any additional attributes are optional, any data type, etc.
layout(location = 1) in vec3 aNormal; //attribute normal
layout(location = 2) in vec2 aTexCoords; //attribute_texCoords

uniform mat4 uM_m = mat4(1.0);//uniform mat4 model;
uniform mat4 uV_m = mat4(1.0);//uniform mat4 view;
uniform mat4 uP_m = mat4(1.0);//uniform mat4 projection;



out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} vs_out;

uniform float tex_scale = 1.0f; // scale factor for texture coordinates

void main() {

    vec4 worldPos = uM_m * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz;
    vs_out.Normal = mat3(transpose(inverse(uM_m))) * aNormal;
    vs_out.TexCoord = aTexCoords * tex_scale; // násobení opakování textury;

    gl_Position = uP_m * uV_m * worldPos;
}
