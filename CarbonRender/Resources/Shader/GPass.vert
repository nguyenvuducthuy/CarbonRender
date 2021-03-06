#version 430

layout(location = 0) in vec4 msPos;
layout(location = 1) in vec3 msN;
layout(location = 2) in vec3 msT;
layout(location = 3) in vec4 uvs;
layout(location = 4) in vec4 vColor;
layout(location = 5) in vec3 msB;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 proMat;
uniform vec2 depthClampPara;

out vec3 wsP;
out vec2 uv;
out mat3 TBN;
out float d;
out vec4 vertexColor;

void main ()
{
	gl_Position = viewMat * (modelMat * msPos);
	d = -gl_Position.z;
	gl_Position = proMat * gl_Position;
	vec3 T = normalize(msT);
	vec3 N = normalize(msN);
	vec3 B = normalize(msB);
	TBN = (mat3(T.x, T.y, T.z,
				B.x, B.y, B.z,
				N.x, N.y, N.z));
	
	wsP = (modelMat * msPos).xyz;
	uv = uvs.xy;
	vertexColor = vColor;
}