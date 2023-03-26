#ifndef _FBO_HG_
#define _FBO_HG_

#include "../OpenGL.h"

#include <string>

class cFBO
{
public:
	cFBO();
	~cFBO();

	GLuint ID;						// = 0;
	GLuint colourTexture_0_ID;		// = 0;

	// For depth of field, we are storing the vertex world coordinates
	GLuint vertexWorldPosition_ID;

//	GLuint TBDTexture_1_ID;
//	GLuint TBDTexture_2_ID;

	GLuint depthTexture_ID;		// = 0;
	GLint width;		// = 512 the WIDTH of the framebuffer, in pixels;
	GLint height;

	// Inits the FBP
	bool init(int width, int height, std::string &error);
	bool shutdown(void);
	// Calls shutdown(), then init()
	bool reset(int width, int height, std::string &error);
	
	void clearBuffers(bool bClearColour = true, bool bClearDepth = true);

	void clearColourBuffer(int bufferindex);
	void clearAllColourBuffers(void);
	void clearDepthBuffer(void);
	void clearStencilBuffer( int clearColour, int mask = 0xFF );

	int getMaxColourAttachments(void);
	int getMaxDrawBuffers(void);
};

#endif
