#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"


// Class for generating a pole
class CPole
{
public:
	CPole();
	~CPole();
	void Create(string filename);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;

	vector<glm::vec3> cylinder_points;
	vector<glm::vec2> texture_Coords;

	glm::vec3 p;
	glm::vec2 s;
	glm::vec3 n;

	vector<glm::vec3> m_TopPoints;
	vector<glm::vec3> m_BottomPoints;
	vector<glm::vec3> m_Face1;
	vector<glm::vec3> m_Face2;
	vector<glm::vec3> m_Face3;
	vector<glm::vec3> m_Face4;
	vector<glm::vec3> m_Face5;
	vector<glm::vec3> m_Face6;

	vector<glm::vec2> m_TextTBCoords;
	vector<glm::vec2> m_TextCoords;

	vector<glm::vec3> m_TextNormals;
};
