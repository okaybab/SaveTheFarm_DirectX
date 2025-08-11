#include "DefenseModeCameraManager.h"

GOTOEngine::DefenseModeCameraManager* GOTOEngine::DefenseModeCameraManager::instance = nullptr;

void GOTOEngine::DefenseModeCameraManager::Awake()
{
	if (instance == nullptr)
	{
		instance = this;
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