#define NOMINMAX

#include "AudioManager.h"
#include "AudioListener.h"
#include "AudioSource.h"
#include "AudioClip.h"
#include "WStringHelper.h"
#include <iostream>
#include <algorithm>
#include <chrono>

using namespace GOTOEngine;

void AudioManager::StartUp()
{
	// 오디오 엔진 초기화
	ma_engine_config config = ma_engine_config_init();
	config.periodSizeInFrames = 512;
	config.periodSizeInMilliseconds = 0;
	config.sampleRate = 48000;

	ma_result result = ma_engine_init(&config, &m_audioEngine);
	m_isInitialized = (result == MA_SUCCESS);
	m_mainListener = nullptr;
	m_audioUpdateTime = 0.0f;

#ifdef _DEBUG_AUDIO
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

	auto startTime = std::chrono::high_resolution_clock::now();

	// 메인 리스너 위치 업데이트
	if (Object::IsValidObject(m_mainListener) && m_mainListener->IsActiveAndEnabled())
	{
		m_mainListener->UpdateListenerPosition();
	}

	// AudioSource들의 백그라운드 업데이트 (자동 준비 포함)
	UpdateAudioSources();

	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	m_audioUpdateTime = duration.count() / 1000.0f; // 밀리초로 변환
}

void AudioManager::UpdateAudioSources()
{
	for (auto& source : m_audioSources)
	{
		if (Object::IsValidObject(source) && source->IsActiveAndEnabled())
		{
			// 위치 업데이트
			source->UpdateTransform();

			// 내부 상태 업데이트 (자동 준비 포함)
			source->InternalUpdate();
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

void AudioManager::NotifyClipUnloaded(AudioClip* clip)
{
	// 해당 클립을 사용하는 모든 AudioSource에게 다시 준비 필요하다고 알림
	for (auto& source : m_audioSources)
	{
		if (source && source->GetClip() == clip)
		{
			source->MarkNeedsPrepare();
		}
	}
}

void AudioManager::PreloadSceneAudioClips()
{
	if (!m_isInitialized) return;

#ifdef _DEBUG_AUDIO
	std::cout << "=== Preloading Scene Audio Clips ===" << std::endl;
#endif

	int preloadedCount = 0;
	size_t totalMemoryLoaded = 0;

	// 현재 씬의 모든 AudioSource를 순회하며 preloadAudioData 클립들을 로딩
	for (auto& source : m_audioSources)
	{
		if (source && source->GetClip())
		{
			AudioClip* clip = source->GetClip();

			// preloadAudioData가 true이고 DecompressOnLoad 모드인 클립만 로딩
			if (clip->GetPreloadAudioData() &&
				clip->GetLoadMode() == AudioLoadMode::DecompressOnLoad &&
				!clip->IsAudioDataLoaded())
			{
				clip->ForcedLoadAudioData();
				if (clip->IsAudioDataLoaded())
				{
					preloadedCount++;
					totalMemoryLoaded += clip->GetMemoryUsage();

					// 로딩 후 해당 AudioSource도 준비 필요 상태로 설정
					source->MarkNeedsPrepare();

#ifdef _DEBUG_AUDIO
					std::cout << "Preloaded: " << WStringHelper::wstring_to_string(clip->GetFilePath())
						<< " (" << (clip->GetMemoryUsage() / 1024.0f) << " KB)" << std::endl;
#endif
				}
			}
		}
	}

#ifdef _DEBUG_AUDIO
	std::cout << "Scene preload complete: " << preloadedCount << " clips, "
		<< (totalMemoryLoaded / 1024.0f / 1024.0f) << " MB total" << std::endl;
	std::cout << "==============================" << std::endl;
#endif
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

AudioStats AudioManager::GetAudioStats() const
{
	AudioStats stats;

	// 활성/재생 중인 소스 수 계산
	for (const auto& source : m_audioSources)
	{
		if (source)
		{
			stats.activeSources++;
			if (source->IsPlaying())
				stats.playingSources++;
			if (source->IsSoundReady())
				stats.readySources++;
		}
	}

	return stats;
}

void AudioManager::LogAudioStats() const
{
	AudioStats stats = GetAudioStats();

#ifdef _DEBUG_AUDIO
	std::cout << "=== Audio Statistics ===" << std::endl;
	std::cout << "Active Sources: " << stats.activeSources << std::endl;
	std::cout << "Playing Sources: " << stats.playingSources << std::endl;
	std::cout << "Ready Sources: " << stats.readySources << std::endl;
	std::cout << "Audio Update Time: " << m_audioUpdateTime << "ms" << std::endl;
	std::cout << "========================" << std::endl;
#endif
}