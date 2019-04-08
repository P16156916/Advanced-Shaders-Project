#version 430
 
in vec3 texCoord;
 
out vec4 colour;
 
uniform samplerCube cubeMap;
 
 
void main() 
{
 colour = texture(cubeMap, texCoord);
}