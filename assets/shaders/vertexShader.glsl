// Vertex shader
#version 420

// Coming in from VAO
in vec4 vPosition;
in vec4 vColour;
in vec4 vNormal;
in vec4 vUV2;
in vec4 vTangent;
in vec4 vBiNormal;
in vec4 vBoneID;
in vec4 vBoneWeight;

// Out to the geometry shader
out vec4 gColour;
out vec4 gNormal;
out vec4 gWorldLocation;
out vec4 gUV2;
out vec4 gTangent;
out vec4 gBiNormal;
out vec4 gBoneID;
out vec4 gBoneWeight;

// Uniforms
uniform mat4 Model;
uniform mat4 ModelInverse;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
	vec3 vertPosition = vPosition.xyz;

	mat4 MVP = Projection * View * Model;
	
	gl_Position = MVP * vec4(vertPosition, 1.f);

	gWorldLocation.xyz = (Model * vec4(vPosition)).xyz;
	gWorldLocation.w = 1.f;

	gNormal.xyz = normalize(ModelInverse * vec4(vNormal.xyz, 1.f)).xyz;
	gNormal.w = 1.f;

	gColour = vColour;
	gUV2 = vUV2;
	gTangent = vTangent;
	gBiNormal = vBiNormal;
	gBoneID = vBoneID;
	gBoneWeight = vBoneWeight;
}
