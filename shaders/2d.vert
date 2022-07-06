#version 330 core

layout (location = 0) in vec3 iPos;

uniform mat4 world;
uniform mat4 projection;

void main(){
   gl_PointSize = 5.0;
   gl_Position = projection * world * vec4(iPos, 1.0);
}