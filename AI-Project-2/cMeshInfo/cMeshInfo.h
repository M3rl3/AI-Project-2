#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../cVAOManager/cVAOManager.h"

class cMeshInfo {

public:

	cMeshInfo();
	~cMeshInfo();

	std::string meshName;
	std::string friendlyName;

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 velocity;
	glm::vec3 target;
	glm::vec3 up;
	glm::quat rotation;
	glm::vec4 colour;
	glm::vec4 RGBAColour;
	glm::vec3 facingDirection;

	int counter = 0;
	float radius;
	float rotationAngle;
	bool hasCompletedRotation;
	bool completed = false;
	float moveSpeed = 0;
	
	//float scale;
	bool isWireframe;
	bool isVisible;
	bool drawBBox;
	bool useRGBAColour;
	bool hasTexture;
	bool teleport;
	bool doNotLight;
	bool isTerrainMesh;
	bool isSkyBoxMesh;
	bool hasChildMeshes;
	bool isAnimated;

	int nTriangles;
	int nIndices;
	int nVertices;

	int textureIDs[8];
	std::string textures[8];
	float textureRatios[8];

	std::string message;

	std::vector <glm::vec3> vertices;
	std::vector <glm::vec3> indices;

	glm::vec3 min;
	glm::vec3 max;

	void LockTarget(glm::vec3 target);
	void TranslateOverTime(float dt);
	void TranslateOverTime(glm::vec3 velocity, float dt);
	void KillAllForces();
	void SetRotationFromEuler(glm::vec3 newEulerAngleXYZ);
	void AdjustRoationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust);
	void SetUniformScale(float newScale);
	void SetTextureRatiosAcrossTheBoard(float newTextureRatio);

	void CopyVertices(sModelDrawInfo model);
	void CopyIndices(sModelDrawInfo model);

	std::vector <cMeshInfo*> vecChildMeshes;
};