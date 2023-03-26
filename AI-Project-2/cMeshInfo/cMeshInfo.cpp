#include "cMeshInfo.h"

cMeshInfo::cMeshInfo() {

	this->position = glm::vec3(0.f);
	this->velocity = glm::vec3(0.f);
	this->up = glm::vec3(0.f, 1.f, 0.f);
	this->facingDirection = glm::vec3(0.f);
	this->rotation = glm::quat(glm::vec3(0.f));
	this->colour = glm::vec4(0.f, 0.f, 0.f, 1.f);
	this->RGBAColour = glm::vec4(0.f, 0.f, 0.f, 1.f);
	this->isWireframe = false;
	this->isVisible = true;
	this->useRGBAColour = false;
	this->drawBBox = false;
	this->doNotLight = false;
	this->isTerrainMesh = false;
	this->hasTexture = false;
	this->isSkyBoxMesh = false;
	this->hasChildMeshes = false;
	this->isAnimated = false;
	this->radius = 7.f;
	this->moveSpeed = 10.f;
	this->min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	this->max = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);
	this->SetUniformScale(1.f);
	this->SetTextureRatiosAcrossTheBoard(0.f);

	// Iterate through all textures and set them to null
	for (unsigned int i = 0; i < 8; i++) {
		textureIDs[i] = -1;
	}
	for (unsigned int i = 0; i < 8; i++) {
		textures[i] = "";
	}
	for (unsigned int i = 0; i < 8; i++) {
		textureRatios[i] = 0.f;
	}
}

cMeshInfo::~cMeshInfo() {

}

void cMeshInfo::LockTarget(glm::vec3 target)
{
	this->target = target - this->position;
	this->velocity = glm::normalize(this->target);
}

void cMeshInfo::TranslateOverTime(float dt)
{
	this->position += this->velocity * dt;
}

void cMeshInfo::TranslateOverTime(glm::vec3 velocity, float dt)
{
	this->velocity = velocity;
	this->position += this->velocity * dt;
}

void cMeshInfo::KillAllForces()
{
	this->velocity = glm::vec3(0.f);
}

void cMeshInfo::SetRotationFromEuler(glm::vec3 newEulerAngleXYZ)
{
	this->rotation = glm::quat(newEulerAngleXYZ);
}

void cMeshInfo::AdjustRoationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust)
{
	// To combine quaternion values, you multiply them together
		// Make a quaternion that represents that CHANGE in angle
	glm::quat qChange = glm::quat(EulerAngleXYZ_Adjust);
	// Multiply them together to get the change
	this->rotation *= qChange;

	//		// This is the same as this
	//		this->qRotation = this->qRotation * qChange;
}

void cMeshInfo::SetUniformScale(float newScale)
{
	this->scale = glm::vec3(newScale, newScale, newScale);
}

void cMeshInfo::SetTextureRatiosAcrossTheBoard(float newTextureRatio)
{
	for (int i = 0; i < 8; i++) {
		this->textureRatios[i] = newTextureRatio;
	}
}

// Make a copy of the vertices from the model file
void cMeshInfo::CopyVertices(sModelDrawInfo model) {
	unsigned int numVertices = model.numberOfVertices;
	std::vector <glm::vec3> vertices(numVertices);

	for (int i = 0; i < numVertices; i++) {
		vertices[i].x = model.pVertices[i].x;
		vertices[i].y = model.pVertices[i].y;
		vertices[i].z = model.pVertices[i].z;

		if (vertices[i].x < this->min.x) this->min.x = vertices[i].x;
		if (vertices[i].x > this->max.x) this->max.x = vertices[i].x;
		if (vertices[i].y < this->min.y) this->min.y = vertices[i].y;
		if (vertices[i].y > this->max.y) this->max.y = vertices[i].y;
		if (vertices[i].z < this->min.z) this->min.z = vertices[i].z;
		if (vertices[i].z > this->max.z) this->max.z = vertices[i].z;
	}
	this->vertices = vertices;
}

void cMeshInfo::CopyIndices(sModelDrawInfo model) {
	unsigned int numIndices = model.numberOfIndices;
	std::vector <unsigned int> indices(numIndices);

	for (int i = 0; i < numIndices; i++) {
		indices[i] = model.pIndices[i];
	}

	unsigned int numTriangles = model.numberOfTriangles;
	std::vector <glm::vec3> tempIndices(numTriangles);

	unsigned int index = 0;
	for (int i = 0; i < numTriangles; i++) {
		tempIndices[i].x = indices[index + 0];
		tempIndices[i].y = indices[index + 1];
		tempIndices[i].z = indices[index + 2];
		index += 3;
	}
	
	this->indices = tempIndices;
}