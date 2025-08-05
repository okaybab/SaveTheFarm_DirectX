#include "FadeInOutFXManager.h"
#include "Image.h"
#include <TimeManager.h>

GOTOEngine::FadeInOutFXManager* GOTOEngine::FadeInOutFXManager::instance = nullptr;

void GOTOEngine::FadeInOutFXManager::FadeIn()
{
	m_enableScreen = false;
}

void GOTOEngine::FadeInOutFXManager::FadeOut()
{
	m_enableScreen = true;
	blackScreen->SetEnabled(true);
}

bool GOTOEngine::FadeInOutFXManager::IsPerfectlyFadeOut()
{
	return IsValidObject(blackScreen) && m_enableScreen && m_fadingTimer >= fadingTime;
}

void GOTOEngine::FadeInOutFXManager::Awake()
{
	if (instance == nullptr)
	{
		instance = this;
		DontDestroyOnLoad(GetGameObject());
		m_fadingTimer = fadingTime;
	}
	else
	{
		Destroy(GetGameObject());
	}
}

void GOTOEngine::FadeInOutFXManager::Update()
{
	m_fadingTimer = Mathf::Clamp(m_fadingTimer + (m_enableScreen ? Mathf::Min(TIME_GET_FIXED_DELTATIME(), TIME_GET_DELTATIME()) : -Mathf::Min(TIME_GET_FIXED_DELTATIME(), TIME_GET_DELTATIME())), 0.0f, fadingTime);

	auto alpha = static_cast<uint8_t>(Mathf::Clamp(255.0f * (m_fadingTimer / fadingTime),0.0f,255.0f));

	if (IsValidObject(blackScreen))
	{
		blackScreen->SetColor({ 0, 0, 0, alpha });

		if (!m_enableScreen && alpha == 0)
		{
			blackScreen->SetEnabled(false);
		}
	}
}