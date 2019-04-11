#version 410 core

out vec4 FragColour;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
	vec3 I = normalize(Position - cameraPos);
	vec3 R = reflect(I, normalize(Normal));
	
	FragColour = vec4(texture(skybox, R).rgb, 1.0f);
}