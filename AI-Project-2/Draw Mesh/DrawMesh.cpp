#include "DrawMesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "../Draw Bounding Box/DrawBoundingBox.h"

// Function for drawing individual meshes
void DrawMesh(cMeshInfo* currentMesh,
    glm::mat4 model,
    GLuint shaderID,
    cBasicTextureManager* TextureManager,
    cVAOManager* VAOManager,
    sCamera* camera,
    GLint modelULoc,
    GLint modelInverseULoc) {


    // Don't draw any "back facing" triangles
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // Turn on depth buffer test at draw time
    glEnable(GL_DEPTH_TEST);

    //model = glm::mat4x4(1.f);

    glm::mat4 matModel = model;
    glm::vec3 origin = glm::vec3(1.f);

    if (currentMesh->isVisible == false) {
        return;
    }

    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), currentMesh->position);
    glm::mat4 scaling = glm::scale(glm::mat4(1.f), currentMesh->scale);

    glm::mat4 rotation = glm::mat4_cast(currentMesh->rotation);

    matModel *= translationMatrix;
    matModel *= rotation;
    matModel *= scaling;

    glUniformMatrix4fv(modelULoc, 1, GL_FALSE, glm::value_ptr(matModel));

    glm::mat4 modelInverse = glm::inverse(glm::transpose(matModel));
    glUniformMatrix4fv(modelInverseULoc, 1, GL_FALSE, glm::value_ptr(modelInverse));

    if (currentMesh->isWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint useIsTerrainMeshLocation = glGetUniformLocation(shaderID, "bIsTerrainMesh");

    if (currentMesh->isTerrainMesh)
    {
        glUniform1f(useIsTerrainMeshLocation, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(useIsTerrainMeshLocation, (GLfloat)GL_FALSE);
    }

    GLint RGBAColourLocation = glGetUniformLocation(shaderID, "RGBAColour");

    glUniform4f(RGBAColourLocation, currentMesh->RGBAColour.r, currentMesh->RGBAColour.g, currentMesh->RGBAColour.b, currentMesh->RGBAColour.w);

    GLint useRGBAColourLocation = glGetUniformLocation(shaderID, "useRGBAColour");

    if (currentMesh->useRGBAColour)
    {
        glUniform1f(useRGBAColourLocation, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(useRGBAColourLocation, (GLfloat)GL_FALSE);
    }

    GLint bHasTextureLocation = glGetUniformLocation(shaderID, "bHasTexture");

    if (currentMesh->hasTexture)
    {
        glUniform1f(bHasTextureLocation, (GLfloat)GL_TRUE);

        if (currentMesh->textures[0] != "") {

            std::string texture0 = currentMesh->textures[0];

            GLuint texture0ID = TextureManager->getTextureIDFromName(texture0);

            GLuint texture0Unit = 0;
            glActiveTexture(texture0Unit + GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture0ID);

            GLint texture0Location = glGetUniformLocation(shaderID, "texture0");
            glUniform1i(texture0Location, texture0Unit);

            GLint texRatio_0_3 = glGetUniformLocation(shaderID, "texRatio_0_3");
            glUniform4f(texRatio_0_3,
                currentMesh->textureRatios[0],
                currentMesh->textureRatios[1],
                currentMesh->textureRatios[2],
                currentMesh->textureRatios[3]);
        }
        else {
          
            GLuint texture0ID = currentMesh->textureIDs[0];
            
            GLuint texture0Unit = 0;
            glActiveTexture(texture0Unit + GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture0ID);
            
            GLint texture0Location = glGetUniformLocation(shaderID, "texture0");
            glUniform1i(texture0Location, texture0Unit);
            
            GLint texRatio_0_3 = glGetUniformLocation(shaderID, "texRatio_0_3");
            glUniform4f(texRatio_0_3,
                currentMesh->textureRatios[0],
                currentMesh->textureRatios[1],
                currentMesh->textureRatios[2],
                currentMesh->textureRatios[3]);
        }
    }
    else
    {
        glUniform1f(bHasTextureLocation, (GLfloat)GL_FALSE);
    }

    GLint doNotLightLocation = glGetUniformLocation(shaderID, "doNotLight");

    if (currentMesh->doNotLight)
    {
        glUniform1f(doNotLightLocation, (GLfloat)GL_TRUE);

    }
    else
    {
        glUniform1f(doNotLightLocation, (GLfloat)GL_FALSE);
    }

    GLint bIsSkyboxObjectLocation = glGetUniformLocation(shaderID, "bIsSkyboxObject");

    if (currentMesh->isSkyBoxMesh) {

        //skybox texture
        GLuint cubeMapTextureNumber = TextureManager->getTextureIDFromName("NightSky");
        GLuint texture30Unit = 30;			// Texture unit go from 0 to 79
        glActiveTexture(texture30Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureNumber);
        GLint skyboxTextureLocation = glGetUniformLocation(shaderID, "skyboxTexture");
        glUniform1i(skyboxTextureLocation, texture30Unit);

        glUniform1f(bIsSkyboxObjectLocation, (GLfloat)GL_TRUE);
        currentMesh->position = camera->position;
        currentMesh->SetUniformScale(7500.f);
    }
    else {
        glUniform1f(bIsSkyboxObjectLocation, (GLfloat)GL_FALSE);
    }

    sModelDrawInfo modelInfo;
    if (VAOManager->FindDrawInfoByModelName(currentMesh->meshName, modelInfo)) {

        glBindVertexArray(modelInfo.VAO_ID);
        glDrawElements(GL_TRIANGLES, modelInfo.numberOfIndices, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }
    else {
        std::cout << "Model not found." << std::endl;
    }

    // adds the model's velocity to its current position
    if (currentMesh->velocity != origin) {
        currentMesh->TranslateOverTime(1.f);
    }

    if (currentMesh->hasChildMeshes) {
        for (int i = 0; i < currentMesh->vecChildMeshes.size(); i++) {

            cMeshInfo* currentChild = currentMesh->vecChildMeshes[i];

            DrawMesh(currentChild, matModel, shaderID, TextureManager, VAOManager, camera, modelULoc, modelInverseULoc);
        }
    }

    // Only draw bounding box around meshes with this boolean value set to true
    if (currentMesh->drawBBox) {

        // pass in the model matrix after drawing
        // so it doesnt screw with the matrix values

        draw_bbox(currentMesh, shaderID, matModel);
    }

    return;
}

void DrawMesh(cMeshInfo* currentMesh,
    glm::mat4 model,
    GLuint shaderID,
    GLuint textureID,
    cBasicTextureManager* TextureManager,
    cVAOManager* VAOManager,
    sCamera* camera,
    GLint modelULoc,
    GLint modelInverseULoc) {


    // Don't draw any "back facing" triangles
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // Turn on depth buffer test at draw time
    glEnable(GL_DEPTH_TEST);

    //model = glm::mat4x4(1.f);

    glm::mat4 matModel = model;
    glm::vec3 origin = glm::vec3(1.f);

    if (currentMesh->isVisible == false) {
        return;
    }

    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), currentMesh->position);
    glm::mat4 scaling = glm::scale(glm::mat4(1.f), currentMesh->scale);

    glm::mat4 rotation = glm::mat4_cast(currentMesh->rotation);

    matModel *= translationMatrix;
    matModel *= rotation;
    matModel *= scaling;

    glUniformMatrix4fv(modelULoc, 1, GL_FALSE, glm::value_ptr(matModel));

    glm::mat4 modelInverse = glm::inverse(glm::transpose(matModel));
    glUniformMatrix4fv(modelInverseULoc, 1, GL_FALSE, glm::value_ptr(modelInverse));

    if (currentMesh->isWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint useIsTerrainMeshLocation = glGetUniformLocation(shaderID, "bIsTerrainMesh");

    if (currentMesh->isTerrainMesh)
    {
        glUniform1f(useIsTerrainMeshLocation, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(useIsTerrainMeshLocation, (GLfloat)GL_FALSE);
    }

    GLint RGBAColourLocation = glGetUniformLocation(shaderID, "RGBAColour");

    glUniform4f(RGBAColourLocation, currentMesh->RGBAColour.r, currentMesh->RGBAColour.g, currentMesh->RGBAColour.b, currentMesh->RGBAColour.w);

    GLint useRGBAColourLocation = glGetUniformLocation(shaderID, "useRGBAColour");

    if (currentMesh->useRGBAColour)
    {
        glUniform1f(useRGBAColourLocation, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(useRGBAColourLocation, (GLfloat)GL_FALSE);
    }

    GLint bHasTextureLocation = glGetUniformLocation(shaderID, "bHasTexture");

    if (currentMesh->hasTexture)
    {
        glUniform1f(bHasTextureLocation, (GLfloat)GL_TRUE);

        if (currentMesh->textures[0] != "") {

            // std::string texture0 = currentMesh->textures[0];

            GLuint texture0ID = textureID;

            GLuint texture0Unit = 19;
            glActiveTexture(texture0Unit + GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture0ID);

            GLint texture0Location = glGetUniformLocation(shaderID, "texture1");
            glUniform1i(texture0Location, texture0Unit);

            GLint texRatio_0_3 = glGetUniformLocation(shaderID, "texRatio_0_3");
            glUniform4f(texRatio_0_3,
                currentMesh->textureRatios[0],
                currentMesh->textureRatios[1],
                currentMesh->textureRatios[2],
                currentMesh->textureRatios[3]);
        }
    }
    else
    {
        glUniform1f(bHasTextureLocation, (GLfloat)GL_FALSE);
    }

    GLint doNotLightLocation = glGetUniformLocation(shaderID, "doNotLight");

    if (currentMesh->doNotLight)
    {
        glUniform1f(doNotLightLocation, (GLfloat)GL_TRUE);

    }
    else
    {
        glUniform1f(doNotLightLocation, (GLfloat)GL_FALSE);
    }

    GLint bIsSkyboxObjectLocation = glGetUniformLocation(shaderID, "bIsSkyboxObject");

    if (currentMesh->isSkyBoxMesh) {

        //skybox texture
        GLuint cubeMapTextureNumber = TextureManager->getTextureIDFromName("NightSky");
        GLuint texture30Unit = 30;			// Texture unit go from 0 to 79
        glActiveTexture(texture30Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureNumber);
        GLint skyboxTextureLocation = glGetUniformLocation(shaderID, "skyboxTexture");
        glUniform1i(skyboxTextureLocation, texture30Unit);

        glUniform1f(bIsSkyboxObjectLocation, (GLfloat)GL_TRUE);
        currentMesh->position = camera->position;
        currentMesh->SetUniformScale(7500.f);
    }
    else {
        glUniform1f(bIsSkyboxObjectLocation, (GLfloat)GL_FALSE);
    }

    sModelDrawInfo modelInfo;
    if (VAOManager->FindDrawInfoByModelName(currentMesh->meshName, modelInfo)) {

        glBindVertexArray(modelInfo.VAO_ID);
        glDrawElements(GL_TRIANGLES, modelInfo.numberOfIndices, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }
    else {
        std::cout << "Model not found." << std::endl;
    }

    // adds the model's velocity to its current position
    if (currentMesh->velocity != origin) {
        currentMesh->TranslateOverTime(1.f);
    }

    if (currentMesh->hasChildMeshes) {
        for (int i = 0; i < currentMesh->vecChildMeshes.size(); i++) {

            cMeshInfo* currentChild = currentMesh->vecChildMeshes[i];

            DrawMesh(currentChild, matModel, shaderID, TextureManager, VAOManager, camera, modelULoc, modelInverseULoc);
        }
    }

    // Only draw bounding box around meshes with this boolean value set to true
    if (currentMesh->drawBBox) {

        // pass in the model matrix after drawing
        // so it doesnt screw with the matrix values

        draw_bbox(currentMesh, shaderID, matModel);
    }

    return;
}

//read scene description files
void ReadSceneDescription(std::vector<cMeshInfo*>& meshArray) {

    std::ifstream File("./File Stream/sceneDescription.txt");
    if (!File.is_open()) {
        std::cout << "\nCould not load file." << std::endl;
        return;
    }

    int number = 0;
    std::string input0;
    std::string input1;
    std::string input2;
    std::string input3;

    std::string temp;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    File >> number;

    for (int i = 0; i < number; i++) {
        File >> input0
            >> input1 >> position.x >> position.y >> position.z
            >> input2 >> rotation.x >> rotation.y >> rotation.z
            >> input3 >> scale.x >> scale.y >> scale.z;

        /*  long_highway
            position 0.0 -1.0 0.0
            rotation 0.0 0.0 0.0
            scale 1.0 1.0 1.0
        */

        temp = input0;

        if (input1 == "position") {
            meshArray[i]->position.x = position.x;
            meshArray[i]->position.y = position.y;
            meshArray[i]->position.z = position.z;
        }
        if (input2 == "rotation") {
            /*meshArray[i]->rotation.x = rotation.x;
            meshArray[i]->rotation.y = rotation.y;
            meshArray[i]->rotation.z = rotation.z;*/
            meshArray[i]->SetRotationFromEuler(rotation);
        }
        if (input3 == "scale") {
            meshArray[i]->scale.x = scale.x;
            meshArray[i]->scale.y = scale.y;
            meshArray[i]->scale.z = scale.z;
        }
    }
    File.close();

    /*std::string yes;
    float x, y, z;
    std::ifstream File1("cameraEye.txt");
    if (!File1.is_open()) {
        std::cerr << "Could not load file." << std::endl;
        return;
    }
    while (File1 >> yes >> x >> y >> z) {
        ::cameraEye.x = x;
        ::cameraEye.y = y;
        ::cameraEye.z = z;
    }
    File1.close();*/
}