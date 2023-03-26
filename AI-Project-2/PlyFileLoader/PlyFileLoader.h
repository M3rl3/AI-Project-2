#pragma once

#include <fstream>
#include <iostream>

#include <string>
#include <vector>

#include "../cVAOManager/cVAOManager.h"
 
class PlyFileLoader {
public:

    struct vertexLayout {

        float x, y, z;
        float nx, ny, nz;
        float r, g, b, a;
        float texture_u, texture_v;
    };

    struct triangleLayout {

        unsigned int triangleIndices[3];
    };

	PlyFileLoader();
	~PlyFileLoader();

	sModelDrawInfo* GetPlyModelByID(unsigned int id);
	int LoadModel(std::string fileName, sModelDrawInfo& plyModel);

private:
	std::vector<sModelDrawInfo*> plyModels;
};
