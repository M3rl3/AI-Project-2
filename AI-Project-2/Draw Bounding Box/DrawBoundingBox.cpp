#include "DrawBoundingBox.h"

#include "../OpenGL.h"
#include "../cMeshInfo/cMeshInfo.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Takes in the mesh to put the bounding box around, 
// the compiled shader id, and the model matrix of  
// the mesh after all transformations have been applied.

void draw_bbox(cMeshInfo* mesh, unsigned int shaderID, glm::mat4 meshTransform) {
    if (mesh->vertices.size() == 0)
        return;

    // Cube 1x1x1, centered on origin
    GLfloat vertices[] = {
        -0.5, -0.5, -0.5, 1.0,
         0.5, -0.5, -0.5, 1.0,
         0.5,  0.5, -0.5, 1.0,
        -0.5,  0.5, -0.5, 1.0,
        -0.5, -0.5,  0.5, 1.0,
         0.5, -0.5,  0.5, 1.0,
         0.5,  0.5,  0.5, 1.0,
        -0.5,  0.5,  0.5, 1.0,
    };

    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLushort elements[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 
        2, 6, 3, 7
    };

    GLuint ibo_elements;
    glGenBuffers(1, &ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLfloat min_x, max_x,
            min_y, max_y,
            min_z, max_z;

    min_x = max_x = mesh->vertices[0].x;
    min_y = max_y = mesh->vertices[0].y;
    min_z = max_z = mesh->vertices[0].z;

    for (int i = 0; i < mesh->vertices.size(); i++) {
        if (mesh->vertices[i].x < min_x) min_x = mesh->vertices[i].x;
        if (mesh->vertices[i].x > max_x) max_x = mesh->vertices[i].x;
        if (mesh->vertices[i].y < min_y) min_y = mesh->vertices[i].y;
        if (mesh->vertices[i].y > max_y) max_y = mesh->vertices[i].y;
        if (mesh->vertices[i].z < min_z) min_z = mesh->vertices[i].z;
        if (mesh->vertices[i].z > max_z) max_z = mesh->vertices[i].z;
    }

    glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
    glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
    glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

    // Uniform location of the model matrix from shader
    GLint modelLocaction = glGetUniformLocation(shaderID, "Model");

    // Apply the object's transformation matrix
    glm::mat4 model = meshTransform * transform;
    
    glUniformMatrix4fv(modelLocaction, 1, GL_FALSE, glm::value_ptr(model));

    GLint vpos_location;
    vpos_location = glGetAttribLocation(shaderID, "vPosition");

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(
        vpos_location,      // attribute
        3,                  // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
    );

    // Bind and draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vpos_location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Clean up
    glDeleteBuffers(1, &vbo_vertices);
    glDeleteBuffers(1, &ibo_elements);
}