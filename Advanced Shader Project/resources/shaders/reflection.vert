#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;	

out vec3 Position;
out vec3 Normal;
	
uniform mat4 M;	//Model matrix.
uniform mat4 V;	//View matrix.
uniform mat4 P;	//Projection matrix.
	
void main() {
	Position = vec3(M * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(M))) * aNormal;
	gl_Position = P * V * M * vec4(aPos, 1.0);
}