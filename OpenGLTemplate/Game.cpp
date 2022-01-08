/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)
*/


#include "game.h"
#include "CatmullRom.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "Cube.h"
#include "Start.h"
#include "Pole.h"
#include "Player.h"
#include "PowerUp.h"
#include "Penalty.h"
#include "HeightMapTerrain.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_pFighterMesh = NULL;
	m_pBarrierMesh = NULL;
	m_pCartMesh = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;
	m_pCube = NULL;
	m_pStart = NULL;
	m_pPole = NULL;
	m_pPlayer = NULL;
	m_pHUD = NULL;
	m_pHeightmapTerrain = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_pFighterMesh;
	delete m_pBarrierMesh;
	delete m_pCartMesh;
	delete m_pSphere;
	delete m_pAudio;
	delete m_pCatmullRom;
	delete m_pCube;
	delete m_pStart;
	delete m_pPole;
	delete m_pPlayer;
	delete m_pHUD;
	delete m_pHeightmapTerrain;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pPlayer = new CPlayer;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pHUD = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_pFighterMesh = new COpenAssetImportMesh;
	m_pBarrierMesh = new COpenAssetImportMesh;
	m_pCartMesh = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;
	m_pCube = new CCube;
	m_pStart = new CStart;
	m_pPole = new CPole;
	m_pHeightmapTerrain = new CHeightMapTerrain;

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("toonShader.vert");
	sShaderFileNames.push_back("toonShader.frag");
	sShaderFileNames.push_back("toon2Shader.vert");
	sShaderFileNames.push_back("toon2Shader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	//Create a toon shader
	CShaderProgram* pToonProgram = new CShaderProgram;
	pToonProgram->CreateProgram();
	pToonProgram->AddShaderToProgram(&shShaders[4]);
	pToonProgram->AddShaderToProgram(&shShaders[5]);
	pToonProgram->LinkProgram();
	m_pShaderPrograms->push_back(pToonProgram);

	//Create a second toon shader
	CShaderProgram* pToon2Program = new CShaderProgram;
	pToon2Program->CreateProgram();
	pToon2Program->AddShaderToProgram(&shShaders[6]);
	pToon2Program->AddShaderToProgram(&shShaders[7]);
	pToon2Program->LinkProgram();
	m_pShaderPrograms->push_back(pToon2Program);



	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "grassfloor01.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	// Create the plane for the HUD
	//m_pHUD->Create("resources\\textures\\", "Smiley.jpg", 40.0f, 30.0f, 1.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013


	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	m_pFighterMesh->Load("resources\\models\\Fighter\\fighter1.obj"); // Downloaded from https://www.dropbox.com/sh/xd8i6p8eltuqrlk/AAA1DLBW-YaQlkYng7IKv2xSa?dl=0
	m_pBarrierMesh->Load("resources\\models\\Barrier\\Barrier.obj");
	//m_pCartMesh->Load("resources\\models\\Cart\\Cart.obj");

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	//m_pAudio->PlayMusicStream();

	//glm::vec3 p0 = glm::vec3(-500, 10, -200);
	//glm::vec3 p1 = glm::vec3(0, 10, -200);
	//glm::vec3 p2 = glm::vec3(0, 10, 200);
	//glm::vec3 p3 = glm::vec3(-500, 10, 200);
	//m_pCatmullRom->CreatePath(p0, p1, p2, p3);
	
	//m_pCatmullRom->CreateCentreline();
	//m_pCatmullRom->CreateOffsetCurves();
	//m_pCatmullRom->CreateTrack();

	m_pCatmullRom->Create("resources\\textures\\blue.png");

	//m_t = 0;
	m_SpaceShipPosition = glm::vec3(0, 0, 0);
	m_SpaceShipOrientation = glm::mat4(0);

	m_currentDistance = 0.0f;
	m_cameraRotation = 0.0f;

	m_pCube->Create("resources\\textures\\Smiley.jpg");

	m_pStart->Create("resources\\textures\\Solid_black.png");

	m_pPole->Create("resources\\textures\\Gray.jpg");

	m_pPlayer->Create();

	m_pPowerUp.push_back(new CPowerUp(glm::vec3(-100.f, 105.f, 100.f)));
	m_pPowerUp.push_back(new CPowerUp(glm::vec3(-300.f, 75.f, -210.f)));
	m_pPowerUp.push_back(new CPowerUp(glm::vec3(-240.f, 30.f, 5.f)));
	m_pPowerUp.push_back(new CPowerUp(glm::vec3(200.f, 15.f, 100.f)));

	for (int i = 0; i < m_pPowerUp.size(); i++) {
		m_pPowerUp[i]->Create("resources\\textures\\Smiley.jpg");
		glEnable(GL_CULL_FACE);
	}

	m_pPenalty.push_back(new CPenalty(glm::vec3(-400.f, 25.f, 170.f)));
	m_pPenalty.push_back(new CPenalty(glm::vec3(-190.f, 100.f, 50.f)));
	m_pPenalty.push_back(new CPenalty(glm::vec3(-210.f, 95.f, 0.f)));
	m_pPenalty.push_back(new CPenalty(glm::vec3(5.f, 15.f, -190.f)));
	m_pPenalty.push_back(new CPenalty(glm::vec3(210.f, 15.f, -200.f)));
	m_pPenalty.push_back(new CPenalty(glm::vec3(205.f, 15.f, -100.f)));
	m_pPenalty.push_back(new CPenalty(glm::vec3(190.f, 15.f, -0.f)));

	for (int i = 0; i < m_pPenalty.size(); i++) {
		m_pPenalty[i]->Create("resources\\textures\\Smiley.jpg");
		glEnable(GL_CULL_FACE);
	}

	// Create the heightmap terrain
	//m_pHeightmapTerrain->Create("resources\\textures\\terrainHeightMap200.bmp", "resources\\textures\\GrassBright.bmp", glm::vec3(0, 0, 0), 500.0f, 500.0f, 20.5f);
}

// Render method runs repeatedly in a loop
void Game::Render() 
{
	
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 200, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	
	
	glm::vec4 lightPosition2 = glm::vec4(m_CartPosition, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("spotlight1.position", viewMatrix * lightPosition2); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("spotlight1.La", glm::vec3(0.0f));		
	pMainProgram->SetUniform("spotlight1.Ld", glm::vec3(1.0f));		
	pMainProgram->SetUniform("spotlight1.Ls", glm::vec3(1.0f));		
	pMainProgram->SetUniform("spotlight1.direction", m_CartOrientation);	
	pMainProgram->SetUniform("spotlight1.exponent", glm::vec3(20.0f));	
	pMainProgram->SetUniform("spotlight1.cutoff", glm::vec3(30.0f));	

	


	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(cubeMapTextureUnit);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();

	/*
	// Render the new terrain
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pHeightmapTerrain->Render();
	modelViewMatrixStack.Pop();
	*/


	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	

	/*
	// Render the horse 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();


	
	// Render the barrel 
	for (int i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(100.0f + (i * 10), 0.0f, 0.0f));
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
		modelViewMatrixStack.Pop();
	}
	

	// Render the sphere
	for (int i = 1; i < 4; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f + (i * 60), 2.0f * (i * 3), 150.0f));
		modelViewMatrixStack.Scale(2.0f * (i * 3));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		//pMainProgram->SetUniform("bUseTexture", false);
		m_pSphere->Render();
		modelViewMatrixStack.Pop();
	}
	*/

	/*
	//Render to the Fighter
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(m_SpaceShipPosition);
	modelViewMatrixStack *= m_SpaceShipOrientation;
	//modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
	//modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f));
	modelViewMatrixStack.Scale(1.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pFighterMesh->Render();
	modelViewMatrixStack.Pop();
	*/

	//Render the house
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(10.0f, 5.0f, 0.0f));
	modelViewMatrixStack.Scale(2.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCube->Render();
	modelViewMatrixStack.Pop();


	//Render Start
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(100.0f, 6.0f, 100.0f));
	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
	modelViewMatrixStack.Scale(10.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pStart->Render();
	modelViewMatrixStack.Pop();

	//Render Pole 1
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-100.0f, 0.0f, 100.0f));
	modelViewMatrixStack.Scale(2.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPole->Render();
	modelViewMatrixStack.Pop();

	//Render Pole 2
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-300.0f, 0.0f, 300.0f));
	modelViewMatrixStack.Scale(1.4f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPole->Render();
	modelViewMatrixStack.Pop();

	//Render Pole 3
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-200.0f, 0.0f, 200.0f));
	modelViewMatrixStack.Scale(2.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPole->Render();
	modelViewMatrixStack.Pop();

	//Render Pole 4
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-200.0f, 0.0f, 50.0f));
	modelViewMatrixStack.Scale(1.85f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPole->Render();
	modelViewMatrixStack.Pop();

	//Render Pole 5
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-200.0f, 0.0f, -100.0f));
	modelViewMatrixStack.Scale(1.4f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPole->Render();
	modelViewMatrixStack.Pop();

	//Render Pole 6
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-300.0f, 0.0f, -200.0f));
	modelViewMatrixStack.Scale(1.4f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPole->Render();
	modelViewMatrixStack.Pop();

	//Render the Barrier
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(glm::vec3(70.0f + (i * 30), 0.0f, 150.0f + (j * 20)));
			modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
			modelViewMatrixStack.Scale(1.5f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pBarrierMesh->Render();
			modelViewMatrixStack.Pop();
		}
	}

	//Render the Cart
	modelViewMatrixStack.Push();
	//modelViewMatrixStack.Translate(glm::vec3(100.0f, 0.0f, 0.0f));
	//modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
	modelViewMatrixStack.Translate(m_CartPosition);
	modelViewMatrixStack *= m_CartOrientation;
	modelViewMatrixStack.Scale(3.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPlayer->GetMesh()->Render();
	modelViewMatrixStack.Pop();


	//Render the Catmull Spline
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCatmullRom->RenderCentreline();
	modelViewMatrixStack.Pop();

	//Render offset curves
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCatmullRom->RenderOffsetCurves();
	modelViewMatrixStack.Pop();

	//Render Track
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCatmullRom->RenderTrack();
	modelViewMatrixStack.Pop();

	// Switch to the toon program
	CShaderProgram* pToonProgram = (*m_pShaderPrograms)[2];
	pToonProgram->UseProgram();

	// Set light and materials in toon programme
	pToonProgram->SetUniform("material1.Ma", glm::vec3(0.0f, 1.0f, 0.0f));
	pToonProgram->SetUniform("material1.Md", glm::vec3(0.0f, 1.0f, 0.0f));
	pToonProgram->SetUniform("material1.Ms", glm::vec3(1.0f, 1.0f, 1.0f));
	pToonProgram->SetUniform("material1.shininess", 50.0f);
	pToonProgram->SetUniform("light1.La", glm::vec3(0.15f, 0.15f, 0.15f));
	pToonProgram->SetUniform("light1.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
	pToonProgram->SetUniform("light1.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pToonProgram->SetUniform("light1.position", viewMatrix * lightPosition2);
	pToonProgram->SetUniform("t", m_t);
	pToonProgram->SetUniform("pickUp", m_pickedUpPower);

	
	// Render the power up
	for (unsigned int i = 0; i < m_pPowerUp.size(); i++)
	{
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_pPowerUp[i]->GetPosition());
		//modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, -1.0f), 125);
		modelViewMatrixStack.Rotate(glm::vec3(0.f, 1.f, 0.f), m_spin);
		modelViewMatrixStack.Scale(2.0f);
		pToonProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pToonProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pToonProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPowerUp[i]->Render();
		modelViewMatrixStack.Pop();
	}

	// Switch to the second toon program
	CShaderProgram* pToon2Program = (*m_pShaderPrograms)[3];
	pToon2Program->UseProgram();

	// Set light and materials in toon2 programme
	pToon2Program->SetUniform("material1.Ma", glm::vec3(0.0f, 1.0f, 0.0f));
	pToon2Program->SetUniform("material1.Md", glm::vec3(0.0f, 1.0f, 0.0f));
	pToon2Program->SetUniform("material1.Ms", glm::vec3(1.0f, 1.0f, 1.0f));
	pToon2Program->SetUniform("material1.shininess", 50.0f);
	pToon2Program->SetUniform("light1.La", glm::vec3(0.15f, 0.15f, 0.15f));
	pToon2Program->SetUniform("light1.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
	pToon2Program->SetUniform("light1.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pToon2Program->SetUniform("light1.position", viewMatrix * lightPosition2);
	pToon2Program->SetUniform("t", m_t);
	pToon2Program->SetUniform("pickUp", m_pickedUpPenalty);

	// Render the penalty
	for (unsigned int i = 0; i < m_pPenalty.size(); ++i)
	{
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_pPenalty[i]->GetPosition());
		//modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, -1.0f), 125);
		modelViewMatrixStack.Rotate(glm::vec3(0.f, 1.f, 0.f), m_spin);
		modelViewMatrixStack.Scale(2.0f);
		pToon2Program->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pToon2Program->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pToon2Program->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPenalty[i]->Render();
		modelViewMatrixStack.Pop();
	}

	/* 
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", false); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCatmullRom->RenderPath();
	// Render your object here
	modelViewMatrixStack.Pop();

	*/
		
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	//HUD
	DisplaySpeed();
	DisplayTime();
	//DisplayHUD();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		
}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	//glm::vec3 cPos = glm::vec3(0, 300, 0);
	//glm::vec3 cView = glm::vec3(0, 0, 0);
	//glm::vec3 cUp = glm::vec3(0, 1, 0);
	//m_pCamera->Set(cPos, cView, cUp);

	//m_pCamera->Update(m_dt);


	/* 
	static float t = 0.0f;
	t += 0.0005f * (float)m_dt;
	if (t > 1.0f) {
		t = 0.0f;
	}

	CCatmullRom cr;
	glm::vec3 p0 = glm::vec3(-500, 10, -200);
	glm::vec3 p1 = glm::vec3(0, 10, -200);
	glm::vec3 p2 = glm::vec3(0, 10, 200);
	glm::vec3 p3 = glm::vec3(-500, 10, 200);

	glm::vec3 x = cr.Interpolate(p0, p1, p2, p3, t);

	m_pCamera->Set(x, cView, cUp);

	*/

	//Spaceship
	m_t += 0.001f * (float)m_dt;
	float r = 75.0f;
	glm::vec3 x = glm::vec3(1, 0, 0);
	glm::vec3 y = glm::vec3(0, 1, 0);
	glm::vec3 z = glm::vec3(0, 0, 1);
	m_SpaceShipPosition = r * cos(m_t) * x + 50.0f * y + r * sin(m_t) * z;

	glm::vec3 T = glm::normalize(-r * sin(m_t) * x + r * cos(m_t) * z);
	glm::vec3 N = glm::normalize(glm::cross(T, y));
	glm::vec3 B = glm::normalize(glm::cross(N, T));
	m_SpaceShipOrientation = glm::mat4(glm::mat3(T, B, N));



	//Catmull Path
	m_currentDistance += 0.1f * m_pCamera->GetSpeed();
	glm::vec3 p;
	glm::vec3 pNext;
	glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 posUp = glm::vec3(0.f, 5.0f, 0.f);
	glm::vec3 posTop = glm::vec3(0.f, 500.0f, 0.f);
	glm::vec3 posCartUp = glm::vec3(0.0f, 3.f, 0.f);
	glm::vec3 posLight = glm::vec3(0.0f, 0.0f, 5.f);

	m_pCatmullRom->Sample(m_currentDistance, p, up);
	m_pCatmullRom->Sample(m_currentDistance+0.1f, pNext, up);

	glm::vec3 t = glm::normalize(pNext - p);
	glm::vec3 n = glm::normalize(glm::cross(t, y));
	glm::vec3 b = glm::normalize(glm::cross(n, t));

	glm::vec3 upC = glm::rotate(glm::vec3(0, 1, 0), m_cameraRotation, t);

	switch (m_CAngle)
	{
		//First person
	case 1:
		m_pCamera->Set(p + posUp, p + 10.f * t, upC);
		break;
		//Top view
	case 2:
		m_pCamera->Set(p + posTop, p + 10.f * t, upC);
		break;
		//Third person
	case 3:
		m_pCamera->Set(p - 50.0f * t + 25.0f * b, p + 10.f * t, upC);
		break;
		//Left view
	case 4:
		m_pCamera->Set(p - 50.0f * n, p + 10.f * t, upC);
		break;
		//Right view
	case 5:
		m_pCamera->Set(p + 50.0f * n, p + 10.f * t, upC);
		break;
		//Free view (debug)
	case 6:
		m_pCamera->Update(m_dt);
		break;
	}

	m_CartPosition = p + posCartUp + sidePos * n;
	m_CartOrientation = glm::mat4(glm::mat3(t, b, n));
	m_SpotLightPosition = p + posLight + sidePos * n;

	m_Time += 1;

	m_spin += 0.01f;

	m_pAudio->Update();

	Collision();
}



void Game::DisplayFrameRate()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	}
}

void Game::DisplaySpeed()
{
	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	

	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	//modelViewMatrix.Rotate(glm::vec3(1.0f, 0.0f, 0.f), glm::radians(90.0f));
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pFtFont->Render(20, height - 50, 20, "Speed: %d", m_CartSpeed);
	//m_pFtFont->Render(115, height - 50, 20, "mph");

}

void Game::DisplayTime()
{
	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	
	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	//modelViewMatrix.Rotate(glm::vec3(1.0f, 0.0f, 0.f), glm::radians(90.0f));
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pFtFont->Render(20, height - 80, 20, "Time: %d", m_Time);
	m_pFtFont->Render(20, height - 110, 20, "End Time: %d", m_LapTime);
	if ((glm::distance(glm::vec3(50.f, 10.f, 100.f), m_CartPosition) < 30.0f))
	{
		m_pFtFont->Render(300, height - 300, 20, "LAP FINISHED!");
	}
}

void Game::DisplayHUD()
{
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	CShaderProgram* fontProgram = (*m_pShaderPrograms)[0];
	//fontProgram->UseProgram();
	//fontProgram->SetUniform("sampler0", 0);

	/*
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	*/

	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	//modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(90.f));
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//fontProgram->SetUniform("transformationMatrix", modelViewMatrixStack.Top());
	m_pHUD->Render();
	
}

void Game::Collision()
{
	for (int i = 0; i < m_pPowerUp.size(); i++)
	{
		if ((glm::distance(m_pPowerUp[i]->GetPosition(), m_CartPosition) < 10.0f))
		{
			m_pCamera->IncreaseSpeed(0.1f);
			m_pPlayer->IncreaseSpeed(0.1f);
			m_CartSpeed += 2;
			m_pickedUpPower = 0.5f;
		}
		else if (((glm::distance(m_pPowerUp[i]->GetPosition(), m_CartPosition) > 30.0f)) && ((glm::distance(m_pPowerUp[i]->GetPosition(), m_CartPosition) < 40.0f)))
		{
			m_pickedUpPower = 0.0f;
		}
	}

	for (int i = 0; i < m_pPenalty.size(); i++)
	{
		if ((glm::distance(m_pPenalty[i]->GetPosition(), m_CartPosition) < 10.0f))
		{
			m_pCamera->DecreaseSpeed(0.05f);
			m_pPlayer->DecreaseSpeed(0.05f);
			m_CartSpeed -= 1;
			m_pickedUpPenalty = 1.0f;
		}
		else if (((glm::distance(m_pPenalty[i]->GetPosition(), m_CartPosition) > 30.0f)) && ((glm::distance(m_pPenalty[i]->GetPosition(), m_CartPosition) < 40.0f)))
		{
			m_pickedUpPenalty = 0.0f;
		}
	}

	//Sets lap time when cart reaches finish
	if ((glm::distance(glm::vec3(50.f, 10.f, 100.f), m_CartPosition) < 10.0f))
	{
		m_LapTime = m_Time;
	}
	if ((glm::distance(glm::vec3(30.f, 10.f, 100.f), m_CartPosition) < 10.0f))
	{
		m_pCamera->ResetSpeed(1.0f);
		m_pPlayer->ResetSpeed(1.0f);
		m_CartSpeed = 50;
		m_Time = 0;
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{

	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		//Steers left
		case VK_LEFT:
			/*
			m_cameraRotation -= 0.02f;
			if ((m_CAngle == 2) || (m_CAngle == 3)) {
				m_cameraRotation = 0.f;
			}
			else if (m_cameraRotation < -0.3f) {
				m_cameraRotation = -0.3f;
			}
			*/
			sidePos -= 0.3f;
			if (sidePos < -6.0f) {
				sidePos = -6.0f;
			}
			break;
		//Steers right
		case VK_RIGHT:
			/*
			m_cameraRotation += 0.02f;
			if ((m_CAngle == 2) || (m_CAngle == 3)) {
				m_cameraRotation = 0.f;
			}
			else if (m_cameraRotation > 0.3f) {
				m_cameraRotation = 0.3f;
			}
			break;
			*/
			sidePos += 0.3f;
			if (sidePos > 6.0f) {
				sidePos = 6.0f;
			}
			break;
		//Goes to top view
		case VK_SPACE:
			if (m_CAngle == 1) {
				m_cameraRotation = 0.f;
				m_CAngle = 2;
			}
			else if (m_CAngle == 2) {
				m_CAngle = 1;
			}
			break;
		//Goes to third person
		case 'S':
			if (m_CAngle == 1) {
				m_cameraRotation = 0.f;
				m_CAngle = 3;
			}
			else if (m_CAngle == 3) {
				m_CAngle = 1;
			}
			break;
		// Debug
		case 'A':
			if (m_CAngle == 1) {
				m_cameraRotation = 0.f;
				m_CAngle = 4;
			}
			else if ((m_CAngle == 4) || (m_CAngle == 5)) {
				m_CAngle = 1;
			}
			break;
		case 'D':
			if (m_CAngle == 1) {
				m_cameraRotation = 0.f;
				m_CAngle = 5;
			}
			else if ((m_CAngle == 5) || (m_CAngle == 4)) {
				m_CAngle = 1;
			}
			break;
		case 'P':
			m_CAngle = 6;
			break;
		case '1':
			m_pAudio->PlayEventSound();
			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
