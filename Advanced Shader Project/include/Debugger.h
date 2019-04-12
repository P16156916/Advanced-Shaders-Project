#pragma once
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H 
#include "Shader.h"

struct Character {
	GLuint textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLuint advance;
};

struct Message {
	std::string message; //!< Message body string
	float yPosition; //!< Message Y axis position on screen
	float xPosition; //!< Message Y axis position on screen
};

class Debugger
{
private:
	FT_Library m_ft;
	FT_Face m_face;
	Shader ourShaderText;

	const float mc_fontHeight = 100.f; //!< The height of the font
	const float mc_textHeight = 25.f; //!< The physical height of the text itself
	const float mc_scale = mc_textHeight / mc_fontHeight; //!< Calculated text scalar

	glm::vec3 mc_color = glm::vec3(1.0f, 1.0f, 1.0f);

	GLuint m_VAO, m_VBO;
	glm::mat4 m_projection;
	std::map<GLchar, Character> m_mCharacters;

public:
	Debugger();
	void drawMessage(Message message, float p_fXPosition);
};