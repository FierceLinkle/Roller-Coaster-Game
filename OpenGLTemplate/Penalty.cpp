#include "Penalty.h" 

CPenalty::CPenalty(glm::vec3 pos)
{
	position = pos;
}
CPenalty::~CPenalty()
{
	Release();
}
void CPenalty::Create(string filename) {
	m_texture.Load(filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create(); m_vbo.Bind();

	// Write the code to add interleaved vertex attributes to the VBO


	// Vertex positions
	glm::vec3 p0 = glm::vec3(-1, -1, -1);
	glm::vec3 p1 = glm::vec3(1, -1, -1);
	glm::vec3 p2 = glm::vec3(-1, 1, -1);
	glm::vec3 p3 = glm::vec3(1, 1, -1);
	glm::vec3 p4 = glm::vec3(-1, -1, 1);
	glm::vec3 p5 = glm::vec3(1, -1, 1);
	glm::vec3 p6 = glm::vec3(-1, 1, 1);
	glm::vec3 p7 = glm::vec3(1, 1, 1);

	m_FrontPoints = { p4, p5, p6, p7 }; //Front
	m_LeftPoints = { p0, p4, p2, p6 }; //Left
	m_RightPoints = { p5, p1, p7, p3 }; //Right
	m_BackPoints = { p1, p0, p3, p2 }; //Back
	m_TopPoints = { p6, p7, p2, p3 }; //Top
	m_BottomPoints = { p4, p5, p0, p1 }; //Bottom

	// Texture coordinates
	glm::vec2 tx1 = glm::vec2(0, 0); //Bottom Left
	glm::vec2 tx2 = glm::vec2(1, 0); //Bottom Right
	glm::vec2 tx3 = glm::vec2(0, 1); //Top Left
	glm::vec2 tx4 = glm::vec2(1, 1); //Top Right

	m_TextCoords = { tx1, tx2, tx3, tx4 };



	// Power up normal
	glm::vec3 n1 = glm::vec3(0, 0, 1); //Front
	glm::vec3 n2 = glm::vec3(-1, 0, 0); //Left
	glm::vec3 n3 = glm::vec3(1, 0, 0); //Right
	glm::vec3 n4 = glm::vec3(0, 0, -1); //Back
	glm::vec3 n5 = glm::vec3(0, 1, 0); //Top
	glm::vec3 n6 = glm::vec3(0, -1, 0); //Bottom

	m_TextNormals = { n1, n2, n3, n4, n5, n6 };


	// Put the vertex attributes in the VBO
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


	// Upload the VBO to the GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	// Upload data to GPU m_vbo.UploadDataToGPU(GL_STATIC_DRAW); GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2); // Vertex positions glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0); // Texture coordinates glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3)); // Normal vectors glEnableVertexAttribArray(2); glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}
void CPenalty::Render() {
	glBindVertexArray(m_vao);
	m_texture.Bind();

	// Call glDrawArrays to render each side
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

}
void CPenalty::Release() {
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}

glm::vec3 CPenalty::GetPosition()
{
	return position;
}