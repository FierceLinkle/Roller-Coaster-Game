#include "Pole.h"


CPole::CPole()
{}

CPole::~CPole()
{
	Release();
}

void CPole::Create(string filename)
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
	
	//Create a vertex and texture co-ords point for each point in the circle at the top and bottom
	for (int i = 0; i <= 360; i++)
	{
		const float ctxt = (i / (float)(2 * M_PI));
		texture_Coords.push_back(glm::vec2(ctxt, 0.0f));
		cylinder_points.push_back(glm::vec3(cos(M_PI / 180 * i), -1, sin(M_PI / 180 * i)));
		texture_Coords.push_back(glm::vec2(ctxt, 1.0f));
		cylinder_points.push_back(glm::vec3(cos(M_PI / 180 * i), +50, sin(M_PI / 180 * i)));
	}

	int cp = cylinder_points.size();

	//Add them to the vbo
	for (int i = 0; i < cp; ++i)
	{

		m_vbo.AddData(&cylinder_points[i], sizeof(glm::vec3));
		m_vbo.AddData(&texture_Coords[i], sizeof(glm::vec2));
		glm::vec3 normalTmp = glm::normalize(cylinder_points[i] - glm::vec3(0, cylinder_points[i].y, 0));
		m_vbo.AddData(&normalTmp, sizeof(glm::vec3));
	}

	//Old code
	/*
	//Face texture co-rds
	glm::vec2 tx1 = glm::vec2(0, 0); //Bottom Left
	glm::vec2 tx2 = glm::vec2(1, 0); //Bottom Right
	glm::vec2 tx3 = glm::vec2(0, 1); //Top Left
	glm::vec2 tx4 = glm::vec2(1, 1); //Top Right

	//Top and bottom co-rds;
	glm::vec2 tx5 = glm::vec2(-0.5, -1); //Bottom Left
	glm::vec2 tx6 = glm::vec2(0.5, -1); //Bottom Right
	glm::vec2 tx7 = glm::vec2(1, 0); //Top Left
	glm::vec2 tx8 = glm::vec2(-1, 0); //Top Right
	glm::vec2 tx9 = glm::vec2(-0.5, 1); //Bottom Left
	glm::vec2 tx10 = glm::vec2(0.5, 1); //Bottom Right


	m_TextTBCoords = { tx5, tx6, tx7 , tx8, tx9, tx10, tx10 };
	m_TextCoords = { tx1, tx2, tx3, tx4 };

	//Normals
	glm::vec3 n1 = glm::vec3(0, 1, 0); //Top
	glm::vec3 n2 = glm::vec3(0, -1, 0); //Bottom
	glm::vec3 n3 = glm::vec3(0, 0, -1); //Face 1
	glm::vec3 n4 = glm::vec3(-1, 0, -1); //Face 2
	glm::vec3 n5 = glm::vec3(-1, 0, 1); //Face 3
	glm::vec3 n6 = glm::vec3(0, 0, 1); //Face 4
	glm::vec3 n7 = glm::vec3(1, 0, 1); //Face 5
	glm::vec3 n8 = glm::vec3(1, 0, -1); //Face 6

	m_TextNormals = { n1, n2, n3, n4, n5, n6, n7, n8 };
	
	//Vertex Positions
	//Top part
	glm::vec3 p0 = glm::vec3(-0.5, 50, 1); //bottom left
	glm::vec3 p1 = glm::vec3(0.5, 50, 1); //bottom right
	glm::vec3 p2 = glm::vec3(1, 50, 0); //middle right
	glm::vec3 p3 = glm::vec3(0.5, 50, -1); //top right
	glm::vec3 p4 = glm::vec3(-0.5, 50, -1); //top left
	glm::vec3 p5 = glm::vec3(-1, 50, 0); //middle left

	//Bottom part
	glm::vec3 p6 = glm::vec3(-0.5, 0, 1); //bottom left
	glm::vec3 p7 = glm::vec3(0.5, 0, 1); //bottom right
	glm::vec3 p8 = glm::vec3(1, 0, 0); //middle right
	glm::vec3 p9 = glm::vec3(0.5, 0, -1); //top right
	glm::vec3 p10 = glm::vec3(-0.5, 0, -1); //top left
	glm::vec3 p11 = glm::vec3(-1, 0, 0); //middle left

	m_TopPoints = { p0, p1, p2, p5, p3, p4, p5 }; //Bottom
	m_BottomPoints = { p6, p7, p8, p11, p9, p10, p11 }; //Top

	//cylinder faces
	m_Face1 = { p6, p7, p0, p1 }; 
	m_Face2 = { p7, p8, p1, p2 };
	m_Face3 = { p8, p9, p2, p3 };
	m_Face4 = { p9, p10, p3, p4 };
	m_Face5 = { p10, p11, p4, p5 };
	m_Face6 = { p11, p6, p5, p0 }; 

	
	//Top
	for (int i = 0; i < 7; i++) {
		m_vbo.AddData(&m_TopPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextTBCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[0], sizeof(glm::vec3));
	}

	//Bottom
	for (int i = 0; i < 7; i++) {
		m_vbo.AddData(&m_BottomPoints[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextTBCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[1], sizeof(glm::vec3));
	}

	//Face 1
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_Face1[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[2], sizeof(glm::vec3));
	}

	//Face 2
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_Face2[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[3], sizeof(glm::vec3));
	}

	//Face 3
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_Face3[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[4], sizeof(glm::vec3));
	}

	//Face 4
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_Face4[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[5], sizeof(glm::vec3));
	}

	//Face 5
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_Face5[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[6], sizeof(glm::vec3));
	}

	//Face 6
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&m_Face6[i], sizeof(glm::vec3));
		m_vbo.AddData(&m_TextCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_TextNormals[7], sizeof(glm::vec3));
	}

	*/
	
	
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

void CPole::Render()
{
	int cp = cylinder_points.size();

	glBindVertexArray(m_vao);
	m_texture.Bind();
	// Call glDrawArrays to render each side
	glDrawArrays(GL_TRIANGLE_STRIP, 0, cp);
}
void CPole::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}