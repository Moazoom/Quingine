#version 330 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iTexCoords;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main(){
   gl_Position = projection * view * world * vec4(iPosition, 1.0);
   Normal = mat3(transpose(inverse(world))) * iNormal; // for optimisation, get this on the cpu and pass in as uniform
   FragPos = vec3(world * vec4(iPosition, 1.0));
   TexCoords = iTexCoords;
}