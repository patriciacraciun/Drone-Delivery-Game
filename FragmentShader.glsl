#version 330 core

in float vertexNoise;

out vec4 fragColor;

uniform int isTerrain;
uniform int isChristmas;
uniform vec3 objectColor;

void main(){
	if(isTerrain == 1){
		if(isChristmas == 1){
			vec3 snowcolor = vec3(0.7, 0.7, 1.0);
			vec3 snowcolor2 = vec3(1.0, 1.0, 1.0);
			vec3 finalColor = mix(snowcolor, snowcolor2, vertexNoise);
			fragColor = vec4(finalColor, 1.0);
		} else{
			vec3 color1 = vec3(0.1, 0.5, 0.1);
			vec3 color2 = vec3(0.3, 0.2, 0.1);
			vec3 finalColor = mix(color2, color1, vertexNoise);
			fragColor = vec4(finalColor, 1.0);
		}
	} else {
		fragColor = vec4(objectColor, 1.0);
	}
}