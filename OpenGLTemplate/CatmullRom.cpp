#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk

	/* Original Track
	m_controlPoints.push_back(glm::vec3(100, 5, 0));
	m_controlPoints.push_back(glm::vec3(71, 5, 71));
	m_controlPoints.push_back(glm::vec3(0, 5, 100));
	m_controlPoints.push_back(glm::vec3(-71, 5, 71));
	m_controlPoints.push_back(glm::vec3(-100, 5, 0));
	m_controlPoints.push_back(glm::vec3(-71, 5, -71));
	m_controlPoints.push_back(glm::vec3(0, 5, -100));
	m_controlPoints.push_back(glm::vec3(71, 5, -71));
	*/

	//New Track
	//Start
	m_controlPoints.push_back(glm::vec3(0, 10, 100));

	//First peek (start)
	m_controlPoints.push_back(glm::vec3(-100, 100, 100));

	//First Curve
	m_controlPoints.push_back(glm::vec3(-300, 10, 100));
	m_controlPoints.push_back(glm::vec3(-371, 15, 129));
	m_controlPoints.push_back(glm::vec3(-400, 20, 200));
	m_controlPoints.push_back(glm::vec3(-371, 50, 271));
	m_controlPoints.push_back(glm::vec3(-300, 70, 300));
	m_controlPoints.push_back(glm::vec3(-229, 90, 271));
	m_controlPoints.push_back(glm::vec3(-200, 100, 200));
	m_controlPoints.push_back(glm::vec3(-200, 90, 0));

	//Second Curve
	m_controlPoints.push_back(glm::vec3(-200, 70, -100));
	m_controlPoints.push_back(glm::vec3(-229, 70, -171));
	m_controlPoints.push_back(glm::vec3(-300, 70, -200));
	m_controlPoints.push_back(glm::vec3(-371, 60, -171));
	m_controlPoints.push_back(glm::vec3(-400, 50, -100));
	m_controlPoints.push_back(glm::vec3(-371, 40, -29));
	m_controlPoints.push_back(glm::vec3(-300, 30, 0));

	//First Bend
	m_controlPoints.push_back(glm::vec3(-100, 10, 0));
	m_controlPoints.push_back(glm::vec3(-29, 10, -29));
	m_controlPoints.push_back(glm::vec3(0, 10, -100));

	//Thrid Curve
	m_controlPoints.push_back(glm::vec3(0, 10, -300));
	m_controlPoints.push_back(glm::vec3(29, 10, -371));
	m_controlPoints.push_back(glm::vec3(100, 10, -400));
	m_controlPoints.push_back(glm::vec3(171, 10, -371));
	m_controlPoints.push_back(glm::vec3(200, 10, -300));

	//Fourth Curve
	m_controlPoints.push_back(glm::vec3(200, 10, 200));
	m_controlPoints.push_back(glm::vec3(229, 10, 271));
	m_controlPoints.push_back(glm::vec3(300, 10, 300));
	m_controlPoints.push_back(glm::vec3(371, 10, 271));
	m_controlPoints.push_back(glm::vec3(400, 10, 200));
	m_controlPoints.push_back(glm::vec3(371, 10, 129));
	m_controlPoints.push_back(glm::vec3(300, 10, 100));

	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)

}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int) m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i-1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M-1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int) m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int) (d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;
	
	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j-1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));


	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points

	SetControlPoints();

	// Call UniformlySampleControlPoints with the number of samples required

	UniformlySampleControlPoints(500);

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card

	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);
	// Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < m_centrelinePoints.size(); i++) {
		vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}
	
	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
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


void CCatmullRom::CreateOffsetCurves()
{
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively

	float m_currentDistance = 0.0f;
	glm::vec3 p;
	glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 pNext;
	glm::vec3 y = glm::vec3(0, 1, 0);
	float w = 20.f;
	glm::vec3 l;
	glm::vec3 r;

	Sample(m_currentDistance, p, up);

	for (int i = 0; i < (int)m_centrelinePoints.size(); i++) {
		p = m_centrelinePoints[i];
		if (i != m_centrelinePoints.size() - 1) {
			pNext = m_centrelinePoints[i + 1];
		}
		else {
			pNext = m_centrelinePoints[0];
		}
		glm::vec3 t = glm::normalize(pNext - p);
		glm::vec3 n = glm::normalize(glm::cross(t, y));
		glm::vec3 b = glm::normalize(glm::cross(n, t));
		l = p - (w/2) * n;
		r = p + (w/2) * n;
		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);
	}

	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	// Note it is possible to only use one VAO / VBO with all the points instead.

	//Left offset curve 

	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	// Create a VBO
	CVertexBufferObject vbol;
	vbol.Create();
	vbol.Bind();
	glm::vec2 texCoordl(0.0f, 0.0f);
	glm::vec3 normall(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < m_leftOffsetPoints.size(); i++) {
		vbol.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
		vbol.AddData(&texCoordl, sizeof(glm::vec2));
		vbol.AddData(&normall, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbol.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stridel = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stridel, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stridel, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stridel,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	//right offset curve

	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);
	// Create a VBO
	CVertexBufferObject vbor;
	vbor.Create();
	vbor.Bind();
	glm::vec2 texCoordr(0.0f, 0.0f);
	glm::vec3 normalr(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < m_rightOffsetPoints.size(); i++) {
		vbor.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
		vbor.AddData(&texCoordr, sizeof(glm::vec2));
		vbor.AddData(&normalr, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbor.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei strider = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strider, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strider, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, strider,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}


void CCatmullRom::CreateTrack()
{
	
	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card

	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);
	// Create a VBO
	CVertexBufferObject vbot;
	vbot.Create();
	vbot.Bind();
	//Texture Co-ords
	glm::vec2 tx1 = glm::vec2(0, 0); //Bottom Left
	glm::vec2 tx2 = glm::vec2(0, 1); //Bottom Right
	glm::vec2 tx3 = glm::vec2(1, 0); //Top Left
	glm::vec2 tx4 = glm::vec2(1, 1); //Top Right

	vector<glm::vec2> m_TextCoords = { tx1, tx2, tx3, tx4 };

	glm::vec3 normalt(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < m_centrelinePoints.size(); i++) {
		vbot.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
		vbot.AddData(&m_TextCoords, sizeof(glm::vec2));
		vbot.AddData(&normalt, sizeof(glm::vec3));
		vbot.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
		vbot.AddData(&m_TextCoords, sizeof(glm::vec2));
		vbot.AddData(&normalt, sizeof(glm::vec3));
	}

	vbot.AddData(&m_leftOffsetPoints[0], sizeof(glm::vec3));
	vbot.AddData(&m_TextCoords, sizeof(glm::vec2));
	vbot.AddData(&normalt, sizeof(glm::vec3));
	vbot.AddData(&m_rightOffsetPoints[0], sizeof(glm::vec3));
	vbot.AddData(&m_TextCoords, sizeof(glm::vec2));
	vbot.AddData(&normalt, sizeof(glm::vec3));


	//vbot.AddData(&m_leftOffsetPoints[0], sizeof(glm::vec3));
	//vbot.AddData(&texCoordt, sizeof(glm::vec2));
	//vbot.AddData(&normalt, sizeof(glm::vec3));

	//vbot.AddData(&m_rightOffsetPoints[0], sizeof(glm::vec3));
	//vbot.AddData(&texCoordt, sizeof(glm::vec2));
	//vbot.AddData(&normalt, sizeof(glm::vec3));

	// Upload the VBO to the GPU
	vbot.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
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


void CCatmullRom::RenderCentreline()
{
	int P = m_centrelinePoints.size();

	// Bind the VAO m_vaoCentreline and render it
	glLineWidth(5.0f);
	glBindVertexArray(m_vaoCentreline);
	m_texture.Bind();
	glDrawArrays(GL_LINE_LOOP, 0, 500);
	glDrawArrays(GL_POINTS, 0, 500);
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it

	glLineWidth(5.0f);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	m_texture.Bind();
	glDrawArrays(GL_LINE_LOOP, 0, 500);
	glDrawArrays(GL_POINTS, 0, 500);

	// Bind the VAO m_vaoRightOffsetCurve and render it

	glLineWidth(5.0f);
	glBindVertexArray(m_vaoRightOffsetCurve);
	m_texture.Bind();
	glDrawArrays(GL_LINE_LOOP, 0, 500);
	glDrawArrays(GL_POINTS, 0, 500);
}


void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glLineWidth(5.0f);
	glBindVertexArray(m_vaoTrack);
	m_texture.Bind();

	//glDisable(GL_CULL_FACE);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 1004);
	//glDrawArrays(GL_POINTS, 0, 1000);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

void CCatmullRom::Create(string filename)
{
	m_texture.Load(filename);
	//m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	//m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	CreateCentreline();
	CreateOffsetCurves();
	CreateTrack();
}