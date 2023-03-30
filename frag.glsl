#version 330

/*
vec3 I = vec3(2, 2, 2);

varying vec4 fragPos;
varying vec3 N;
*/
vec3 eyePos = vec3(0, 0, 2);
vec3 Iamb = vec3(0.8, 0.8, 0.8);

vec3 kd = vec3(0.8, 0.8, 0.8);
vec3 ka = vec3(0.3, 0.3, 0.3);
vec3 ks = vec3(0.8, 0.8, 0.8);

in vec3 lightPos[5];
in vec3 intensity[5];
in int lightCount;
in vec4 fragPos;
in vec4 N;
out vec4 FragColor;

void main(void)
{
	vec3 diffuseColor = vec3(0,0,0);
	vec3 specularColor  = vec3(0,0,0);
	for(int i=0; i< lightCount; i++){
		vec3 L = normalize(lightPos - vec3(fragPos));
		vec3 V = normalize(eyePos - vec3(fragPos));
		vec3 H = normalize(L + V);
		float NdotL = dot(N, L);
		float NdotH = dot(N, H);

		diffuseColor += I * kd * max(0, NdotL);
		specularColor += I * ks * pow(max(0, NdotH), 400);
	}


	vec3 ambientColor = Iamb * ka;

    FragColor = vec4(diffuseColor + ambientColor + specularColor, 1);
}
