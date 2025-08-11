#include "DefenseModeCameraManager.h"

#include <Camera.h>

GOTOEngine::DefenseModeCameraManager* GOTOEngine::DefenseModeCameraManager::instance = nullptr;

void GOTOEngine::DefenseModeCameraManager::Awake()
{
	if (instance == nullptr)
	{
		instance = this;

		auto p1CamGO = GameObject::Find(L"p1Cam");

		if (IsValidObject(p1CamGO))
		{
			m_p1Cam = p1CamGO->GetComponent<Camera>();
		}

		auto p2CamGO = GameObject::Find(L"p2Cam");

		if (IsValidObject(p2CamGO))
		{
			m_p2Cam = p2CamGO->GetComponent<Camera>();
		}

		auto commonCamGO = GameObject::Find(L"commonCam");

		if (IsValidObject(commonCamGO))
		{
			m_commonCam = commonCamGO->GetComponent<Camera>();
		}
	}
	else
	{
		Destroy(GetGameObject());
	}
}

void GOTOEngine::DefenseModeCameraManager::Update()
{

}

void GOTOEngine::DefenseModeCameraManager::OnDestroy()
{
	if (instance == this)
		instance = nullptr;
}