#include "cLightManager.h"

#include "../OpenGL.h"
#include <sstream>		// Light a string builder in other languages

cLightManager::cLightManager()
{
	//// Create the initial lights
	//for ( unsigned int count = 0;
	//	  count != cLightManager::NUMBER_OF_LIGHTS;
	//	  count++ )
	//{

	//	cLight newLight;
	//	this->vecTheLights.push_back(newLight);
	//}
	ambientLight = 1.f;
}

cLightManager::~cLightManager()
{
	
}

cLight* cLightManager::AddLight(glm::vec4 position) {

	cLight* newLight = new cLight();

	newLight->position = position;
	vecTheLights.push_back(newLight);
	numLights = vecTheLights.size();

	return newLight;
}

void cLightManager::GetAmbientLightAmount(float& amount)
{
	amount = ambientLight;
}

void cLightManager::SetAmbientLightAmount(float& amount)
{
	ambientLight = amount;
}

void cLightManager::LoadLightUniformLocations(unsigned int shaderID)
{
	unsigned int lightNumber = 0;

	numLightsUniformLocation = glGetUniformLocation(shaderID, "NUMBEROFLIGHTS");
	ambientLightUniformLocation = glGetUniformLocation(shaderID, "ambientLight");

	for (int i = 0; i < vecTheLights.size(); i++) {

		cLight* currentLight = vecTheLights[i];

		std::stringstream ssLightName;
		ssLightName << "sLightsArray[" << lightNumber << "].";

		// Set the light information for lights (from the shader)
		std::string lightPosition = ssLightName.str() + "position";
		std::string lightDiffuse = ssLightName.str() + "diffuse";
		std::string lightSpecular = ssLightName.str() + "specular";
		std::string lightAtten = ssLightName.str() + "atten";
		std::string lightDirection = ssLightName.str() + "direction";
		std::string lightParam1 = ssLightName.str() + "param1";
		std::string lightParam2 = ssLightName.str() + "param2";

		currentLight->position_UniformLocation = glGetUniformLocation(shaderID, lightPosition.c_str());
		currentLight->diffuse_UniformLocation = glGetUniformLocation(shaderID, lightDiffuse.c_str());
		currentLight->specular_UniformLocation = glGetUniformLocation(shaderID, lightSpecular.c_str());
		currentLight->atten_UniformLocation = glGetUniformLocation(shaderID, lightAtten.c_str());
		currentLight->direction_UniformLocation = glGetUniformLocation(shaderID, lightDirection.c_str());
		currentLight->param1_UniformLocation = glGetUniformLocation(shaderID, lightParam1.c_str());
		currentLight->param2_UniformLocation = glGetUniformLocation(shaderID, lightParam2.c_str());

		lightNumber++;
	}

	return;
}

void cLightManager::CopyLightInformationToShader(unsigned int shaderID)
{
	// set number of lights
	glUniform1i(numLightsUniformLocation, numLights);
	glUniform1f(ambientLightUniformLocation, ambientLight);

	for (int i = 0; i < vecTheLights.size(); i++)
	{
		cLight* itLight = vecTheLights[i];

		glUniform4f(itLight->position_UniformLocation, 
					itLight->position.x, 
					itLight->position.y, 
					itLight->position.z, 
					itLight->position.w );

		glUniform4f(itLight->diffuse_UniformLocation,
					itLight->diffuse.x,
					itLight->diffuse.y,
					itLight->diffuse.z,
					itLight->diffuse.w );

		glUniform4f(itLight->specular_UniformLocation,
					itLight->specular.x,
					itLight->specular.y,
					itLight->specular.z,
					itLight->specular.w );

		glUniform4f(itLight->atten_UniformLocation,
					itLight->atten.x,
					itLight->atten.y,
					itLight->atten.z,
					itLight->atten.w );

		glUniform4f(itLight->direction_UniformLocation,
					itLight->direction.x,
					itLight->direction.y,
					itLight->direction.z,
					itLight->direction.w );

		glUniform4f(itLight->param1_UniformLocation,
					itLight->param1.x,
					itLight->param1.y,
					itLight->param1.z,
					itLight->param1.w );

		glUniform4f(itLight->param2_UniformLocation,
					itLight->param2.x,
					itLight->param2.y,
					itLight->param2.z,
					itLight->param2.w );


	}

	return;
}

