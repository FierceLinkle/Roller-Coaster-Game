#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CCube;
class CStart;
class CPole;
class CPlayer;
class CPowerUp;
class CPenalty;
class CFrameBufferObject;
class CHeightMapTerrain;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();
	//void RenderScene(int pass);

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	COpenAssetImportMesh* m_pFighterMesh;
	COpenAssetImportMesh* m_pBarrierMesh;
	COpenAssetImportMesh* m_pCartMesh;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CCatmullRom *m_pCatmullRom;
	CCube *m_pCube;
	CStart *m_pStart;
	CPole* m_pPole;
	CPlayer* m_pPlayer;
	vector <CPowerUp*> m_pPowerUp;
	vector < CPenalty* > m_pPenalty;
	CFrameBufferObject* m_pFBO;
	CPlane* m_pHUD;
	CHeightMapTerrain* m_pHeightmapTerrain;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;

	int m_CartSpeed = 100;
	int m_Time = 0.f;
	int m_LapTime = 0.f;

	float m_currentDistance;
	float m_cameraRotation;

	float m_t;
	glm::vec3 m_SpaceShipPosition;
	glm::mat4 m_SpaceShipOrientation;

	glm::vec3 m_CartPosition;
	glm::mat4 m_CartOrientation;
	glm::vec3 m_SpotLightPosition;

	//float posTop = 5.f;
	float posSide = 0.f;

	int m_CAngle = 1;

	float sidePos = 0.f;

	float m_pickedUpPower = 0.f;
	float m_pickedUpPenalty = 0.f;

	float m_spin = glm::radians(0.f);


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();


private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void DisplaySpeed();
	void DisplayTime();
	void DisplayHUD();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
	void Collision();
};
