#include "Player.h"
#include "OpenAssetImportMesh.h"
#include "gamewindow.h"
#include "Camera.h"

CPlayer::CPlayer()
{
	m_position = glm::vec3(0, 0, 0);
	m_strafeVector = glm::vec3(0, 0, 0);

	m_speed = 0.1f;
	m_currentDistance = 0.0f;
	m_sidePos = 0.0f;


	m_pShipMesh = new COpenAssetImportMesh;
}

CPlayer::~CPlayer()
{
}

void CPlayer::Create()
{
	m_pShipMesh->Load("resources\\models\\Cart\\Cart.obj");
}

void CPlayer::Update(double dt)
{
	//m_currentDistance += dt * m_pCamera->GetSpeed();


	//TranslateByKeyboard(dt, m_pCatmullRom->W);
	//m_pCamera->SetPlayerSidePos(m_sidePos);
}


void CPlayer::TranslateByKeyboard(double dt, float w)
{

}

void CPlayer::Release()
{
}

void CPlayer::Strafe(double)
{
}

COpenAssetImportMesh* CPlayer::GetMesh()
{
	return m_pShipMesh;
}

glm::vec3 CPlayer::GetPosition()
{
	return m_position;
}

float CPlayer::GetCurrentDist()
{
	return m_currentDistance;
}

void CPlayer::IncreaseSpeed(float s)
{
	m_speed += s;
}

void CPlayer::DecreaseSpeed(float s)
{
	m_speed -= s;
}

void CPlayer::ResetSpeed(float s)
{
	m_speed = s;
}