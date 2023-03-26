// Geometry shader
#version 420

layout(triangles) 			in;
layout(triangle_strip)		out;
layout(max_vertices = 3)	out;

// Coming in from the vertex shader
in vec4 gColour[];	
in vec4 gNormal[];
in vec4 gWorldLocation[];
in vec4 gUV2[];
in vec4 gTangent[];
in vec4 gBiNormal[];
in vec4 gBoneID[];
in vec4 gBoneWeight[];

// Going out to the fragment shader
out vec4 fColour;		
out vec4 fNormal;
out vec4 fWorldLocation;
out vec4 fUV2;
out vec4 fTangent;
out vec4 fBiNormal;
out vec4 fBoneID;
out vec4 fBoneWeight;

void main()
{
	gl_Position = gl_in[0].gl_Position;
	fColour = gColour[0];	
	fNormal = gNormal[0];
	fWorldLocation = gWorldLocation[0];
	fUV2 = gUV2[0];
	fTangent = gTangent[0];
	fBiNormal = gBiNormal[0];
	fBoneID = gBoneID[0];
	fBoneWeight = gBoneWeight[0];
	EmitVertex();
	
	gl_Position = gl_in[1].gl_Position;
	fColour = gColour[1];
	fNormal = gNormal[1];
	fWorldLocation = gWorldLocation[1];
	fUV2 = gUV2[1];
	fTangent = gTangent[1];
	fBiNormal = gBiNormal[1];
	fBoneID = gBoneID[1];
	fBoneWeight = gBoneWeight[1];
	EmitVertex();
	
	gl_Position = gl_in[2].gl_Position;
	fColour = gColour[2];
	fNormal = gNormal[2];
	fWorldLocation = gWorldLocation[2];
	fUV2 = gUV2[2];
	fTangent = gTangent[2];
	fBiNormal = gBiNormal[2];
	fBoneID = gBoneID[2];
	fBoneWeight = gBoneWeight[2];
	EmitVertex();
	
	EndPrimitive();
}