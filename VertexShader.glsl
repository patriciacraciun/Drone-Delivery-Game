#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform int isTerrain;
uniform int isChristmas;
uniform vec3 objectColor;

out float vertexNoise;

float random2D(vec2 st){
	return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float noise2D(vec2 st){
	vec2 i = floor(st);
	vec2 f = fract(st);

	float a = random2D(i);
	float b = random2D(i + vec2(1.0, 0.0));
	float c = random2D(i + vec2(0.0, 1.0));
	float d = random2D(i + vec2(1.0, 1.0));

	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main(){
	vec3 pos = inPosition;
	if (isTerrain == 1){
		float freq = 0.1;
		float n = noise2D(pos.xz * freq);
		float amplitude = 2.0;
		if(isChristmas == 1){
			amplitude = 3.0;
		}
		pos.y = n * amplitude;
		vertexNoise = n;
	}
	else {
		vertexNoise = 0.0;
	}
	gl_Position = Projection * View * Model * vec4(pos, 1.0);
}