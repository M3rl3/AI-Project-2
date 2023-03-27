#include "OpenGL.h"
#include "sCamera.h"
#include "cMeshInfo/cMeshInfo.h"
#include "Draw Mesh/DrawMesh.h"
#include "Draw Bounding Box/DrawBoundingBox.h"
#include "A-Star Algorithm/A-Star.h"

#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PlyFileLoader/PlyFileLoader.h"
#include "cShaderManager/cShaderManager.h"
#include "cVAOManager/cVAOManager.h"
#include "cLightManager/cLightManager.h"
#include "cBasicTextureManager/cBasicTextureManager.h"
#include "cFBO/cFBO.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>

#include <stdlib.h>
#include <stdio.h>

GLFWwindow* window;
GLint mvp_location = 0;
GLuint shaderID = 0;

PlyFileLoader* plyLoader;
cVAOManager* VAOMan;
cBasicTextureManager* TextureMan;
cLightManager* LightMan;
cFBO* FrameBuffer;

sModelDrawInfo player_obj;

cMeshInfo* full_screen_quad;
cMeshInfo* skybox_sphere_mesh;
cMeshInfo* player_mesh;
cMeshInfo* cube;
cMeshInfo* agent;

cMeshInfo* bulb_mesh;
cLight* pointLight;

unsigned int readIndex = 0;
int object_index = 0;
int elapsed_frames = 0;
int index = 0;

bool enableMouse = false;
bool useFBO = false;
bool mouseClick = false;

std::vector <std::string> meshFiles;
std::vector <cMeshInfo*> meshArray;

void ReadFromFile(std::string filePath);
void LoadTextures(void);
void ManageLights(void);
float RandomFloat(float a, float b);
bool RandomizePositions(cMeshInfo* mesh);
void LoadPlyFilesIntoVAO(void);
int A_STAR_DRIVER();
bool BitmapStream(std::string filePath, std::vector<std::vector<glm::vec3>>& v);
void GenerateCubes(glm::vec3& startPos, float tileSize, std::vector<cMeshInfo*>& blocks);
void RenderToFBO(GLFWwindow* window, sCamera* camera, glm::mat4& view, glm::mat4& projection,
    GLuint eyeLocationLocation, GLuint viewLocation, GLuint projectionLocation,
    GLuint modelLocaction, GLuint modelInverseLocation);

void LoadTextures();

const glm::vec3 origin = glm::vec3(0);
const glm::mat4 matIdentity = glm::mat4(1.0f);
const glm::vec3 upVector = glm::vec3(0.f, 1.f, 0.f);

glm::vec3 wallPos = glm::vec3(0);

glm::vec3 direction(0.f);

// attenuation on all lights
glm::vec4 constLightAtten = glm::vec4(1.0f);

std::vector<std::vector<glm::vec3>> graph;
std::vector<std::vector<glm::vec3>> positions(64, std::vector<glm::vec3>(64));
std::vector<cMeshInfo*> cubes;
std::vector<glm::vec2> path;

int simplifiedGraph[64][64];

enum eEditMode
{
    MOVING_CAMERA,
    MOVING_LIGHT,
    MOVING_SELECTED_OBJECT,
    TAKE_CONTROL
};

// glm::vec3 cameraEye = glm::vec3(-280.0, 140.0, -700.0);
// glm::vec3 cameraTarget = glm::vec3(0.f, 0.f, 0.f);

sCamera* camera;

glm::vec3 initCamera;

eEditMode theEditMode = MOVING_CAMERA;

float yaw = 0.f;
float pitch = 0.f;
float fov = 45.f;

// mouse state
bool firstMouse = true;
float lastX = 800.f / 2.f;
float lastY = 600.f / 2.f;

float beginTime = 0.f;
float currentTime = 0.f;
float timeDiff = 0.f;
int frameCount = 0;

// Pre-existing light, independent of the scene lighting
float ambientLight = 1.f;

static void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        theEditMode = MOVING_CAMERA;
    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        theEditMode = MOVING_SELECTED_OBJECT;
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS) 
    {
        theEditMode = MOVING_LIGHT;
    }
    // Wireframe
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        for (int i = 0; i < meshArray.size(); i++) {
            meshArray[i]->isWireframe = true;
        }
    }
    if (key == GLFW_KEY_X && action == GLFW_RELEASE) {
        for (int i = 0; i < meshArray.size(); i++) {
            meshArray[i]->isWireframe = false;
        }
    }
    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    /* 
    *    updates translation of all objects in the scene based on changes made to scene 
    *    description files, resets everything if no changes were made
    */
    if (key == GLFW_KEY_U && action == GLFW_PRESS) {
        ReadSceneDescription(meshArray);
        camera->position = initCamera;
    }
    // Enable mouse look
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        enableMouse = !enableMouse;
    }

    switch (theEditMode)
    {
        case MOVING_CAMERA:
        {
            const float CAMERA_MOVE_SPEED = 10.f;
            if (key == GLFW_KEY_A)     // Left
            {
                camera->StrafeLeft();
            }
            if (key == GLFW_KEY_D)     // Right
            {
                camera->StrafeRight();
            }
            if (key == GLFW_KEY_W)     // Forward
            {
                camera->MoveForward();
            }
            if (key == GLFW_KEY_S)     // Backwards
            {
                camera->MoveBackward();
            }
            if (key == GLFW_KEY_Q)     // Up
            {
                camera->MoveUp();
            }
            if (key == GLFW_KEY_E)     // Down
            {
                camera->MoveDown();
            }
        }
        break;
        case MOVING_SELECTED_OBJECT:
        {
            const float OBJECT_MOVE_SPEED = 1.f;
            if (key == GLFW_KEY_A)     // Left
            {
                meshArray[object_index]->position.x -= OBJECT_MOVE_SPEED;
            }
            if (key == GLFW_KEY_D)     // Right
            {
                meshArray[object_index]->position.x += OBJECT_MOVE_SPEED;
            }
            if (key == GLFW_KEY_W)     // Forward
            {
                meshArray[object_index]->position.z += OBJECT_MOVE_SPEED;
            }
            if (key == GLFW_KEY_S)     // Backwards
            {
                meshArray[object_index]->position.z -= OBJECT_MOVE_SPEED;
            }
            if (key == GLFW_KEY_Q)     // Down
            {
                meshArray[object_index]->position.y -= OBJECT_MOVE_SPEED;
            }
            if (key == GLFW_KEY_E)     // Up
            {
                meshArray[object_index]->position.y += OBJECT_MOVE_SPEED;
            }

            // cycle through objects in the scene
            if (key == GLFW_KEY_1 && action == GLFW_PRESS)
            {
                object_index++;
                if (object_index > meshArray.size() - 1) {
                    object_index = 0;
                }
                if (!enableMouse) camera->target = meshArray[object_index]->position;
            }
            if (key == GLFW_KEY_2 && action == GLFW_PRESS)
            {
                object_index--;
                if (object_index < 0) {
                    object_index = meshArray.size() - 1;
                }
                if (!enableMouse) camera->target = meshArray[object_index]->position;
            }    
        }
        break;
        case MOVING_LIGHT:
        {
            if (key == GLFW_KEY_1 && action == GLFW_PRESS)
            {
                constLightAtten *= glm::vec4(1, 1.e+1, 1, 1);
            }
            if (key == GLFW_KEY_2 && action == GLFW_PRESS)
            {
                constLightAtten *= glm::vec4(1, 1.e-1, 1, 1);
            }
            if (key == GLFW_KEY_3 && action == GLFW_PRESS)
            {
                constLightAtten *= glm::vec4(1, 1, 1.e+1, 1);
            }
            if (key == GLFW_KEY_4 && action == GLFW_PRESS)
            {
                constLightAtten *= glm::vec4(1, 1, 1.e-1, 1);
            }

            if (key == GLFW_KEY_W && action == GLFW_PRESS)
            {
                ambientLight *= 1.e+1;
                LightMan->SetAmbientLightAmount(ambientLight);
            }
            if (key == GLFW_KEY_S && action == GLFW_PRESS)
            {
                ambientLight *= 1.e-1;
                LightMan->SetAmbientLightAmount(ambientLight);
            }
        }
        break;
    }
}

static void MouseCallBack(GLFWwindow* window, double xposition, double yposition) {

    if (firstMouse) {
        lastX = xposition;
        lastY = yposition;
        firstMouse = false;
    }

    float xoffset = xposition - lastX;
    float yoffset = lastY - yposition;  // reversed since y coordinates go from bottom to up
    lastX = xposition;
    lastY = yposition;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // prevent perspective from getting flipped by capping it
    if (pitch > 89.f) {
        pitch = 89.f;
    }
    if (pitch < -89.f) {
        pitch = -89.f;
    }

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    if (enableMouse) {
        camera->target = glm::normalize(front);
    }
}

static void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
    if (fov >= 1.f && fov <= 45.f) {
        fov -= yoffset;
    }
    if (fov <= 1.f) {
        fov = 1.f;
    }
    if (fov >= 45.f) {
        fov = 45.f;
    }
}

void Initialize() {

    std::cout.flush();

    if (!glfwInit()) {
        std::cerr << "GLFW init failed." << std::endl;
        glfwTerminate();
        return;
    }

    const char* glsl_version = "#version 420";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* currentMonitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* mode = glfwGetVideoMode(currentMonitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    // windowed mode
    // window = glfwCreateWindow(1366, 768, "Man", NULL, NULL);

    // fullscreen support based on current monitor
    window = glfwCreateWindow(mode->width, mode->height, "Man", currentMonitor, NULL);
    
    if (!window) {
        std::cerr << "Window creation failed." << std::endl;
        glfwTerminate();
        return;
    }

    glfwSetWindowAspectRatio(window, 16, 9);

    // keyboard callback
    glfwSetKeyCallback(window, KeyCallback);

    // mouse and scroll callback
    glfwSetCursorPosCallback(window, MouseCallBack);
    glfwSetScrollCallback(window, ScrollCallBack);

    // capture mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetErrorCallback(ErrorCallback);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress))) {
        std::cerr << "Error: unable to obtain pocess address." << std::endl;
        return;
    }
    glfwSwapInterval(1); //vsync

    camera = new sCamera();

    // Init camera object
    //camera->position = glm::vec3(-280.0, 140.0, -700.0);
    camera->position = glm::vec3(-530, 2500.0, -675.0);
    camera->target = glm::vec3(0.f, 0.f, 1.f);

    // Init imgui for crosshair
    // crosshair.Initialize(window, glsl_version);
}

void Render() {

    // FBO
    std::cout << "\nCreating FBO..." << std::endl;
    FrameBuffer = new cFBO();

    useFBO = true;

    int screenWidth = 0;
    int screenHeight = 0;

    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    std::string fboErrorString = "";
    if (!FrameBuffer->init(screenWidth, screenHeight, fboErrorString)) {
        std::cout << "FBO creation error: " << fboErrorString << std::endl;
    }
    else {
        std::cout << "FBO created." << std::endl;
    }

    // Shader Manager
    std::cout << "\nCompiling shaders..." << std::endl;
    cShaderManager* shadyMan = new cShaderManager();

    cShader vertexShader;
    cShader geometryShader;
    cShader fragmentShader;

    vertexShader.fileName = "../assets/shaders/vertexShader.glsl";
    geometryShader.fileName = "../assets/shaders/geometryShader_passthrough.glsl";
    fragmentShader.fileName = "../assets/shaders/fragmentShader.glsl";

    if (!shadyMan->createProgramFromFile("ShadyProgram", vertexShader, geometryShader, fragmentShader)) {
        std::cout << "Error: Shader program failed to compile." << std::endl;
        std::cout << shadyMan->getLastError();
        return;
    }
    else {
        std::cout << "Shaders compiled." << std::endl;
    }

    initCamera = camera->position;

    shaderID = shadyMan->getIDFromFriendlyName("ShadyProgram");
    glUseProgram(shaderID);

    // Load asset paths from external file
    ReadFromFile("./File Stream/readFile.txt");

    // Load the ply model
    plyLoader = new PlyFileLoader();

    // VAO Manager
    VAOMan = new cVAOManager();

    // Scene
    std::cout << "\nLoading assets..." << std::endl;

    // Load all ply files
    LoadPlyFilesIntoVAO();

    // Lights
    LightMan = new cLightManager();

    // Pre-existing light, independent of the scene lighting
    ambientLight = 0.75f;
    LightMan->SetAmbientLightAmount(ambientLight);

    constLightAtten = glm::vec4(0.1f, 2.5e-5f, 2.5e-8f, 1.0f);

    // The actual light
    pointLight = LightMan->AddLight(glm::vec4(0.f, 0.f, 0.f, 1.f));
    pointLight->diffuse = glm::vec4(1.f, 1.f, 1.f, 1.f);
    pointLight->specular = glm::vec4(1.f, 1.f, 1.f, 1.f);
    pointLight->direction = glm::vec4(1.f, 1.f, 1.f, 1.f);
    pointLight->atten = constLightAtten;
    pointLight->param1 = glm::vec4(0.f, 50.f, 50.f, 1.f);
    pointLight->param2 = glm::vec4(1.f, 0.f, 0.f, 1.f);

    // The model to be drawn where the light exists
    bulb_mesh = new cMeshInfo();
    bulb_mesh->meshName = "bulb";
    bulb_mesh->friendlyName = "bulb";
    meshArray.push_back(bulb_mesh);

    glm::vec4 terrainColor = glm::vec4(0.45f, 0.45f, 0.45f, 1.f);

    // Mesh Models
    cMeshInfo* terrain_mesh = new cMeshInfo();
    terrain_mesh->meshName = "terrain";
    terrain_mesh->friendlyName = "terrain";
    terrain_mesh->RGBAColour = terrainColor;
    terrain_mesh->useRGBAColour = true;
    terrain_mesh->isVisible = false;
    terrain_mesh->doNotLight = false;
    meshArray.push_back(terrain_mesh);

    cMeshInfo* flat_plain = new cMeshInfo();
    flat_plain->meshName = "flat_plain";
    flat_plain->friendlyName = "flat_plain";
    flat_plain->RGBAColour = terrainColor;
    flat_plain->useRGBAColour = true;
    flat_plain->hasTexture = false;
    flat_plain->textures[0] = "traversal_graph.bmp";
    flat_plain->textureRatios[0] = 1.f;
    flat_plain->doNotLight = false;
    flat_plain->isVisible = true;
    meshArray.push_back(flat_plain);

    agent = new cMeshInfo();
    agent->meshName = "pyramid";
    agent->friendlyName = "agent";
    agent->doNotLight = false;
    agent->isVisible = true;
    agent->useRGBAColour = true;
    agent->RGBAColour = glm::vec4(50, 30, 0, 1);
    meshArray.push_back(agent);

    skybox_sphere_mesh = new cMeshInfo();
    skybox_sphere_mesh->meshName = "skybox_sphere";
    skybox_sphere_mesh->friendlyName = "skybox_sphere";
    skybox_sphere_mesh->isSkyBoxMesh = true;

    full_screen_quad = new cMeshInfo();
    full_screen_quad->meshName = "fullScreenQuad";
    full_screen_quad->friendlyName = "fullScreenQuad";
    full_screen_quad->doNotLight = false;

    // all textures loaded here
    LoadTextures();

    // reads scene descripion files for positioning and other info
    ReadSceneDescription(meshArray);

    if (!BitmapStream("../assets/textures/ex_traversal_graph.bmp", graph)) {
        std::cout << "Could not open BMP file." << std::endl;
    }

    wallPos = glm::vec3(-2500.0, 0.0, -2000.0);

    GenerateCubes(wallPos, 75.f, cubes);

    glm::vec2 startPos;
    glm::vec2 goalPos;

    for (int i = 0; i < graph.size(); i++) {
        for (int j = 0; j < graph[i].size(); j++) {
            if (graph[i][j] == glm::vec3(0.f)) {
                simplifiedGraph[i][j] = 0;
            }
            else if (graph[i][j] == glm::vec3(255.f)) {
                simplifiedGraph[i][j] = 1;
            }
            else if (graph[i][j] == glm::vec3(76, 177, 34)) {
                startPos.x = i;
                startPos.y = j;

                simplifiedGraph[i][j] = 1;
            }
            else if (graph[i][j] == glm::vec3(36, 28, 237)) {
                goalPos.x = i;
                goalPos.y = j;

                simplifiedGraph[i][j] = 1;
            }
        }
    }

    // Source
    Pair src = make_pair(startPos.x, startPos.y);

    // Destination
    Pair dest = make_pair(goalPos.x, goalPos.y);

    // Object
    A_STAR aStar;

    // Run the actual search and print results
    aStar.aStarSearch(simplifiedGraph, src, dest);

    // Get the path that was found
    path = aStar.GetPath();

    std::cout << std::endl;
}

void Update() {

    // Cull back facing triangles
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // Depth test
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // MVP
    glm::mat4x4 model, view, projection;

    GLint modelLocaction = glGetUniformLocation(shaderID, "Model");
    GLint viewLocation = glGetUniformLocation(shaderID, "View");
    GLint projectionLocation = glGetUniformLocation(shaderID, "Projection");
    GLint modelInverseLocation = glGetUniformLocation(shaderID, "ModelInverse");
    
    // Lighting
    // ManageLights();
    LightMan->LoadLightUniformLocations(shaderID);
    LightMan->CopyLightInformationToShader(shaderID);
    
    pointLight->position = glm::vec4(bulb_mesh->position, 1.f);
    pointLight->atten = constLightAtten;

    float ratio;
    int width, height;

    // Bind the Frame Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer->ID);

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    glViewport(0, 0, width, height);

    FrameBuffer->clearBuffers(true, true);

    // mouse support
    if (enableMouse) {
        view = glm::lookAt(camera->position, camera->position + camera->target, upVector);
        projection = glm::perspective(glm::radians(fov), ratio, 0.1f, 10000.f);
    }
    else {
        view = glm::lookAt(camera->position, camera->target, upVector);
        projection = glm::perspective(0.6f, ratio, 0.1f, 10000.f);
    }

    GLint eyeLocationLocation = glGetUniformLocation(shaderID, "eyeLocation");
    glUniform4f(eyeLocationLocation, camera->position.x, camera->position.y, camera->position.z, 1.f);

    model = glm::mat4(1.f);

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    // Set the camera to follow the agent
    if (!enableMouse) {
        camera->target = agent->position;
    }
    
    elapsed_frames++;

    for (int i = 0; i < meshArray.size(); i++) {

        cMeshInfo* currentMesh = meshArray[i];

        // Check if the current object is the agent
        if (currentMesh->friendlyName == "agent") {

            // Assign position according to the path found by the A* algorithm
            currentMesh->position = positions[(int)path[index].x][(int)path[index].y];

            // Move to the next position after x amount of frames
            if (elapsed_frames > 10) {
                if (index == path.size() - 1) {
                    index = path.size() - 1;
                }
                else {
                    index++;
                }
                elapsed_frames = 0;
            }
        }
         
        // Draw all the meshes pushed onto the vector
        DrawMesh(currentMesh,           // theMesh
                 model,                 // Model Matrix
                 shaderID,              // Compiled Shader ID
                 TextureMan,            // Instance of the Texture Manager
                 VAOMan,                // Instance of the VAO Manager
                 camera,                // Instance of the struct Camera
                 modelLocaction,        // UL for model matrix
                 modelInverseLocation); // UL for transpose of model matrix
        
    }
    
    for (int i = 0; i < cubes.size(); i++) {

        cMeshInfo* currentMesh = cubes[i];
         
        // Draw all the meshes pushed onto the vector
        DrawMesh(currentMesh,           // theMesh
                 model,                 // Model Matrix
                 shaderID,              // Compiled Shader ID
                 TextureMan,            // Instance of the Texture Manager
                 VAOMan,                // Instance of the VAO Manager
                 camera,                // Instance of the struct Camera
                 modelLocaction,        // UL for model matrix
                 modelInverseLocation); // UL for transpose of model matrix
        
    }

    // Draw the skybox
    DrawMesh(skybox_sphere_mesh, matIdentity, shaderID, 
        TextureMan, VAOMan, camera, modelLocaction, modelInverseLocation);

    // Redirect output to an offscreen quad
    if (useFBO)
    {
        RenderToFBO(window, camera, view, projection,
            eyeLocationLocation, viewLocation, projectionLocation,
            modelLocaction, modelInverseLocation);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();

    currentTime = glfwGetTime();
    timeDiff = currentTime - beginTime;
    frameCount++;

    //const GLubyte* vendor = glad_glGetString(GL_VENDOR); // Returns the vendor
    const GLubyte* renderer = glad_glGetString(GL_RENDERER); // Returns a hint to the model

    if (timeDiff >= 1.f / 30.f) {
        std::string frameRate = std::to_string((1.f / timeDiff) * frameCount);
        std::string frameTime = std::to_string((timeDiff / frameCount) * 1000);

        std::stringstream ss;
        if (theEditMode == MOVING_SELECTED_OBJECT) {
            ss << " Camera: " << "(" << camera->position.x << ", " << camera->position.y << ", " << camera->position.z << ")"
               << "    GPU: " << renderer
               << "    FPS: " << frameRate << " ms: " << frameTime
               << "    Target: Index = " << object_index << ", MeshName: " << meshArray[object_index]->friendlyName << ", Position: (" << meshArray[object_index]->position.x << ", " << meshArray[object_index]->position.y << ", " << meshArray[object_index]->position.z << ")"
               ;
        }
        else if (theEditMode == MOVING_LIGHT) {
            ss << " Camera: " << "(" << camera->position.x << ", " << camera->position.y << ", " << camera->position.z << ")"
                << "    GPU: " << renderer
                << "    FPS: " << frameRate << " ms: " << frameTime
                << "    Light Atten: " << "(" << constLightAtten.x << ", " << constLightAtten.y << ", " << constLightAtten.z << ")"
                << "    Ambient Light: " << ambientLight
                ;
        }
        else {
            ss << " Camera: " << "(" << camera->position.x << ", " << camera->position.y << ", " << camera->position.z << ")"
               << "    GPU: " << renderer
               << "    FPS: " << frameRate << " ms: " << frameTime
               ;
        }
        

        glfwSetWindowTitle(window, ss.str().c_str());

        beginTime = currentTime;
        frameCount = 0;
    }
}

void Shutdown() {

    glfwDestroyWindow(window);
    glfwTerminate();

    window = nullptr;
    delete window;

    exit(EXIT_SUCCESS);
}

void ReadFromFile(std::string filePath) {

    std::ifstream readFile;

    readFile.open(filePath);

    if (!readFile.is_open()) {
        readFile.close();
        return;
    }

    std::string input0;

    while (readFile >> input0) {
        meshFiles.push_back(input0);
        readIndex++;
    }

    readFile.close();
}

struct BMPHeader {
    unsigned char headerField[2];
    unsigned char sizeOfBMP[4];
    unsigned char reserved1[2];
    unsigned char reserved2[2];
    unsigned char dataOffset[4];
};

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

bool BitmapStream(std::string filePath, std::vector<std::vector<glm::vec3>>& v) {

    std::ifstream bmpStream;
    bmpStream.open(filePath);

    if (!bmpStream.is_open()) {
        return false;
    }

    bmpStream.seekg(0);

    BMPHeader fileHeader;
    bmpStream.read((char*)&fileHeader.headerField, sizeof(fileHeader));
    bmpStream.seekg(fileHeader.dataOffset[0]);

    printf("\n");

    // Assumes the size of the image is 64x64
    std::vector<glm::vec3> localgraph(4096);

    Color color;
    for (int i = 0; i < 4096; i++) {
        bmpStream.read((char*)&color, 3);

        localgraph[i].r = (int)color.r;
        localgraph[i].g = (int)color.g;
        localgraph[i].b = (int)color.b;
    }

    // Initialize a 2D vector of size 64x64
    std::vector<std::vector<glm::vec3>> v1(64, std::vector<glm::vec3>(64)); 

    // Copy over the 1D vector into the 2D vector
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            v1[i][j] = localgraph[i * 64 + j];
        }
    }

    v = v1;

    int breakpoint = 0;

    bmpStream.close();
    return true;
}

void GenerateCubes(glm::vec3& startPos, float tileSize, std::vector<cMeshInfo*>& blocks) {

    glm::vec3 temp = startPos;

    startPos.y += 40.f;

    for (int i = 0; i < graph.size(); i++) {
        for (int j = 0; j < graph[i].size(); j++) {
            positions[i][j] = startPos;

            if (graph[i][j] == glm::vec3(0, 0, 0)) {

                cube = new cMeshInfo();
                cube->meshName = "wall_cube";
                cube->friendlyName = "wall_cube";
                cube->useRGBAColour = true;
                cube->RGBAColour = glm::vec4(0.f, 0.f, 0.f, 1.f);
                cube->SetUniformScale(tileSize);
                cube->SetRotationFromEuler(origin);

                cube->position = startPos;
                blocks.push_back(cube);

                startPos.x += tileSize;
            }
            else if (graph[i][j] == glm::vec3(36, 28, 237)) {

                cube = new cMeshInfo();
                cube->meshName = "wall_cube";
                cube->friendlyName = "wall_cube";
                cube->useRGBAColour = true;
                cube->RGBAColour = glm::vec4(10, 0, 0, 1.f);
                cube->SetUniformScale(tileSize);
                cube->SetRotationFromEuler(origin);

                cube->position = startPos;
                blocks.push_back(cube);

                startPos.x += 75.f;
            }
            else if (graph[i][j] == glm::vec3(76, 177, 34)) {

                cube = new cMeshInfo();
                cube->meshName = "wall_cube";
                cube->friendlyName = "wall_cube";
                cube->useRGBAColour = true;
                cube->RGBAColour = glm::vec4(0, 10, 0, 1.f);
                cube->SetUniformScale(tileSize);
                cube->SetRotationFromEuler(origin);

                cube->position = startPos;
                blocks.push_back(cube);

                startPos.x += tileSize;
            }
            else {
                startPos.x += tileSize;
            }
        }
        startPos.x = temp.x;
        startPos.z += tileSize;
    }
    int breakPoint = 0;
}

// All lights managed here
void ManageLights() {
    
    GLint PositionLocation = glGetUniformLocation(shaderID, "sLightsArray[0].position");
    GLint DiffuseLocation = glGetUniformLocation(shaderID, "sLightsArray[0].diffuse");
    GLint SpecularLocation = glGetUniformLocation(shaderID, "sLightsArray[0].specular");
    GLint AttenLocation = glGetUniformLocation(shaderID, "sLightsArray[0].atten");
    GLint DirectionLocation = glGetUniformLocation(shaderID, "sLightsArray[0].direction");
    GLint Param1Location = glGetUniformLocation(shaderID, "sLightsArray[0].param1");
    GLint Param2Location = glGetUniformLocation(shaderID, "sLightsArray[0].param2");

    //glm::vec3 lightPosition0 = meshArray[1]->position;
    glm::vec3 lightPosition0 = meshArray[0]->position;
    glUniform4f(PositionLocation, lightPosition0.x, lightPosition0.y, lightPosition0.z, 1.0f);
    //glUniform4f(PositionLocation, 0.f, 0.f, 0.f, 1.0f);
    glUniform4f(DiffuseLocation, 1.f, 1.f, 1.f, 1.f);
    glUniform4f(SpecularLocation, 1.f, 1.f, 1.f, 1.f);
    glUniform4f(AttenLocation, 0.1f, 0.5f, 0.1f, 1.f);
    glUniform4f(DirectionLocation, 1.f, 1.f, 1.f, 1.f);
    glUniform4f(Param1Location, 0.f, 0.f, 0.f, 1.f); //x = Light Type
    glUniform4f(Param2Location, 1.f, 0.f, 0.f, 1.f); //x = Light on/off
}

void LoadTextures() {

    std::cout << "\nLoading textures...";

    std::string errorString = "";
    TextureMan = new cBasicTextureManager();

    TextureMan->SetBasePath("../assets/textures");

    // skybox/cubemap textures
    std::string skyboxName = "NightSky";
    if (TextureMan->CreateCubeTextureFromBMPFiles(skyboxName,
        "SpaceBox_right1_posX.bmp",
        "SpaceBox_left2_negX.bmp",
        "SpaceBox_top3_posY.bmp",
        "SpaceBox_bottom4_negY.bmp",
        "SpaceBox_front5_posZ.bmp",
        "SpaceBox_back6_negZ.bmp",
        true, errorString))
    {
        std::cout << "\nLoaded skybox textures: " << skyboxName << std::endl;
    }
    else
    {
        std::cout << "\nError: failed to load skybox because " << errorString;
    }
    
    if (TextureMan->Create2DTextureFromBMPFile("traversal_graph.bmp"))
    {
        std::cout << "Loaded traversal_graph texture." << std::endl;
    }
    else
    {
        std::cout << "Error: failed to load traversal_graph texture.";
    }
}

float RandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void RenderToFBO(GLFWwindow* window, sCamera* camera, glm::mat4& view, glm::mat4& projection,
                 GLuint eyeLocationLocation, GLuint viewLocation, GLuint projectionLocation,
                 GLuint modelLocaction, GLuint modelInverseLocation)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 exCameraEye = camera->position;
    glm::vec3 exCameraLookAt = camera->target;

    camera->position = glm::vec3(0.0f, 0.0f, -6.0f);
    camera->target = glm::vec3(0.0f, 0.0f, 0.0f);

    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    GLint FBOsizeLocation = glGetUniformLocation(shaderID, "FBO_size");
    GLint screenSizeLocation = glGetUniformLocation(shaderID, "screen_size");

    glUniform2f(FBOsizeLocation, (GLfloat)FrameBuffer->width, (GLfloat)FrameBuffer->height);
    glUniform2f(screenSizeLocation, (GLfloat)width, (GLfloat)height);

    projection = glm::perspective(0.6f, ratio, 0.1f, 100.f);

    glViewport(0, 0, width, height);

    view = glm::lookAt(camera->position, camera->target, upVector);

    // Set eyelocation again
    glUniform4f(eyeLocationLocation, camera->position.x, camera->position.y, camera->position.z, 1.f);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    // Drawing
    GLint bIsFullScreenQuadLocation = glGetUniformLocation(shaderID, "bIsFullScreenQuad");
    glUniform1f(bIsFullScreenQuadLocation, (GLfloat)GL_TRUE);

    // Texture on the quad
    GLuint texture21Unit = 21;			// Picked 21 because it's not being used
    glActiveTexture(texture21Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
    glBindTexture(GL_TEXTURE_2D, FrameBuffer->colourTexture_0_ID);

    GLint FBO_TextureLocation = glGetUniformLocation(shaderID, "FBO_Texture");
    glUniform1i(FBO_TextureLocation, texture21Unit);

    full_screen_quad->SetUniformScale(10.f);
    full_screen_quad->isVisible = true;

    // Draw the quad
    DrawMesh(full_screen_quad,
        matIdentity,
        shaderID,
        TextureMan,
        VAOMan,
        camera,
        modelLocaction,
        modelInverseLocation);

    camera->position = exCameraEye;
    camera->target = exCameraLookAt;

    glUniform1f(bIsFullScreenQuadLocation, (GLfloat)GL_FALSE);
}

bool RandomizePositions(cMeshInfo* mesh) {

    int i = 0;
    float x, y, z, w;

    x = RandomFloat(-500, 500);
    y = mesh->position.y;
    z = RandomFloat(-200, 200);

    mesh->position = glm::vec3(x, y, z);
    
    return true;
}

void LoadPlyFilesIntoVAO(void)
{
    sModelDrawInfo bulb;
    plyLoader->LoadModel(meshFiles[0], bulb);
    if (!VAOMan->LoadModelIntoVAO("bulb", bulb, shaderID)) {
        std::cerr << "Could not load model into VAO" << std::endl;
    }

    sModelDrawInfo flat_plain_obj;
    plyLoader->LoadModel(meshFiles[9], flat_plain_obj);
    if (!VAOMan->LoadModelIntoVAO("flat_plain", flat_plain_obj, shaderID)) {
        std::cerr << "Could not load model into VAO" << std::endl;
    }

    sModelDrawInfo wall_cube;
    plyLoader->LoadModel(meshFiles[2], wall_cube);
    if (!VAOMan->LoadModelIntoVAO("wall_cube", wall_cube, shaderID)) {
        std::cerr << "Could not load model into VAO" << std::endl;
    }
    
    sModelDrawInfo pyramid;
    plyLoader->LoadModel(meshFiles[11], pyramid);
    if (!VAOMan->LoadModelIntoVAO("pyramid", pyramid, shaderID)) {
        std::cerr << "Could not load model into VAO" << std::endl;
    }
    
    // 2-sided full screen quad aligned to x-y axis
    sModelDrawInfo fullScreenQuad;
    plyLoader->LoadModel(meshFiles[10], fullScreenQuad);
    if (!VAOMan->LoadModelIntoVAO("fullScreenQuad", fullScreenQuad, shaderID)) {
        std::cerr << "Could not load model into VAO" << std::endl;
    }

    // skybox sphere with inverted normals
    sModelDrawInfo skybox_sphere_obj;
    plyLoader->LoadModel(meshFiles[6], skybox_sphere_obj);
    if (!VAOMan->LoadModelIntoVAO("skybox_sphere", skybox_sphere_obj, shaderID)) {
        std::cerr << "Could not load model into VAO" << std::endl;
    }
}

int main(int argc, char** argv) 
{
    Initialize();
    Render();
    
    while (!glfwWindowShouldClose(window)) {
        Update();
    }
    
    Shutdown();
    
    return 0;
}