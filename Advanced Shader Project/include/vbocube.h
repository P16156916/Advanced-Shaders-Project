#pragma once
#include "glslprogram.h"
#include <GLFW/glfw3.h>
#include "Bitmap.h"

class VBOCube
{
private:
	unsigned int m_ivaoHandle;
	int m_iFaces;
	GLuint m_glTextureID;
public:
	VBOCube(float p_fVert, unsigned int shaderID);
	void render() const;

	GLuint getID();
};