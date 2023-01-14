#version 330 core

layout (location = 0) in vec3 iPos;

uniform mat4 world;
uniform mat4 projection;

void main(){
   gl_PointSize = 10.0;
   gl_Position = projection * vec4(iPos, 1.0);
}