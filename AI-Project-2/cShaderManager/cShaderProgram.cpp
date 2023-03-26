#include "cShaderProgram.h"

#include "../OpenGL.h"

cShaderProgram::cShaderProgram() : ID(0) 
{
}

cShaderProgram::~cShaderProgram() 
{
}

// Look up the uniform inside the shader, then save it, if it finds it
bool cShaderProgram::LoadUniformLocation(std::string variableName)
{
	// 
	GLint uniLocation = glGetUniformLocation(this->ID,
		variableName.c_str());
	// Did it find it (not -1)
	if (uniLocation == -1)
	{	// Nope.
		return false;
	}
	// Save it
	this->mapUniformName_to_UniformLocation[variableName.c_str()] = uniLocation;

	return true;
}

// Look up the uniform location and save it.
int cShaderProgram::getUniformID_From_Name(std::string name)
{
	std::map< std::string /*name of uniform variable*/,
		int /* uniform location ID */ >::iterator
		itUniform = this->mapUniformName_to_UniformLocation.find(name);

	if (itUniform == this->mapUniformName_to_UniformLocation.end())
	{
		return -1;		// OpenGL uniform not found value
	}

	return itUniform->second;		// second if the "int" value
}