#version 330

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;

uniform mat4 modelingMat;
uniform mat4 transMat;
uniform mat4 cps;

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
	float y = ymin + sY * ((ymax-ymin))/(sampleSize-1);

	mat4 MB = mat4(
		-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 3, 0, 0,
		1, 0, 0, 0
	);

	mat4 MBT = transpose(MB);

	float s = (x-xmin)/(xmax-xmin);
	float t = (y-ymin)/(ymax-ymin);

	vec4 T = vec4(t*t*t, t*t, t, 1);
	vec4 S = vec4(s*s*s, s*s, s, 1);

	vec4 Q = MB * cps * MBT * T;
	float z = dot(S,Q);

	fragPos = modelingMat *  vec4(x,y,z,1.0);
	N = vec4(0,0,1,1);

	gl_Position = transMat * vec4(x,y,z,1.0);
}

