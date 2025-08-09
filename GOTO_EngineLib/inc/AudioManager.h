#pragma once
#include "Singleton.h"
#include <vector>
#include <miniaudio.h>

namespace GOTOEngine
{
	class AudioListener;
	class AudioSource;
	class AudioClip;

	struct AudioStats
	{
		size_t activeSources = 0;        // 활성화된 오디오 소스 수
		size_t playingSources = 0;       // 재생 중인 오디오 소스 수
		size_t readySources = 0;         // 재생 준비된 소스 수
	};

	class AudioManager : public Singleton<AudioManager>
	{
	private:
		friend class Engine;
		friend class AudioListener;
		friend class AudioSource;
		friend class AudioClip;

		ma_engine m_audioEngine;
		bool m_isInitialized;

		std::vector<AudioListener*> m_listeners;
		std::vector<AudioSource*> m_audioSources;
		AudioListener* m_mainListener;

		// 성능 모니터링
		float m_audioUpdateTime;

		void StartUp();
		void ShutDown();
		void Update();
		void UpdateAudioSources();

		void RegisterListener(AudioListener* listener);
		void UnRegisterListener(AudioListener* listener);
		void RegisterAudioSource(AudioSource* source);
		void UnRegisterAudioSource(AudioSource* source);

		// AudioClip 언로드 알림
		void NotifyClipUnloaded(AudioClip* clip);

	public:
		ma_engine* GetEngine() { return &m_audioEngine; }
		bool IsInitialized() const { return m_isInitialized; }
		AudioListener* GetMainListener() { return m_mainListener; }

		void SetMasterVolume(float volume);
		float GetMasterVolume() const;

		// 씬 로드시 preloadAudioData 클립들 프리로딩
		void PreloadSceneAudioClips();

		// 통계 및 디버깅
		AudioStats GetAudioStats() const;
		float GetAudioUpdateTime() const { return m_audioUpdateTime; }
		void LogAudioStats() const;
	};
}