#include "PlyFileLoader.h"

PlyFileLoader::PlyFileLoader() {

}

PlyFileLoader::~PlyFileLoader() {

}

sModelDrawInfo* PlyFileLoader::GetPlyModelByID(unsigned int id) {

    return plyModels[id];
}

int PlyFileLoader::LoadModel(std::string fileName, sModelDrawInfo& plyModel) {

    vertexLayout* modelArray = NULL;
    triangleLayout* triangleArray = NULL;

    std::ifstream plyFile(fileName);
    if (!plyFile.is_open()) {
        std::cout << "Could not load file." << std::endl;
        return -1;
    }

    std::string input1;

    // Scan for the word "vertex"
    while (plyFile >> input1)
    {
        if (input1 == "vertex")
        {
            break;
        }
    }

    plyFile >> plyModel.numberOfVertices;

    // Scan for the word "face"
    while (plyFile >> input1)
    {
        if (input1 == "face")
        {
            break;
        }
    }

    plyFile >> plyModel.numberOfTriangles;

    // Scan for the word "end_header"
    while (plyFile >> input1)
    {
        if (input1 == "end_header")
        {
            break;
        }
    }

    //Vertex Layout
    modelArray = new vertexLayout[plyModel.numberOfVertices];

    std::cout << "Loading " << fileName << std::endl;
    for (unsigned int count = 0; count != plyModel.numberOfVertices; count++)
    {
        plyFile >> modelArray[count].x;
        plyFile >> modelArray[count].y;
        plyFile >> modelArray[count].z;

        plyFile >> modelArray[count].nx;
        plyFile >> modelArray[count].ny;
        plyFile >> modelArray[count].nz;

        plyFile >> modelArray[count].r;
        plyFile >> modelArray[count].g;
        plyFile >> modelArray[count].b;
        plyFile >> modelArray[count].a;

        plyFile >> modelArray[count].texture_u;
        plyFile >> modelArray[count].texture_v;
        int breakpoint = 1;
    }

    triangleArray = new triangleLayout[plyModel.numberOfTriangles];

    // Triangle Layout
    for (unsigned int count = 0; count != plyModel.numberOfTriangles; count++) {
        unsigned int temp = 0;
        plyFile >> temp;

        plyFile >> triangleArray[count].triangleIndices[0];
        plyFile >> triangleArray[count].triangleIndices[1];
        plyFile >> triangleArray[count].triangleIndices[2];
        int breakpoint = 1;
    }
    plyFile.close();

    plyModel.pVertices = new vertLayout[plyModel.numberOfVertices];

    for (unsigned int index = 0; index != plyModel.numberOfVertices; index++)
    {
        plyModel.pVertices[index].x = modelArray[index].x;
        plyModel.pVertices[index].y = modelArray[index].y;
        plyModel.pVertices[index].z = modelArray[index].z;

        plyModel.pVertices[index].r = modelArray[index].r;
        plyModel.pVertices[index].g = modelArray[index].g;
        plyModel.pVertices[index].b = modelArray[index].b;

        plyModel.pVertices[index].nx = modelArray[index].nx;
        plyModel.pVertices[index].ny = modelArray[index].ny;
        plyModel.pVertices[index].nz = modelArray[index].nz;

        plyModel.pVertices[index].texture_u = modelArray[index].texture_u;
        plyModel.pVertices[index].texture_v = modelArray[index].texture_v;
    }

    plyModel.numberOfIndices = plyModel.numberOfTriangles * 3;
    plyModel.pIndices = new unsigned int[plyModel.numberOfIndices];

    unsigned int indexIndices = 0;

    for (unsigned int triangleIndex = 0; triangleIndex != plyModel.numberOfTriangles; triangleIndex++)
    {
        plyModel.pIndices[indexIndices + 0] = triangleArray[triangleIndex].triangleIndices[0];
        plyModel.pIndices[indexIndices + 1] = triangleArray[triangleIndex].triangleIndices[1];
        plyModel.pIndices[indexIndices + 2] = triangleArray[triangleIndex].triangleIndices[2];

        indexIndices += 3;
    }

    if (&plyModel != nullptr) {

        plyModels.push_back(&plyModel);
    }
    
    delete[] modelArray;
    delete[] triangleArray;

    return plyModels.size() - 1;
}