#pragma once
#include "Common.h"

class COpenAssetImportMesh;

class CPlayer {
private:
	glm::vec3 m_position;			// The position of the carts's centre of projection
	glm::vec3 m_strafeVector;		// The carts's strafe vector

	float m_speed;					// How fast the cart moves
	float m_currentDistance;
	float m_sidePos;

	COpenAssetImportMesh* m_pShipMesh;


public:
	CPlayer();
	~CPlayer();
	void Create();
	void Update(double dt);
	void TranslateByKeyboard(double dt, float w);
	void Release();
	void Strafe(double);
	COpenAssetImportMesh* GetMesh();
	glm::vec3 GetPosition();
	float GetCurrentDist();


	void IncreaseSpeed(float s);
	void DecreaseSpeed(float s);
	void ResetSpeed(float s);

};