#include "AudioListener.h"
#include "AudioManager.h"
#include "Transform.h"
#include "GameObject.h"
#include <iostream>

using namespace GOTOEngine;

AudioListener::AudioListener()
	: m_lastPosition(Vector2{ 0, 0 })
	, m_positionDirty(true)
{

	if (AudioManager::Get()->IsInitialized())
	{
		AudioManager::Get()->RegisterListener(this);
#ifdef _DEBUG_AUDIO
		std::cout << "AudioListener enabled." << std::endl;
#endif
	}
}

AudioListener::~AudioListener()
{
	if (AudioManager::Get()->IsInitialized())
	{
		AudioManager::Get()->UnRegisterListener(this);
#ifdef _DEBUG_AUDIO
		std::cout << "AudioListener disabled." << std::endl;
#endif
	}
}

void AudioListener::UpdateListenerPosition()
{
	if (!AudioManager::Get()->IsInitialized() || !GetTransform())
		return;

	Vector2 currentPosition = GetTransform()->GetPosition();

	if (currentPosition.x != m_lastPosition.x ||
		currentPosition.y != m_lastPosition.y ||
		m_positionDirty)
	{
		ma_engine* engine = AudioManager::Get()->GetEngine();

		// 리스너 위치 설정 (Z축은 0)
		ma_engine_listener_set_position(engine, 0, currentPosition.x, currentPosition.y, 0.0f);

		m_lastPosition = currentPosition;
		m_positionDirty = false;
	}
}

AudioListener* AudioListener::GetMain()
{
	return AudioManager::Get()->GetMainListener();
}

void AudioListener::SetAsMainListener()
{
	AudioManager::Get()->m_mainListener = this;
}