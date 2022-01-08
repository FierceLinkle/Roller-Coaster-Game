#pragma once
#include "Common.h" 
#include "Texture.h" 
#include "VertexBufferObject.h"

// Class for generating a unit cube 
class CPenalty {
public:
	CPenalty(glm::vec3 pos);
	~CPenalty();
	void Create(string filename);
	void Render();
	void Release();
	glm::vec3 GetPosition();

private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	glm::vec3 position;

	glm::vec3 p;
	glm::vec2 s;
	glm::vec3 n;

	vector<glm::vec3> m_FrontPoints;
	vector<glm::vec3> m_LeftPoints;
	vector<glm::vec3> m_RightPoints;
	vector<glm::vec3> m_BackPoints;
	vector<glm::vec3> m_TopPoints;
	vector<glm::vec3> m_BottomPoints;

	vector<glm::vec2> m_TextCoords;

	vector<glm::vec3> m_TextNormals;
};
