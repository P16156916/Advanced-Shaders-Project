#version 430 core

out vec4 FragColour;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform float ratio=0.65;

void main() {
	vec3 I = normalize(Position - cameraPos);
	vec3 R = refract(I, normalize(Normal), ratio);
	
	FragColour = vec4(texture(skybox, R).rgb, 1.0f);
}