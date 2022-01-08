#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"


// Class for generating the start of the roller coaster
class CStart
{
public:
	CStart();
	~CStart();
	void Create(string filename);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;

	glm::vec3 p;
	glm::vec2 s;
	glm::vec3 n;

	vector<glm::vec3> m_FrontPoints;
	vector<glm::vec3> m_LeftPoints;
	vector<glm::vec3> m_RightPoints;
	vector<glm::vec3> m_BackPoints;
	vector<glm::vec3> m_TopPoints;
	vector<glm::vec3> m_BottomPoints;

	vector<glm::vec3> m_FrontPoints2;
	vector<glm::vec3> m_LeftPoints2;
	vector<glm::vec3> m_RightPoints2;
	vector<glm::vec3> m_BackPoints2;
	vector<glm::vec3> m_TopPoints2;
	vector<glm::vec3> m_BottomPoints2;

	vector<glm::vec3> m_FrontPoints3;
	vector<glm::vec3> m_LeftPoints3;
	vector<glm::vec3> m_RightPoints3;
	vector<glm::vec3> m_BackPoints3;
	vector<glm::vec3> m_TopPoints3;
	vector<glm::vec3> m_BottomPoints3;

	vector<glm::vec2> m_TextCoords;

	vector<glm::vec3> m_TextNormals;
};