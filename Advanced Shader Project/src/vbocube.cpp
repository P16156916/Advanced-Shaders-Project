#include "vbocube.h"

VBOCube::VBOCube(float p_fVert, unsigned int shaderID)
{
	float v[] = {
		-p_fVert,-p_fVert,-p_fVert, //V0
		-p_fVert,p_fVert,-p_fVert, //V1
		p_fVert,p_fVert,-p_fVert, //V2
		p_fVert,-p_fVert,-p_fVert, //V3

		-p_fVert,-p_fVert,p_fVert, //V4
		-p_fVert,p_fVert,p_fVert, //V5
		p_fVert,p_fVert,p_fVert, //V6
		p_fVert,-p_fVert,p_fVert, //V7
	};

	unsigned int l_iIndex[] =
	{
		4,5,6,
		4,6,7, //Front Square

		0,1,2,
		0,2,3, // Back Square

		1,5,6,
		1,6,2, // Top Square

		0,4,7,
		0,7,3, // Bottom Square

		3,2,6,
		3,6,7, // Left Square

		4,5,1,
		4,1,0 // Right Square
	};

	GLuint handle[2];
	glGenBuffers(2, handle);  //Generate handles as buffers.

	glGenVertexArrays(1, &m_ivaoHandle);
	glBindVertexArray(m_ivaoHandle); //Generate vertex array and store in ivaoHandle

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]); //Being used as an array buffer, and is currently active
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW); //storing vertices in handle[0], at size of v

	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, FALSE, 0, ((GLubyte*)NULL + (0))); //setting up the 3 points for each vertex as a float
	glEnableVertexAttribArray(0); //enabling the vertex array, becomes active.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[1]);//Being used as an element buffer, and is currently active
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(l_iIndex), l_iIndex, GL_STATIC_DRAW);//storing indices in handle[1], at size of l_iIndex

	glBindVertexArray(0); //unbind the vertex array

	// generated the vertices the VAO and the indices for the faces.

	glGenTextures(1, &m_glTextureID); //assigning textures to glTextureID
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_glTextureID); //binding the textures as a cube map specifically

	GLuint CubeMapFaces[6] = //assigning cube face indices.
	{
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, //Right
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, //Left
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, //Top
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, //Bottom
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, //Back
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, //Front
	};

	//For everyface of the cubemap
	for (GLuint iCount = 0; iCount < 6; iCount++) //load in textures 0-5 for cube map
	{
		string l_sTextureDir = "resources/images/cubemap/cubemap" + std::to_string(iCount) + ".jpg";
		Bitmap bmp = Bitmap::bitmapFromFile(l_sTextureDir);
		glTexImage2D(CubeMapFaces[iCount],
			0,
			GL_RGB,
			bmp.width(),
			bmp.height(),
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bmp.pixelBuffer());
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); //setting the rendering parameters, stops overflowing textures from the cube

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_glTextureID); //activating the textures as a cube map, and assigning to texture0

	GLint loc = glGetUniformLocation(shaderID, "cubeMap");
	glUniform1i(loc, 0); //Cube map needs to look at the texture in slot 0, told to shader
}

void VBOCube::render() const
{
	glBindVertexArray(m_ivaoHandle); //binds vao to be drawn
	//Draws the contents of teh VAO as triangles using the indices
	glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, ((GLubyte*)NULL + (0)));

}

GLuint VBOCube::getID()
{
	return m_glTextureID;
}




