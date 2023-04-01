#version 330

vec3 eyePos = vec3(0, 0, 2);
vec3 Iamb = vec3(0.8, 0.8, 0.8);

vec3 kd = vec3(0.8, 0.8, 0.8);
vec3 ka = vec3(0.3, 0.3, 0.3);
vec3 ks = vec3(0.8, 0.8, 0.8);

uniform vec3 lightPosition[5];
uniform vec3 color[5];
uniform int lightSize;

in vec4 fragPos;
in vec4 N;
in vec3 testColor;
out vec4 FragColor;

void main(void)
{
	vec3 diffuseColor = vec3(0,0,0);
	vec3 specularColor  = vec3(0,0,0);

	vec3 normal = normalize(vec3(N));

	vec3 V = normalize(eyePos - vec3(fragPos));

	for(int i=0; i< lightSize; i++){
		vec3 L = normalize(lightPosition[i] - vec3(fragPos));
		vec3 H = normalize(L + V);
		float NdotL = dot(normal, L);
		float NdotH = dot(normal, H);

		float r2 = dot(lightPosition[i] - vec3(fragPos),lightPosition[i] - vec3(fragPos));
		vec3 Ir = (1/r2) * color[i];
		diffuseColor +=  Ir * max(0, NdotL);
		specularColor += Ir * ks * pow(max(0, NdotH), 400);
	}

	vec3 ambientColor = Iamb * ka;

    FragColor = vec4(diffuseColor + ambientColor + specularColor, 1);
}
