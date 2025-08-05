#define NOMINMAX

#include "AudioManager.h"
#include "AudioListener.h"
#include "AudioSource.h"
#include <iostream>
#include <algorithm>

using namespace GOTOEngine;

void AudioManager::StartUp()
{
	// 오디오 끊김 방지를 위한 설정
	ma_engine_config config = ma_engine_config_init();

	config.periodSizeInFrames = 512;      // 버퍼 크기 조정
	config.periodSizeInMilliseconds = 0;  // 프레임 기준 사용

	ma_result result = ma_engine_init(NULL, &m_audioEngine);
	m_isInitialized = (result == MA_SUCCESS);
	m_mainListener = nullptr;

#ifdef _DEBUG
	if (m_isInitialized)
		std::cout << "AudioManager initialized successfully." << std::endl;
	else
		std::cout << "Failed to initialize AudioManager." << std::endl;
#endif
}

void AudioManager::ShutDown()
{
	if (m_isInitialized)
	{
		m_listeners.clear();
		m_audioSources.clear();
		m_mainListener = nullptr;
		ma_engine_uninit(&m_audioEngine);
		m_isInitialized = false;
	}
}

void AudioManager::Update()
{
	if (!m_isInitialized) return;

	// 메인 리스너 위치 업데이트
	if (m_mainListener && m_mainListener->GetEnabled())
	{
		m_mainListener->UpdateListenerPosition();
	}

	// 모든 AudioSource 위치 업데이트
	for (auto& source : m_audioSources)
	{
		if (source && source->GetEnabled())
		{
			source->UpdateTransform();
		}
	}
}

void AudioManager::RegisterListener(AudioListener* listener)
{
	if (!listener) return;

	m_listeners.push_back(listener);

	// 첫 번째 리스너를 메인으로 설정
	if (!m_mainListener)
		m_mainListener = listener;
}

void AudioManager::UnRegisterListener(AudioListener* listener)
{
	if (!listener) return;

	auto it = std::find(m_listeners.begin(), m_listeners.end(), listener);
	if (it != m_listeners.end())
	{
		m_listeners.erase(it);

		// 메인 리스너가 제거되면 다른 리스너로 교체
		if (m_mainListener == listener)
		{
			m_mainListener = m_listeners.empty() ? nullptr : m_listeners[0];
		}
	}
}

void AudioManager::RegisterAudioSource(AudioSource* source)
{
	if (!source) return;
	m_audioSources.push_back(source);
}

void AudioManager::UnRegisterAudioSource(AudioSource* source)
{
	if (!source) return;

	auto it = std::find(m_audioSources.begin(), m_audioSources.end(), source);
	if (it != m_audioSources.end())
	{
		m_audioSources.erase(it);
	}
}

void AudioManager::SetMasterVolume(float volume)
{
	if (!m_isInitialized) return;
	volume = std::max(0.0f, std::min(1.0f, volume));
	ma_engine_set_volume(&m_audioEngine, volume);
}

float AudioManager::GetMasterVolume() const
{
	if (!m_isInitialized) return 0.0f;
	return ma_engine_get_volume(const_cast<ma_engine*>(&m_audioEngine));
}