#version 100

attribute vec3 inPosition;

uniform mat4 matRotate;
uniform mat4 matWorld;
uniform mat4 matWVP;

varying vec3 TexCoords;

float ComputeFog(vec3 camToWorldPos, vec3 param)
{
	float fdist = max(length(camToWorldPos) - param.x, 0.0);
	float density = clamp(clamp(fdist/(param.y-param.x), 0.0, 1.0) * param.z, 0.0, 1.0);
	return 1.0 - density;
}

void main(void)
{
	TexCoords = inPosition;
	vec4 rotatePos = matRotate * vec4(inPosition, 1.0);
	vec4 worldPos = matWorld * rotatePos;
	gl_Position = matWVP * worldPos;
}


