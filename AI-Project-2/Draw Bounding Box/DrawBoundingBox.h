#pragma once

#include "../cMeshInfo/cMeshInfo.h"
#include <glm/mat4x4.hpp>

void draw_bbox(cMeshInfo* mesh, unsigned int shaderID, glm::mat4 meshTransform);