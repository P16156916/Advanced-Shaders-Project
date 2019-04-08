#version 430
 
layout (location = 0) in vec3 position;
out vec3 texCoord;
 
uniform mat4 VM;
uniform mat4 P;
 
 
void main() {
 
gl_Position = P * VM * vec4(position, 1.0);
 
texCoord = position;
 
}