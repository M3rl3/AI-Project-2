#ifndef _cShaderProgram_HG_
#define _cShaderProgram_HG_

#include <map>
#include <string>

class cShaderProgram {
public:

	cShaderProgram();
	~cShaderProgram();

	unsigned int ID;	// ID from OpenGL (calls it a "name")
	std::string friendlyName;	// We give it this name

	// TODO: For the students to do, because it's FUN, FUN, FUN
	std::map< std::string /*name of uniform variable*/,
		int /* uniform location ID */ >
		mapUniformName_to_UniformLocation;

	// Returns -1 (just like OpenGL) if NOT found
	int getUniformID_From_Name(std::string name);

	// Look up the uniform location and save it.
	bool LoadUniformLocation(std::string variableName);
};

#endif