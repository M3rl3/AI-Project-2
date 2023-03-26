#include "cVAOManager.h"
#include "../OpenGL.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include <sstream>

sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = 0;	// or NULL
	this->pIndices = 0;		// or NULL

	// You could store the max and min values of the 
	//  vertices here (determined when you load them):
	glm::vec3 maxValues;
	glm::vec3 minValues;

//	scale = 5.0/maxExtent;		-> 5x5x5
	float maxExtent;

	return;
}

void sModelDrawInfo::CalculateExtents(void)
{
	// Do we even have an array?
	if (this->pVertices)		// same as != NULL
	{
		// Assume that the 1st vertex is both the min and max
		this->minX = this->maxX = this->pVertices[0].x;
		this->minY = this->maxY = this->pVertices[0].y;
		this->minZ = this->maxZ = this->pVertices[0].z;

		for (unsigned int index = 0; index != this->numberOfVertices; index++)
		{
			// See if "this" vertex is smaller than the min
			if (this->pVertices[index].x < this->minX) { this->minX = this->pVertices[index].x; }
			if (this->pVertices[index].y < this->minY) { this->minY = this->pVertices[index].y; }
			if (this->pVertices[index].z < this->minZ) { this->minZ = this->pVertices[index].z; }

			// See if "this" vertex is larger than the max
			if (this->pVertices[index].x > this->maxX) { this->maxX = this->pVertices[index].x; }
			if (this->pVertices[index].y > this->maxY) { this->maxY = this->pVertices[index].y; }
			if (this->pVertices[index].z > this->maxZ) { this->maxZ = this->pVertices[index].z; }
		}//for (unsigned int index = 0...
	}//if ( this->pVertices )

	// Update the extents
	this->extentX = this->maxX - this->minX;
	this->extentY = this->maxY - this->minY;
	this->extentZ = this->maxZ - this->minZ;

	// What's the largest of the three extents
	this->maxExtent = this->extentX;
	if (this->extentY > this->maxExtent) { this->maxExtent = this->extentY; }
	if (this->extentZ > this->maxExtent) { this->maxExtent = this->extentZ; }

	return;
}

bool cVAOManager::LoadModelIntoVAO(
		std::string fileName, 
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID)

{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)

	drawInfo.meshName = fileName;

	// Calculate the min and max values
	drawInfo.CalculateExtents();

	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(vertLayout) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
				  GL_STATIC_DRAW );
	// glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Set the vertex attributes.

	GLint vPositionLocation = glGetAttribLocation(shaderProgramID, "vPosition");	
	GLint vColourLocation = glGetAttribLocation(shaderProgramID, "vColour");
	GLint vNormalLocation = glGetAttribLocation(shaderProgramID, "vNormal");
	GLint vUV2Location = glGetAttribLocation(shaderProgramID, "vUV2");

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vPositionLocation);	// vPos
	glVertexAttribPointer(vPositionLocation, 3,			// vPos
						    GL_FLOAT, GL_FALSE,
						    sizeof(vertLayout),
						    (void*)offsetof(vertLayout, x));

	glEnableVertexAttribArray(vColourLocation);		// vCol
	glVertexAttribPointer(vColourLocation, 3,			// vCol
						    GL_FLOAT, GL_FALSE,
						    sizeof(vertLayout),
						    (void*)offsetof(vertLayout, r));

	glEnableVertexAttribArray(vNormalLocation);		// vNormal
	glVertexAttribPointer(vNormalLocation, 3,			// vNormal
							GL_FLOAT, GL_FALSE,
							sizeof(vertLayout),
							(void*)offsetof(vertLayout, nx));

	glEnableVertexAttribArray(vUV2Location);		// vUV2
	glVertexAttribPointer(vUV2Location, 4,				// vUV2
						    GL_FLOAT, GL_FALSE,
							sizeof(vertLayout),						// Stride	(number of bytes)
							(void*)offsetof(vertLayout, texture_u));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vPositionLocation);
	glDisableVertexAttribArray(vColourLocation);
	glDisableVertexAttribArray(vNormalLocation);
	glDisableVertexAttribArray(vUV2Location);


	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;


	return true;
}


// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}

