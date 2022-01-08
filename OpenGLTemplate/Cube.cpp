
#include "Cube.h"

CCube::CCube()
{}

CCube::~CCube()
{
	Release();
}

void CCube::Create(string filename)
{
	m_texture.Load(filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();

	// Write the code to add interleaved vertex attributes to the VBO

	//Texture Co-ords
	glm::vec2 tx1 = glm::vec2(0, 0); //Bottom Left
	glm::vec2 tx2 = glm::vec2(1, 0); //Bottom Right
	glm::vec2 tx3 = glm::vec2(0, 1); //Top Left
	glm::vec2 tx4 = glm::vec2(1, 1); //Top Right
	glm::vec2 tx5 = glm::vec2(0.5, 1); //Top Middle

	m_TextCoords = { tx1, tx2, tx3, tx4 };
	m_FRoofCoords = { tx1, tx2, tx5 };

	//Normals
	glm::vec3 n1 = glm::vec3(0, 0, 1); //Front
	glm::vec3 n2 = glm::vec3(-1, 0, 0); //Left
	glm::vec3 n3 = glm::vec3(1, 0, 0); //Right
	glm::vec3 n4 = glm::vec3(0, 0, -1); //Back
	glm::vec3 n5 = glm::vec3(0, 1, 0); //Top
	glm::vec3 n6 = glm::vec3(0, -1, 0); //Bottom
	glm::vec3 n7 = glm::vec3(-1, 1, 0); //Left Roof
	glm::vec3 n8 = glm::vec3(1, 1, 0); //Right Roof

	m_TextNormals = {n1, n2, n3, n4, n5, n6, n7, n8};


	//Vertex Positions
	glm::vec3 p0 = glm::vec3(-1, -1, -1);
	glm::vec3 p1 = glm::vec3(1, -1, -1);
	glm::vec3 p2 = glm::vec3(-1, 1, -1);
	glm::vec3 p3 = glm::vec3(1, 1, -1);
	glm::vec3 p4 = glm::vec3(-1, -1, 1);
	glm::vec3 p5 = glm::vec3(1, -1, 1);
	glm::vec3 p6 = glm::vec3(-1, 1, 1);
	glm::vec3 p7 = glm::vec3(1, 1, 1);

	//Roof Vertex Positions
	glm::vec3 p8 = glm::vec3(0, 2, 1);
	glm::vec3 p9 = glm::vec3(0, 2, -1);

	m_FrontPoints = { p4, p5, p6, p7 }; //Front
	m_LeftPoints = { p0, p4, p2, p6 }; //Left
	m_RightPoints = { p5, p1, p7, p3 }; //Right
	m_BackPoints = { p1, p0, p3, p2 }; //Back
	m_TopPoints = { p6, p7, p2, p3 }; //Top
	m_BottomPoints = { p4, p5, p0, p1 }; //Bottom

	m_FRoofPoints = { p6, p7, p8 }; //Front Roof
	m_LRoofPoints = { p2, p6, p9, p8 }; //Left Roof
	m_RRoofPoints = { p7, p3, p8, p9 }; //Right Roof
	m_BRoofPoints = { p3, p2, p9 }; //Back Roof

	//Front
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_FrontPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[0], sizeof(glm::vec3));
	}

	//Left
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_LeftPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[1], sizeof(glm::vec3));
	}

	//Right
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_RightPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[2], sizeof(glm::vec3));
	}

	//Back
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_BackPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[3], sizeof(glm::vec3));
	}

	//Top
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_TopPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[4], sizeof(glm::vec3));
	}

	//Bottom
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_BottomPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[5], sizeof(glm::vec3));
	}

	//Front Roof
	for (int i = 0; i < 3; i++) {
		m_vbo.AddData(&m_FRoofPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_FRoofCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[0], sizeof(glm::vec3));
	}

	//Left Roof
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_LRoofPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[6], sizeof(glm::vec3));
	}

	//Right Roof
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_RRoofPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[7], sizeof(glm::vec3));
	}

	//Back Roof
	for (int i = 0; i < 3; i++) {
		m_vbo.AddData(&m_BRoofPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_FRoofCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[3], sizeof(glm::vec3));
	}

	// Upload data to GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CCube::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	// Call glDrawArrays to render each side
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 24, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 27, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 31, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 35, 3);
}

void CCube::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}