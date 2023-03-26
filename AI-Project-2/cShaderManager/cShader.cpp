#include "cShader.h"
#include "../OpenGL.h"		// glfw.h, etc. (openGL.h)

cShader::cShader()
{
	this->ID = 0;
	this->shaderType = cShader::UNKNOWN;
	return;
}

cShader::~cShader() 
{
	return;
}

std::string cShader::getShaderTypeString(void)
{
	switch ( this->shaderType )
	{
	case cShader::VERTEX_SHADER:
		return "VERTEX_SHADER";
		break;
	case cShader::FRAGMENT_SHADER:
		return "FRAGMENT_SHADER";
		break;
	case cShader::GEOMETRY_SHADER:
		return "GEOMETRY_SHADER";
		break;
	case cShader::UNKNOWN:
	default:
		return "UNKNOWN_SHADER_TYPE";
		break;
	}
	// Should never reach here...
	return "UNKNOWN_SHADER_TYPE";
}


//// TODO: For the students to do, because it's FUN, FUN, FUN
//std::map< std::string /*name of uniform variable*/,
//		    unsigned int /* uniform location ID */ > 
//						mapUniformName_to_UniformLocation;
