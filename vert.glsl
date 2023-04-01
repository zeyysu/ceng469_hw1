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
uniform int sampleSize;
uniform float coordMultiplier;
uniform int surfaceindex;
uniform int bezierX;
uniform int bezierY;


out vec4 fragPos;
out vec4 N;
out vec3 testColor;

void main(void)
{
	float xmin = (-0.5)* coordMultiplier;
	float ymin = (-0.5)* coordMultiplier;

	float xmax, ymax;

	if(bezierX >= bezierY){
		xmax =  (0.5)* coordMultiplier;
		ymax = ymin + (bezierY*(xmax-xmin))/bezierX;	
	} else{
		ymax =  (0.5)* coordMultiplier;
		xmax =  xmin + (bezierX*(ymax-ymin))/bezierY;	
	}

	float bIndy = floor(surfaceindex/bezierX);
	float bIndx = surfaceindex - bezierX * bIndy;

	float xdist = xmax - xmin;
	float ydist = ymax - ymin;
	xmin = xmin + ((xdist)/(bezierX)) * bIndx;
	xmax = xmin + (xdist)/(bezierX);

	ymin = ymin + ((ydist)/bezierY) * bIndy;
	ymax = ymin + (ydist)/bezierY;


	float sY = floor(gl_VertexID/sampleSize);
	float sX = gl_VertexID - sampleSize * sY;

	float x = xmin + sX * ((xmax-xmin)/(sampleSize-1));
	float y = ymax - sY * ((ymax-ymin))/(sampleSize-1);
	
	fragPos = modelingMat *  vec4(x,y,0,1.0) ;
	N = vec4(0,0,1,1);

	gl_Position = transMat * vec4(x,y,0,1.0);
}

