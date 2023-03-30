#version 330

vec3 eyePos = vec3(0, 0, 0);

vec3 I = vec3(2, 2, 2);
vec3 Iamb = vec3(0.8, 0.8, 0.8);

vec3 kd = vec3(1, 0.2, 0.2);
//vec3 ka = vec3(0.1, 0.1, 0.1);
//vec3 ks = vec3(0.8, 0.8, 0.8);

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;

uniform mat4 modelingMat;
uniform mat4 modelingMatInvTr;
uniform mat4 ortographicMat;

uniform mat4 transMat;
uniform vec3 lightPosition[5];
uniform vec3 color[5];
uniform int lightSize;
uniform float coordMultiplier;


out vec4 fragPos;
out vec3 lightPos[5];
out vec3 intensity[5];
out int lightCount;
out vec4 N;

void main(void)
{

	/*vec3 ks = vec3(0.5,0.5,0.5);
	vec3 ka = vec3(color.x*0.6 , color.y*0.6, color.z*0.6);

	vec4 p = modelingMat * vec4(inVertex, 1);
	vec3 L = normalize(lightPos - vec3(p));
	vec3 V = normalize(eyePos - vec3(p));
	vec3 H = normalize(L + V);
	vec3 N = vec3(modelingMatInvTr * vec4(inNormal, 0)); 
	N = normalize(N);
	float NdotL = dot(N, L);
	float NdotH = dot(N, H);
	
	vec3 diffuseColor = I * color * max(0, NdotL);
	vec3 ambientColor = Iamb * ka;
	vec3 specularColor = I * ks * pow(max(0, NdotH), 7);

    vertexColor  =vec4(diffuseColor + ambientColor + specularColor, 1);
	*/

	for(int i=0; i< lightSize;i++){
		lightPos[i] = lightPosition[i];
		intensity[i] = color[i];
	}
	fragPos = vec4(0,0,0,1);
	N = vec4(0,0,0,1);

	gl_Position = transMat * vec4(inVertex, 1.0);
}

